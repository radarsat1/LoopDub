
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "loopdub.h"
#include "ld_midi.h"

static char *Types[N_CT] = { "Level", "Effect", "Button", "Select" };
static char *configfile = ".loopdub.midi.conf";

MidiControl::MidiControl()
{
	 m_bInitialized = false;
	 m_pmListen = NULL;

	 int ch, t;
	 for (ch=0; ch<N_LOOPS; ch++)
		  for (t=0; t<N_CT; t++)
			   m_ctrlcode[ch][t]=0;

	 m_bLearning = false;

	 FILE *file = fopen(configfile, "r");
	 if (!file) {
		  printf("Couldn't open %s\n", configfile);
		  return;
	 }

	 // Read MIDI configuration
	 char line[1024], *str;
	 char delim[] = "\r\n\t =.";
	 int i, linenumber=0;
	 bool error=false;
	 while (fgets(line, 1024, file)) {
		  i=0;
		  t = -1;
		  ch = -1;
		  str = strtok(line, delim);
		  while (str) {
			   switch (i) {
			   case 0:
					for (t=0; t<N_CT; t++)
						 if (strcmp(str, Types[t])==0)
							  break;
					if (t >= N_CT) t = -1;
					i++;
					break;
			   case 1:
					ch = atoi(str);
					if (ch < 0) ch = -1;
					if (ch >= N_LOOPS) ch = -1;
					i++;
					break;
			   case 2:
					if ((ch!=-1) && (t!=-1)) {
						 m_ctrlcode[ch][t] = atoi(str);
					} else {
						 printf("Error on line %d of %s\n", linenumber, configfile);
					}
					i++;
					break;
			   }			   
			   str = strtok(NULL, delim);
		  }
		  linenumber++;
	 }
	 fclose(file);

	 printf(error ? "There were errors reading the MIDI configuration.\n"
			: "MIDI configuration read successfully.\n");
}

MidiControl::~MidiControl()
{
	 if (m_bInitialized)
		  Pm_Terminate();

	 FILE *file = fopen(configfile, "w");
	 if (!file) return;

	 int ch, t;
	 for (t=0; t<N_CT; t++)
		  for (ch=0; ch<N_LOOPS; ch++)
			   fprintf(file, "%s.%d = %d\n", Types[t], ch, m_ctrlcode[ch][t]);

	 fclose(file);
}

bool MidiControl::Initialize()
{
	 if (!m_bInitialized) {
		  if (Pm_Initialize()==pmNoError)
			   m_bInitialized = true;
		  else
			   printf("Couldn't initialize PortMidi.\n");
	 }

	 return m_bInitialized;
}

int MidiControl::GetMidiNum()
{
	 if (!m_bInitialized)
		  return 0;

	 int n = Pm_CountDevices();

	 if (n==pmNoDevice) n = 0;
	 return n;
}
	 
char* MidiControl::GetMidiName(int n)
{
	 if (!m_bInitialized)
		  return "";

	 const PmDeviceInfo *pdi = Pm_GetDeviceInfo(n);
	 if (!pdi) return "";

	 // Note: portmidi.h says that this is safe
	 if (!pdi->input) return "!";
	 return (char*)pdi->name;
}

void MidiControl::SelectDevice(int n)
{
	 if (!m_bInitialized)
		  return;

	 if (m_pmListen)
		  Pm_Close(m_pmListen);

	 PmError err = Pm_OpenInput( &m_pmListen,
								 n,
								 NULL,
								 256,
								 0,
								 NULL);

	 if (err != pmNoError)
	 {
		  printf("Error selecting device %d.\n", n);
		  m_pmListen = NULL;
	 }
	 else
		  printf("Selected device %d\n", n);
}

void MidiControl::SetLearningMode(bool bLearnMode)
{
	 m_bLearning = bLearnMode;
	 m_nLearnCh = 0;
	 m_nLearnType = 0;
	 m_nLastCode = 0;
	 if (m_bLearning)
	 {
		  for (int ch=0; ch<N_LOOPS; ch++)
			   for (int t=0; t<N_CT; t++)
					m_ctrlcode[ch][t]=0;
	 }

	 int c = m_bLearning ? 3 : 1;
	 switch (m_nLearnType) {
	 case CT_LEVEL:
		  app.m_pLoopOb[m_nLearnCh]->GetVolumeSlider()->SetColor(c);
		  break;
	 case CT_EFFECT:
		  app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(0)->SetColor(c);
		  break;
	 }
}

