
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "loopdub.h"
#include "ld_midi.h"
#include "settings.h"

static char *Types[N_CT] = { "Level", "Effect1", "Effect2", "Effect3",
							 "Effect4", "Button", "Select" };
static char *configfile = ".loopdub.midi.conf";

MidiControl::MidiControl()
{
	 m_bInitialized = false;
	 m_pMidiIn = NULL;
	 m_pMidiOut = NULL;

	 int ch, t;
	 for (ch=0; ch<N_LOOPS; ch++)
		  for (t=0; t<N_CT; t++)
			   m_ctrlcode[ch][t]=0;

	 m_bLearning = false;
	 m_bMidiClockActive = false;
	 m_bMidiClockWaiting = false;

	 LoadConfiguration();
}

void MidiControl::LoadConfiguration()
{
	 int ch, t;
	 char configfilename[MAX_PATH];
//#ifndef WIN32
#if 1
	 sprintf(configfilename, "%s/%s", getenv("HOME"), configfile);
#else
	 // TODO fix this to User folder
	 strcpy(configfilename, configfile);
#endif

	 FILE *file = fopen(configfilename, "r");
	 if (!file) {
		  printf("Couldn't open %s\n", configfile);
		  return;
	 }

	 // Read MIDI configuration
	 SettingsFile f(configfilename);
	 char param[256];
	 char subparam[10];
	 char value[1024];
	 while (f.ReadSetting(0,0,param,256,subparam,10,value,1024)) {
		  t = -1;
		  ch = -1;
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
	 if (m_bInitialized && m_pMidiIn) {
		  delete m_pMidiIn;
		  m_pMidiIn = NULL;
	 }

	 if (m_bMidiCodesHaveChanged) {
		  FILE *file = fopen(configfile, "w");
		  if (!file) return;

		  int ch, t;
		  for (t=0; t<N_CT; t++)
			   for (ch=0; ch<N_LOOPS; ch++)
					fprintf(file, "%s.%d = %d\n", Types[t], ch, m_ctrlcode[ch][t]);

		  fclose(file);
		  printf("Wrote MIDI configuration to %s\n", configfile);
	 }
}

bool MidiControl::Initialize()
{
	 if (!m_bInitialized) {
		  try {
			   m_pMidiIn = new RtMidiIn();
			   if (m_pMidiIn)
					m_bInitialized = true;
			   else
					printf("Couldn't initialize RtMidi.\n");
		  }
		  catch (RtError &e) {
			   e.printMessage();
			   if (m_pMidiIn) delete m_pMidiIn;
			   m_pMidiIn = NULL;
		  }
	 }

	 return m_bInitialized;
}

int MidiControl::GetMidiNum()
{
	 if (!(m_bInitialized && m_pMidiIn))
		  return 0;

	 int n = m_pMidiIn->getPortCount();

	 return n;
}
	 
const char* MidiControl::GetMidiName(int n)
{
	 if (!(m_bInitialized && m_pMidiIn))
		  return "";

	 const char *s = m_pMidiIn->getPortName(n).c_str();
	 if (!s) s = "";
	 return s;
}

MidiType MidiControl::GetMidiType(int n)
{
	 if (!m_bInitialized)
		  return MidiUnknown;

	 /*
	 const PmDeviceInfo *pdi = Pm_GetDeviceInfo(n);
	 if (!pdi) return MidiUnknown;

	 if (pdi->input)
		  return MidiInput;
	 else
	 */
	 return MidiInput;
}

/*
PmTimestamp MidiControl::timeProc(void* time_info)
{
	 return app.m_Player.GetPlayPositionSamples()*1000/44100;
}
*/

void MidiControl::SelectDevice(int n)
{
	 if (!(m_bInitialized && m_pMidiIn))
		  return;

	 char *stype=NULL;

	 switch (GetMidiType(n)) {
	 case MidiInput:
		  stype = "input";
		  try {
			   m_pMidiIn->openPort(n);
		  } catch (RtError &e) {
			   e.printMessage();
		  }
		  break;
	 case MidiOutput:
		  stype = "output";
		  /*
		  if (m_pmOutput)
			   Pm_Close(m_pmOutput);

		  // Note: 1ms latency tells PortMidi to wait 1 ms + timestamp before sending MIDI message
		  err = Pm_OpenOutput( &m_pmOutput, n, NULL, 10, timeProc, NULL, 1);
		  if (err != pmNoError)
			   m_pmOutput = NULL;
		  */
		  break;
	 default:
		  return;
	 }
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
	 case CT_EFFECT1:
		  app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(0)->SetColor(c);
		  break;
	 case CT_EFFECT2:
		  app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(1)->SetColor(c);
		  break;
	 case CT_EFFECT3:
		  app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(3)->SetColor(c);
		  break;
	 }
}

