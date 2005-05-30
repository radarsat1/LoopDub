#ifndef _LD_MIDI_H_
#define _LD_MIDI_H_

#include "portmidi/pm_common/portmidi.h"

// The PCR-30 has 8 sets of controls
// Other controllers may have more or fewer,
// but let's think about that some other time.
#define CONTROLS 8

// There are 2 control types:
// A slider which gives the volume level
// and an "effect" which modifies the currently
// select effect parameter.
enum CtrlType
{
	 CT_LEVEL,
	 CT_EFFECT,
	 CT_BUTTON,
	 CT_SELECT,
	 N_CT
};

// MIDI CC messages can be sent by the controller
// in response to assignable button pushes.
// Since most MIDI controllers have fewer buttons
// than the LoopDub display, it is instead possible
// to assign certain CCs to Select which button is
// to be pushed, and other CCs to individual loops.
enum Buttons
{
	BT_CUE,
	BT_SPLIT,
	BT_HOLD,
	BT_KEYS,
	BT_NORM,
	BT_BREAK,
	N_BT
};

class MidiControl
{
 public:
	 MidiControl();
	 ~MidiControl();
	 
	 // Open MIDI ports, etc
	 bool Initialize();
	 bool IsInitialized() { return m_bInitialized; }

	 // Check for MIDI messages
	 // Return immediately if none
	 void CheckMsg();

	 // Return number of MIDI devices
	 int GetMidiNum();
	 
	 // Return name of MIDI device n
	 char *GetMidiName(int n);

	 // Select a MIDI device
	 void SelectDevice(int n);

	 // Mode
	 void SetLearningMode(bool bLearnMode);

 private:
	 // MIDI control codes, one for each control type
	 char m_ctrlcode[CONTROLS][N_CT];

	 // True if we are in learning mode, otherwise
	 // we are in play mode.
	 bool m_bLearning;
	 int m_nLearnCh;
	 int m_nLearnType;
	 int m_nLastCode;

	 // Select which button is affected by MIDI buttons
	 int m_nButtonMode;

	 // Midi input stream
	 PmStream *m_pmListen;

	 bool m_bInitialized;
};

#endif // _LD_MIDI_H_