void MidiControl::CheckMsg()
{
	 if (!m_pmListen) return;

	 // Don't block
	 PmEvent event;
  	 while (Pm_Poll(m_pmListen)==TRUE
			&& Pm_Read(m_pmListen, &event, 1)>pmNoError)
	 {
		  int code = Pm_MessageData1(event.message);
		  int val = Pm_MessageData2(event.message);
		  int status = Pm_MessageStatus(event.message);
		  int channel = status & 0x0F;
		  status &= 0xF0;
//		  status = status & Pm_MessageStatus(event.message);
//		  if (channel != /*this channel*/0)
//			   continue;

//		  printf("MIDI Message: status=%d, code=%d, val=%d\n", status, code, val);

		  if (m_bLearning) {
			   if (code==m_nLastCode) continue;
			   m_nLastCode = code;
			   if (code!=m_ctrlcode[m_nLearnCh][m_nLearnType]) {
					m_ctrlcode[m_nLearnCh][m_nLearnType] = code;
					
					switch (m_nLearnType) {
					case CT_LEVEL:
						 app.m_pLoopOb[m_nLearnCh]->GetVolumeSlider()->SetColor(1);
						 break;
					case CT_EFFECT:
						 app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(0)->SetColor(1);
						 break;
					}

					m_nLearnCh++;
					if (m_nLearnCh >= N_LOOPS) {
						 m_nLearnCh = 0;
						 m_nLearnType++;
						 if (m_nLearnType >= N_CT)
							  m_bLearning = false;
					}
					if (m_bLearning) {
						 switch (m_nLearnType) {
						 case CT_LEVEL:
							  app.m_pLoopOb[m_nLearnCh]->GetVolumeSlider()->SetColor(3);
							  break;
						 case CT_EFFECT:
							  app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(0)->SetColor(3);
							  break;
						 }
					}
					else
						 app.m_pMidiLearning->SetPressed(false);
			   }
		  }
		  else {
			   if (status==0xB0) // Control Change
			   {
					for (int ch=0; ch<N_LOOPS; ch++)
					{
						 if (m_ctrlcode[ch][CT_LEVEL]==code)
							  app.m_pLoopOb[ch]->SetVolume(val*100/0x7F);
						 else if (m_ctrlcode[ch][CT_EFFECT]==code) {
							  // TODO: this is fucked, make it better
							  for (int i=0; i<N_LOOPS; i++) {
								   Slider *slider = app.m_pLoopOb[i]->GetEffectSlider(ch);
								   if (slider && app.m_pLoopOb[i]->IsSelected())
										slider->SetValue(val*slider->GetMaxValue()/0x7F);
							  }
						 }
						 else if (m_ctrlcode[ch][CT_BUTTON]==code) {
							  switch (m_nButtonMode) {
							  case BT_CUE:
								   app.m_pLoopOb[ch]->m_pCueButton->SetPressed(true);
								   break;
							  case BT_HOLD:
								   app.m_pLoopOb[ch]->SetHolding(
										!app.m_pLoopOb[ch]->IsHolding());
								   break;
							  case BT_SPLIT:
								   app.m_pLoopOb[ch]->m_pSplitButton->SetPressed(true);
								   break;
							  }
						 }
						 else if (m_ctrlcode[ch][CT_SELECT]==code) {
							  m_nButtonMode = ch;
							  if (ch >= N_BT) ch = 0;
						 }
					}
			   }
			   else if (status==0x90) // Key On
			   {
					// TODO: why the hell does my midi keyboard send the NOTE ON
					//       message when i release a key, with velocity of zero?
					if (val==0) {
						 // note off:
						 // find key for this note
						 int i;
						 for (i=0; i<MAX_KEYS && !(app.m_Keys[i].on && app.m_Keys[i].note==code); i++);
						 if (i < MAX_KEYS)
							  app.m_Keys[i].on = false;
					}
					else {
						 // note on:

						 // find keychannel
						 int i;
						 for (i=0; i<N_LOOPS && !app.m_pLoopOb[i]->HasKeys(); i++);
						 if (i<N_LOOPS) {
							  // find unused key
							  for (i=0; i<MAX_KEYS && app.m_Keys[i].on; i++);
							  if (i < MAX_KEYS) {
								   double freq = 440.0 * pow(2.0, (code-69)/12.0);
								   double ratio = 337.12301487 * freq / SAMPLE_RATE;
								   app.m_Keys[i].velocity = (int)(ratio*1024.0);
								   app.m_Keys[i].position = 0;
								   app.m_Keys[i].note = code;
								   app.m_Keys[i].on = true;
							  }
						 }
						 else {
							  // no keychannels selected, therefore keys control
							  // channel selection
							  code = code%12;
							  for (i=0; i<N_LOOPS; i++)
								   app.m_pLoopOb[i]->SetSelected(i==code);
						 }
					}
			   }
			   else if (status==0xC0) // Program change
			   {
					int program = (code & 0x7F);
					// program+= 1; // note: PCR-30 sends 0 when it says "1" in the display??
					printf("MIDI Program Change: %d\n", program);
					app.m_ProgramChanger.ProgramChange(program, app.m_pLoopOb);
			   }
		  }
	 }
	 return;
}
