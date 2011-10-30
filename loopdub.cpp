
#define _LOOPDUB_CPP_
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "loopdub.h"
#include "ld_logo.h"
#include "platform.h"
#include "settings.h"

static char *configfile = ".loopdub.conf";

class Timer
{
public:
	 void init()
		  {
		    #ifdef TIMING
			   time = clock();
			   avgtime=0;
			   count=0;
		    #endif
		  }

	 void reinit()
		  {
			   return;
			   //gettimeofday(&time,NULL);
		  }

	 double elapsed()
		  {
		    #ifdef TIMING
			   clock_t t;
			   //if (count<100000)

					t = clock();

			   /*
			   struct timeval t;
			   double result = (t.tv_sec + t.tv_usec/1000000.0) - (time.tv_sec + t.tv_usec/1000000.0);
			   time.tv_sec  = t.tv_sec;
			   time.tv_usec = t.tv_usec;
			   */

			   double result = t - time;
			   time = t;

			   avgtime = (avgtime*count + result) / (count+1);
			   count++;
			   return result;
       		   #else
			   return 0;
       		   #endif
		  }

protected:
	 clock_t time;

public:
	 int count;
	 double avgtime;
};

Timer timer[10];

/* One global LoopDub object */
LoopDub app;

/*
** LoopDub
*/

#define APP_VERSION _BUILDVER
#define LOOPTOP    50
#define LOOPHEIGHT 60

int count=0;

LoopDub::LoopDub()
	 : m_cfgInternalSampleRate(DEFAULT_SAMPLE_RATE),
	   m_cfgHardwareSampleRate(DEFAULT_HW_SAMPLE_RATE)
{
	 m_strChangeToFolder = NULL;
     m_strRecordPath = NULL;
     m_fRecordFile = 0;
	 m_nBeats = 4;
	 m_nPos = 0;
	 m_nLength = 0;
	 m_pMidiLearning = NULL;
	 m_pMidiClock = NULL;
	 m_nKeysChannel = -1;
	 for (int i=0; i<MAX_KEYS; i++)
		  m_Keys[i].on = false;

timer[0].init();
timer[1].init();

	 CREATEMUTEX(mutex);
}

LoopDub::~LoopDub()
{
    if (m_fRecordFile)
        fclose(m_fRecordFile);
	DESTROYMUTEX(mutex);
}

bool priority = false;
void LoopDub::FillBuffers(void *param, int outTimeSample)
{
	LoopDub& app = *(LoopDub*)param;

	if (!priority) {
		 HIGHPRIORITY();
		 priority = true;
	}

	// TODO: Is mutex necessary?  seems not, but requires proof
	//LOCKMUTEX(app.mutex);

	// MIDI clock
	int ticksize = 816;
	int first = app.m_nPos % ticksize;
	bool startnow = (app.m_nPos == first);
	int i, n=app.m_Player.BufferSizeSamples();

	for (i=first; i<n; i += ticksize) {
		 int ms = (outTimeSample + i) * 1000 / Player::m_nSampleRate;
		 app.m_Midi.SendClockTick(ms, startnow);
	}

	// Audio
	float* pStereoBuffer = app.m_Player.StereoBuffer();
	int maxval=0;
	int volume, volmax;

	volume = app.m_pVolumeSlider->GetValue();
	volmax = app.m_pVolumeSlider->GetValueMax();

	while (n-- > 0)
	{
timer[0].reinit();
		 int value[2];
		 int side=0;
		 value[0] = value[1] = 0;
		for (i=0; i<N_LOOPS; i++)
		{
timer[1].reinit();
			 side = 0;//app.m_pLoopOb[i]->IsCue() ? 1 : 0; (TODO: cueing disabled for now)
			 if (i!=app.m_nKeysChannel || !app.m_Keys[0].on) {
				  value[side] += app.m_pLoopOb[i]->GetSampleValue(app.m_nPos);
			 }
			 else {
				  for (int key=0; key <MAX_KEYS; key++) {
					   if (app.m_Keys[key].on) {
							value[side] += app.m_pLoopOb[i]->GetSampleValue(app.m_Keys[key].position >> 10);
							app.m_Keys[key].position += app.m_Keys[key].velocity;
							if ((app.m_Keys[key].position >> 10) > (app.m_pLoopOb[i]->GetSample()->m_nSamples))
								 app.m_Keys[key].on = false;
					   }
				  }
			 }
timer[1].elapsed();
		}

		for (int i=0; i<2; i++) {
			 value[i] = value[i] * volume / volmax;
             if (value[i] > SHRT_MAX) value[i] = SHRT_MAX;
             else if (value[i] < SHRT_MIN) value[i] = SHRT_MIN;
			 if (value[i] > maxval) maxval = value[i];
		}

        // Record to file
        if (app.m_fRecordFile) {
            short svf = (short)value[0];
            fwrite(&svf, sizeof(short), 1, app.m_fRecordFile);
        }

        // Mix to mono
        float vf = value[0] / 32768.0f;
		*(pStereoBuffer++) = vf;
        *(pStereoBuffer++) = vf;

		if (++app.m_nPos > app.m_nLength)
			 app.m_nPos = 0;

timer[0].elapsed();
	}

	app.m_pVUMeter->SetPercentage(maxval * 100 / 32767);

	if (app.updated) {
		 SDL_Event evt;
		 evt.type = SDL_USEREVENT;
		 SDL_PushEvent(&evt);
		 app.updated = false;
	}

	//UNLOCKMUTEX(app.mutex);

}

