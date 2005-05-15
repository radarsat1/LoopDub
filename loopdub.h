#ifndef _LOOPDUB_H_
#define _LOOPDUB_H_

#include "platform.h"
#include <SDL.h>
#include "gui/scrobui.h"
#include "portmidi/pm_common/portmidi.h"
//using namespace ScrobUI;


// Global definitions
#define N_LOOPS   8
#define MAX_TEMPO 260
#define MIN_TEMPO 20
#define MAX_KEYS  10
#define MAX_PARTS 16

// Commands
enum
{
	 CMD_LEARN=1,
	 CMD_SELECT,
	 CMD_BEATS,
	 CMD_NORMALIZE,
	 CMD_KEYS,
	 CMD_SPLIT,
	 CMD_WAIT,
	 CMD_BREAK,
	 CMD_FILECLICK,
	 CMD_CLOSE=CMD_FILECLICK+N_LOOPS,
	 N_CMD=CMD_CLOSE+N_LOOPS
};

#include "loopob.h"
#include "sample.h"
#include "player.h"
#include "ld_midi.h"

class VUMeter;

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
	static void FillBuffers(void *param);

	// variables
	int m_nBeats;
	int m_nPos;
	int m_nLength;

    // one channel can have the keyboard at a time
	int m_nKeysChannel;

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
	VUMeter *m_pVUMeter;
	Slider *m_pVolumeSlider;
	Button *m_pAutoCueButton;

	HMUTEX mutex;

	// loading
	int m_nLoadingSampleFor;
	char m_strLoadingSample[1024];

	bool updated;

	char* m_strChangeToFolder;
};

/* One global LoopDub object */
extern LoopDub app;


////////////////////////////////////////////////////////
// VUMeter -> display level of sound

class VUMeter : public Scrob
{
public:
	VUMeter() : Scrob() {}
	VUMeter(Scrob* pParent, const Rect& r);
	virtual ~VUMeter() {}

public:
	virtual void Draw();
	virtual bool Create(Scrob *pParent, const Rect& r);

	void SetPercentage(int percent);

protected:
	char m_nPercentage;
	char m_History[30];
	int m_nHistoryPos;
};



#endif // _LOOPDUB_H_