void MidiControl::CheckMsg()
{
	 if (!m_pMidiIn)
		  return;

#if 0
	 // Don't block
	 PmEvent event;
	 PmError rc;
  	 while (Pm_Poll(m_pmListen)==TRUE
			&& (rc=Pm_Read(m_pmListen, &event, 1))>=pmNoError)
	 {
		  int code = Pm_MessageData1(event.message);
		  int val = Pm_MessageData2(event.message);
		  int status = Pm_MessageStatus(event.message);
		  int channel = status & 0x0F;
		  status &= 0xF0;
//		  status = status & Pm_MessageStatus(event.message);
//		  if (channel != /*this channel*/0)
//			   continue;

		  printf("MIDI Message: status=%d, code=%d, val=%d\n", status, code, val);

		  if (m_bLearning) {
			   if (code==m_nLastCode) continue;
			   m_nLastCode = code;
			   if (code!=m_ctrlcode[m_nLearnCh][m_nLearnType]) {
					m_ctrlcode[m_nLearnCh][m_nLearnType] = code;
					m_bMidiCodesHaveChanged = true;
					
					switch (m_nLearnType) {
					case CT_LEVEL:
						 app.m_pLoopOb[m_nLearnCh]->GetVolumeSlider()->SetColor(1);
						 break;
					case CT_EFFECT1:
						 app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(0)->SetColor(1);
						 break;
					case CT_EFFECT2:
						 app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(1)->SetColor(1);
						 break;
					case CT_EFFECT3:
						 app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(2)->SetColor(1);
						 break;
					case CT_EFFECT4:
						 app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(3)->SetColor(1);
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
						 case CT_EFFECT1:
							  app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(0)->SetColor(3);
							  break;
						 case CT_EFFECT2:
							  app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(1)->SetColor(3);
							  break;
						 case CT_EFFECT3:
							  app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(2)->SetColor(3);
							  break;
						 case CT_EFFECT4:
							  app.m_pLoopOb[m_nLearnCh]->GetEffectSlider(3)->SetColor(3);
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
						 else if (m_ctrlcode[ch][CT_EFFECT1]==code) {
							  /*
							  for (int i=0; i<N_LOOPS; i++) {
								   Slider *slider = app.m_pLoopOb[i]->GetEffectSlider(ch);
								   if (slider && app.m_pLoopOb[i]->IsSelected())
										slider->SetValue(val*slider->GetMaxValue()/0x7F);
							  }
							  */
							  Slider *slider = app.m_pLoopOb[ch]->GetEffectSlider(0);
							  slider->SetValue(val*slider->GetMaxValue()/0x7F);
						 }
						 else if (m_ctrlcode[ch][CT_EFFECT2]==code) {
							  Slider *slider = app.m_pLoopOb[ch]->GetEffectSlider(1);
							  slider->SetValue(val*slider->GetMaxValue()/0x7F);
						 }
						 else if (m_ctrlcode[ch][CT_EFFECT3]==code) {
							  Slider *slider = app.m_pLoopOb[ch]->GetEffectSlider(2);
							  slider->SetValue(val*slider->GetMaxValue()/0x7F);
						 }
						 else if (m_ctrlcode[ch][CT_EFFECT4]==code) {
							  Slider *slider = app.m_pLoopOb[ch]->GetEffectSlider(3);
							  slider->SetValue(val*slider->GetMaxValue()/0x7F);
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
							  case BT_NORM:
								   app.m_pLoopOb[ch]->m_pNormalizeButton->SetPressed(true);
								   break;
							  case BT_SWITCH:
								   app.m_pLoopOb[ch]->SetSwitching(
										!app.m_pLoopOb[ch]->IsSwitching());
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
	 
	 if (rc < 0)
		  printf("rc: %d\n", rc);

#endif
	 return;
}

void MidiControl::SendClockTick(long ms, bool startnow)
{
/*
	 if (!m_pmOutput)
		  return;

	 if (m_bMidiClockWaiting && startnow) {
		  printf("MIDI clock starting...\n");
		  Pm_WriteShort(m_pmOutput, 0, 0xFA);
		  m_bMidiClockWaiting = false;
		  m_bMidiClockActive = true;
	 }
	 
	 if (m_bMidiClockActive)
		  Pm_WriteShort(m_pmOutput, ms, 0xF8);
*/
}

void MidiControl::UpdateClockTicks()
{
/*
	 if (!m_pmOutput)
		  return;

	 if (app.m_pMidiClock->IsPressed() && !m_bMidiClockActive && !m_bMidiClockWaiting) {
		  m_bMidiClockWaiting = true;
		  printf("MIDI clock waiting...\n");
	 }
	 else if (app.m_pMidiClock->IsPressed() && !m_bMidiClockActive && m_bMidiClockWaiting) {
		  // check if time to start
		  // send start
//		  m_bMidiClockActive = true;
//		  m_bMidiClockWaiting = false;
	 }
	 else if (!app.m_pMidiClock->IsPressed() && m_bMidiClockActive) {
		  // send stop
		  m_bMidiClockActive = false;
		  m_bMidiClockWaiting = false;

		  printf("MIDI clock stopping...\n");
		  Pm_WriteShort(m_pmOutput, 0, 0xFC);
	 }
*/

//		  int now_sample = app.m_Player.GetPlayPositionSamples();
/*
	int beat_sample_interval = (44100*60)/(135*24);
	
	SendClockTick(beat_sample_interval);
*/
}