void LoopDub::SyncMidi()
{
	for (int i = 0; i < CONTROLS; i++) {
		LoopOb *loop = m_pLoopOb[i];

		for (int j = 0; j < 5; j++) {
			Slider *slider;

			if (j == 0) {
				slider = loop->GetVolumeSlider();
			} else {
				slider = loop->GetEffectSlider(j - 1);
			}

			m_Midi.SendControlMsg(i, j, slider->GetValue() * 127 / slider->GetValueMax());
		}
	}
}

void LoopDub::LoadConfiguration()
{
	m_cfgDefaultVolume = CFG_DEFAULT_DEFAULT_VOLUME;
	m_cfgDefaultButton = CFG_DEFAULT_DEFAULT_BUTTON;

	bool error = false;
	int line = 1;
	char configfilename[MAX_PATH];
	sprintf(configfilename, "%s/%s", getenv("HOME"), configfile);

	SettingsFile f(configfilename);
	while (f.ReadSetting()) {
		if (!strcmp(f.m_strParam, "DefaultVolume")) {
			m_cfgDefaultVolume = atoi(f.m_strValue);
		} else if (!strcmp(f.m_strParam, "DefaultButton")) {
			m_cfgDefaultButton = atoi(f.m_strValue);
		} else if (!strcmp(f.m_strParam, "InternalSampleRate")) {
			m_cfgInternalSampleRate = atoi(f.m_strValue);
		} else if (!strcmp(f.m_strParam, "HardwareSampleRate")) {
			m_cfgHardwareSampleRate = atoi(f.m_strValue);
		} else {
			printf("Error on line %d of %s\n", line, configfile);
		}
		line++;
	}
}

THREADFUNC loadSampleThread(void* pApp)
{
#ifndef WIN32
	 pthread_detach(pthread_self());
#endif

	 LOWPRIORITY();

	if (!pApp) return NULL;

	 LoopDub &app = *(LoopDub*)pApp;

	 Sample *pSample = new Sample();
	 if (pSample)
	 {
		if (!pSample->LoadFromFile(app.m_strLoadingSample))
			printf("Error loading %s.\n", app.m_strLoadingSample);
		else {
		  LOCKMUTEX(app.mutex);
		  if (app.m_nLength==0) app.m_nLength = pSample->m_nSamples;
		  Sample *pOldSample = app.m_pLoopOb[app.m_nLoadingSampleFor]->SetSample(pSample);
		  if (pOldSample)
			   delete pOldSample;
		  UNLOCKMUTEX(app.mutex);
		}
	 }

	 return NULL;
}

