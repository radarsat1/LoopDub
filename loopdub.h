
#ifndef _LOOPDUB_H_
#define _LOOPDUB_H_

#include "platform.h"
#include <SDL.h>
#include "gui/scrobui.h"
#include "portmidi/pm_common/portmidi.h"
//using namespace ScrobUI;

#include "def.h"
#include "loopob.h"
#include "sample.h"
#include "player.h"
#include "ld_midi.h"
#include "vumeter.h"
#include "program.h"

/*
** LoopDub class contains main program variables and functions
*/

class LoopDub
{
public:
	LoopDub();
	~LoopDub();

	// functions

	// main program loop //
	int Run();

	// callback for play buffer mixing routine
	static void FillBuffers(void *param, int outTimeSample=0);

	// variables
	int m_nBeats;
	int m_nPos;
	int m_nLength;

    // one channel can have the keyboard at a time
	int m_nKeysChannel;

	// program list
	ProgramChanger m_ProgramChanger;

	// keep position & velocity for each key pressed (circular buffer)
	struct {
		bool on;
		int position;
		int velocity;
		int note;
	} m_Keys[MAX_KEYS];
	
	
	// objects
	Player m_Player;
	MidiControl m_Midi;

	LoopOb *m_pLoopOb[N_LOOPS];
	Button *m_pMidiLearning;
	Button *m_pMidiClock;
	VUMeter *m_pVUMeter;
	Slider *m_pVolumeSlider;
	Button *m_pAutoCueButton;
	Scrob *m_pLoopArea;
	Scrob *m_pProgramArea;
	Box *m_pBlankArea;

	HMUTEX mutex;

	// loading
	int m_nLoadingSampleFor;
	char m_strLoadingSample[1024];

	bool updated;

	char* m_strChangeToFolder;
};

/* One global LoopDub object */
extern LoopDub app;



#endif // _LOOPDUB_H_