int LoopDub::Run()
{
	 int i;
	 int inputchars=0;
	 int inputprog=0;
	printf("LoopDub started...\n");

	/* Load configuration file */
	LoadConfiguration();

	if (m_Midi.Initialize())
		 printf("MIDI initialized.\n");
	else
		 printf("Couldn't initialize MIDI.\n");

	if (!gui.Initialize("LoopDub", false, WIDTH, HEIGHT, SDL_INIT_TIMER))
	{
		printf("Error initializing GUI.\n");
		return 1;
	}

	/* Change to loop folder */
	if (m_strChangeToFolder && chdir(m_strChangeToFolder))
		 printf("Warning: Couldn't change to %s\n", m_strChangeToFolder);

    /* Open output file */
    if (m_strRecordPath) {
        m_fRecordFile = fopen(m_strRecordPath, "wb");
        if (m_fRecordFile)
            printf("Writing to %s\n", m_strRecordPath);
        else
            printf("Warning: Could not open %s for writing.\n",
                   m_strRecordPath);
    }

	/* Load program changer */
	m_ProgramChanger.LoadPrograms();

	Scrob *pMainScrob = gui.GetScrobList();

	/* Blank area */
    /* Note: Blank area is used to force blanking and re-draw of entire loop area when switching between contexts */
	m_pBlankArea = new Box(pMainScrob, Rect(0, LOOPTOP, WIDTH, HEIGHT-30), -1, 0);
	pMainScrob->AddChild(m_pBlankArea);

	/* Loop area */
	m_pLoopArea = new Scrob(pMainScrob, m_pBlankArea->GetRect());
	pMainScrob->AddChild(m_pLoopArea);

	/* LoopOb instances */
	for (i=0; i<N_LOOPS; i++)
	{
		 m_pLoopOb[i] = new LoopOb(m_pLoopArea,
								   Rect(5,
										(LOOPHEIGHT+5)*i,
										pMainScrob->GetRect().Width() - 5,
										LOOPHEIGHT+(LOOPHEIGHT+5)*i),
								   i);
		m_pLoopOb[i]->SetVolume(m_cfgDefaultVolume);
		m_pLoopArea->AddChild(m_pLoopOb[i]);
	}

	/* Program area */
	m_pProgramArea = new Scrob(pMainScrob, m_pBlankArea->GetRect());
	pMainScrob->AddChild(m_pProgramArea);
	m_pProgramArea->SetVisible(false);
	m_pProgramArea->AddChild(new Label(m_pProgramArea, Rect(10, 10, 70, 30), "Programs", 3, -1));
	m_pProgramArea->AddChild(new Label(m_pProgramArea, Rect(11, 10, 71, 30), "Programs", 3, -1));

	/* List all programs */
	char str[256];
	int x=10, y=20, w, mx=0;
	for (i=0; i<m_ProgramChanger.NumPrograms(); i++)
	{
		 sprintf(str, "%d. %s", i+1, m_ProgramChanger.GetProgramName(i));
		 w = dt.GetTextWidth(str, strlen(str));
		 if (w > mx) mx = w;
		 y += dt.GetFontHeight()+2;
		 m_pProgramArea->AddChild(new Label(m_pProgramArea,
											Rect(x, y, x+w, y+dt.GetFontHeight()),
											str, 3, -1));
		 if (y > (m_pProgramArea->GetRect().Height()-20))
		 {
			  y = 20;
			  x += mx+20;
		 }
	}

	/* Controls along the top */
	Button *pBeats = new Button(pMainScrob, Rect(5, 5, 55, 20), "Beats", 0, 2, CMD_BEATS);
	pMainScrob->AddChild(pBeats);

	m_pAutoCueButton = new Button(pMainScrob, Rect(5, 22, 55, 37), "AutoCue", 0, 2, -1, 0, true);
	pMainScrob->AddChild(m_pAutoCueButton);

    m_pButtonModeLabel = new Label(pMainScrob, Rect(68, 22, 128, 37), "<Cue>", 3, 0);
    pMainScrob->AddChild(m_pButtonModeLabel);

	Label *pTempo = new Label(pMainScrob, Rect(60, 5, 120, 20), NULL, 3, 0);
	pMainScrob->AddChild(pTempo);

	m_pVUMeter = new VUMeter(pMainScrob, Rect(130, 5, 230, 20));
	pMainScrob->AddChild(m_pVUMeter);

	m_pVolumeSlider = new Slider(pMainScrob, Rect(130,27,230,40), false);
	pMainScrob->AddChild(m_pVolumeSlider);
	m_pVolumeSlider->SetValue(m_pVolumeSlider->GetValueMax() / 2);

	pMainScrob->AddChild(
		 new Image(pMainScrob, Rect(780-logo_width, 10, 780, 10+logo_height),
				   logo_width, logo_height, logo_data)
		 );

	/* Controls along the bottom */
	Box *pLoopDirBox = new Box(pMainScrob,
							   Rect(WIDTH-500,HEIGHT-145,WIDTH,HEIGHT-30), 2, 0);
	pLoopDirBox->SetVisible(false);
	pMainScrob->AddChild(pLoopDirBox);

	FileBrowser *pLoopDirBrowser = new FileBrowser;
	pLoopDirBrowser->SetExtension(".wav");
	pLoopDirBrowser->SetBase("/");
	if (!pLoopDirBrowser->Create(pLoopDirBox,
								 Rect(1,1,498,98),
								 ".", 0/*FILECLICK*/, 0/*DIRCLICK*/, false))
	{
		 printf("Error creating loopdir browser.\n");
		 return false;
	}
	pLoopDirBox->AddChild(pLoopDirBrowser);

	pLoopDirBox->AddChild(
		 new Button(pLoopDirBox, Rect(498-75,113-15,498,113), "Select",
					0, 2, CMD_LOOPDIRSEL));

	Button *pLoopDirBtn = new Button(pMainScrob, Rect(WIDTH-90, HEIGHT-25,
													  WIDTH-10, HEIGHT-10),
									 "Loop Folder", 0, 2, CMD_LOOPDIR, NULL, true);
	pMainScrob->AddChild(pLoopDirBtn);

/* TODO: change tempo?
	m_pTempoSlider = new Slider(pMainScrob, Rect(5, 5, 205, 20), false);
	pMainScrob->AddChild(m_pTempoSlider);
	m_pTempoSlider->SetValue((m_nTempo-MIN_TEMPO)*m_pTempoSlider->GetValueMax()/(MAX_TEMPO-MIN_TEMPO));

	Label *pLabel = new Label(pMainScrob, Rect(210, 5, 250, 20), "Tempo", 3, 0);
	pMainScrob->AddChild(pLabel);
	pLabel->SetInteger(100);
*/

	/* For initial update */
	SDL_Event evt;
	evt.type = SDL_USEREVENT;
	SDL_PushEvent(&evt);


	/* Midi controls */
	if (m_Midi.IsInitialized())
	{
		 pMainScrob->AddChild(new Label(pMainScrob, Rect(240, 5, 290, 20), "Midi:", 3, 0));

		 m_pMidiLearning = new Button(pMainScrob, Rect(290, 5, 340, 20), "Learn", 0, 2, CMD_LEARN, 0, true);
		 pMainScrob->AddChild(m_pMidiLearning);

		 m_pMidiClock = new Button(pMainScrob, Rect(290, 25, 340, 40), "Clock", 0, 2, -1, 0, true);
		 pMainScrob->AddChild(m_pMidiClock);

		 int n = m_Midi.GetMidiNum();
		 if (n>0)
		 for (int i=0; i<n; i++)
		 {
			  pMainScrob->AddChild(new Button(pMainScrob, Rect(350 + 50*i, 5, 400 + 50*i, 20),
											  m_Midi.GetMidiName(i),
											  (m_Midi.GetMidiType(i) == MidiInput) ? 0 : 7,
											  2, CMD_SELECT, (void*)i, true));
		 }
		 else
			  printf("No MIDI ports found.\n");
	}

	/* Set MIDI button mode */
	if (m_Midi.IsInitialized()) {
		m_Midi.SetButtonMode(m_cfgDefaultButton);
	}

	/* Initialize player */

	if (!m_Player.Initialize(FillBuffers, this, m_cfgInternalSampleRate, m_cfgHardwareSampleRate))
	{
		printf("Couldn't initialize audio player.\n");
		return 1;
	}

	m_Player.Play();

	SDL_TimerID timerID=0;
	SDL_Event *pEvent;
	bool bQuit=false;

    // Initial update
	gui.ProcessEvent();

	while (!bQuit)
	{
		gui.WaitEvent();

		LOCKMUTEX(mutex);

		/* Hit return to start or stop playback (disabled for now) */
		if ((pEvent=gui.GetEvent())->type == SDL_KEYDOWN
			&& pEvent->key.keysym.sym == SDLK_RETURN
			&& pEvent->key.keysym.mod == KMOD_NONE)
		{
			if (m_Player.IsPlaying())
				m_Player.Stop();
			else
				m_Player.Play();
		}

		/* In key mode, hit spacebar to retrig this sample */
		else if ((pEvent=gui.GetEvent())->type == SDL_KEYDOWN
			&& pEvent->key.keysym.sym == SDLK_SPACE
			&& pEvent->key.keysym.mod == KMOD_NONE)
		{
			 // find unused key
			 for (i=0; i<MAX_KEYS && m_Keys[i].on; i++);
			 if (i>=MAX_KEYS) i=0;
			 m_Keys[i].position = 0;
			 m_Keys[i].velocity = 1024;
			 m_Keys[i].note = 48; // default key C4
			 m_Keys[i].on = true;
		}

		/* Press S to synchronize the MIDI output. */
		else if ((pEvent=gui.GetEvent())->type == SDL_KEYDOWN
				&& pEvent->key.keysym.sym == 's') {
			SyncMidi();
		}

		/* Press P to show program listing */
		else if ((pEvent=gui.GetEvent())->type == SDL_KEYDOWN
				 && pEvent->key.keysym.sym == 'p') {
			 m_pLoopArea->SetVisible(!m_pLoopArea->IsVisible());
			 m_pProgramArea->SetVisible(!m_pProgramArea->IsVisible());
			 m_pBlankArea->SetDirty();
		}


		/* Type two digits to load a specified program */
		else if ((pEvent=gui.GetEvent())->type == SDL_KEYDOWN
			&& pEvent->key.keysym.sym >= '0'
			&& pEvent->key.keysym.sym <= '9'
			&& pEvent->key.keysym.mod == KMOD_NONE)
		{
			 int ch = pEvent->key.keysym.sym - '0';

			 if (inputchars==0) {
			   inputprog = ch;
			   inputchars ++;
			 }
			 else if (inputchars==1) {
			   inputprog = inputprog*10 + ch;
			   printf("Program entered: %d\n", inputprog);
			   m_ProgramChanger.ProgramChange(inputprog, m_pLoopOb);
			   inputchars = 0;
			 }

             // NOTE: previously, number keys were used to select a loop
             //m_pLoopOb[ch]->SetSelected(!m_pLoopOb[ch]->IsSelected());
		}
		else
			bQuit = !gui.ProcessEvent();

		if (bQuit) printf( "Quitting..\n");

		if (!bQuit)
		{
			 int cmd;
			 long value;
			 if (gui.GetCommand(&cmd, (void**)&value))
			 {
				  if (cmd==CMD_LEARN)
				  {
					   if (m_pMidiLearning)
							m_Midi.SetLearningMode(m_pMidiLearning->IsPressed());
				  }
				  else if (cmd==CMD_SELECT)
				  {
					   printf("Select: %d\n", value);
					   m_Midi.SelectDevice(value);
				  }
				  else if (cmd==CMD_BEATS)
				  {
					   m_nBeats = (m_nBeats*2)%128;
					   if (m_nBeats<=0) m_nBeats=1;
					   char str[10];
					   sprintf(str, "%d beat%s", m_nBeats, (m_nBeats>1)?"s":"");
					   pBeats->SetText(str);
					   if (m_pLoopOb[0]->GetSample()) {
							int length = m_pLoopOb[0]->GetSample()->m_nSamples;
							int bpm = Player::m_nSampleRate*m_nBeats*60/length;
							//	(samples/sec) / (samples/beats) = (samples/sec) * (beats/sample) = (beats/sec) * (sec/min) = (beats/min)
							sprintf(str, "%d bpm", bpm);
							pTempo->SetText(str);
					   }
				  }
				  else if (cmd>=CMD_FILECLICK && cmd<(CMD_FILECLICK+N_LOOPS))
				  {
					   int s = cmd-CMD_FILECLICK;

					   m_nLoadingSampleFor = s;
					   strcpy(m_strLoadingSample, (char*)value);

					   HTHREAD thread;
					   CREATETHREAD(thread, loadSampleThread, &app);
				  }
				  else if (cmd==CMD_DIRCLICK)
				  {
					   // Set all filebrowsers to same folder
					   for (int ch=0; ch<N_LOOPS; ch++)
							m_pLoopOb[ch]->m_pFileBrowser->SetDirectory((char*)value);
				  }
				  else if (cmd>=CMD_CLOSE && cmd<(CMD_CLOSE+N_LOOPS))
				  {
					   int s = cmd-CMD_CLOSE;
					   Sample *pOldSample = m_pLoopOb[s]->SetSample(NULL);
					   if (pOldSample)
							delete pOldSample;
				  }
				  else if (cmd==CMD_NORMALIZE) {
					   int ch = value;
					   if (m_pLoopOb[ch]->GetSample()) {
							m_pLoopOb[ch]->GetSample()->Normalize();
							m_pLoopOb[ch]->SetSample(m_pLoopOb[ch]->GetSample());
							m_pLoopOb[ch]->SetDirty();
					   }
				  }
				  else if (cmd==CMD_KEYS) {
					   int ch = value;
					   if (m_nKeysChannel > -1)
					   {
							m_pLoopOb[m_nKeysChannel]->LoseKeys();
							int i;
							for (i=0; i<MAX_KEYS; i++)
								m_Keys[i].on = false;
					   }

					   if (m_pLoopOb[ch]->HasKeys() && !m_pLoopOb[ch]->GetSample())
							m_pLoopOb[ch]->LoseKeys();

					   if (m_pLoopOb[ch]->HasKeys())
							m_nKeysChannel = ch;
					   else
							m_nKeysChannel = -1;
				  }
				  else if (cmd==CMD_SPLIT) {
						int ch = value;
						m_pLoopOb[ch]->Split();
				  }
				  else if (cmd==CMD_HOLD) {
						int ch = value;
						m_pLoopOb[ch]->SetHolding(m_pLoopOb[ch]->m_pHoldButton->IsPressed());
				  }
				  else if (cmd==CMD_SWITCH) {
						int ch = value;
						m_pLoopOb[ch]->SetSwitching(m_pLoopOb[ch]->m_pSwitchButton->IsPressed());
				  }
				  else if (cmd==CMD_PROGRAMCHANGE) {
					   m_ProgramChanger.ProgramChange((int)value, m_pLoopOb);
				  }
				  else if (cmd==CMD_LOOPDIR) {
					   if (pLoopDirBox->IsVisible()) {
							m_pLoopArea->SetDirty();
							m_pProgramArea->SetDirty();
							m_pBlankArea->SetDirty();
					   }
					   pLoopDirBrowser->SetVisible(!pLoopDirBox->IsVisible());
					   pLoopDirBox->SetVisible(!pLoopDirBox->IsVisible());
				  }
				  else if (cmd==CMD_LOOPDIRSEL) {
					   if (pLoopDirBox->IsVisible()) {
							m_pLoopArea->SetDirty();
							m_pProgramArea->SetDirty();
							m_pBlankArea->SetDirty();
					   }
					   pLoopDirBox->SetVisible(!pLoopDirBox->IsVisible());
					   pLoopDirBtn->SetPressed(false);

					   /* Change base folder for all browsers */
					   for (i=0; i<N_LOOPS; i++) {
							m_pLoopOb[i]->m_pFileBrowser->SetBase(
								 pLoopDirBrowser->GetDirectory());
					   }

					   /* Change to loop folder */
					   if (chdir(pLoopDirBrowser->GetDirectory()))
							printf("Warning: Couldn't change to %s\n", m_strChangeToFolder);

					   /* Load program changer */
					   m_ProgramChanger.LoadPrograms();
				  }
			 }

			 app.updated = true;

			 UNLOCKMUTEX(mutex);
			 m_Midi.CheckMsg();
			 m_Midi.UpdateClockTicks();

			 // Check if its time to switch to background sample.
			 for (i=0; i<N_LOOPS; i++)
				  m_pLoopOb[i]->CheckBackgroundSample();
		}
	}

	app.updated = true;
	UNLOCKMUTEX(mutex);
	m_Player.Stop();

	#ifdef TIMING
	printf("timer[0].avgtime = %f\n", timer[0].avgtime);
	#endif

	return 0;
}

int main(int argc, char* argv[])
{
	 if ((argc > 1) && argv[1]) {
		  if (strcmp(argv[1], "-v")==0) {
			   printf("LoopDub " APP_VERSION "\n");
			   exit(0);
		  }
		  else if (strcmp(argv[1], "--help")==0) {
			   printf("Usage: loopdub <folder> [outputfile]\n");
			   exit(0);
		  }
		  app.m_strChangeToFolder = argv[1];
	 }
     if ((argc > 2) && argv[2]) {
         app.m_strRecordPath = argv[2];
     }

	return app.Run();
}
