#ifndef _LD_MIDI_H_
#define _LD_MIDI_H_

#include "libdeps/rtmidi-1.0.6/RtMidi.h"

// The PCR-30 has 8 sets of controls
// Other controllers may have more or fewer,
// but let's think about that some other time.
#define CONTROLS 8

enum MidiType
{
	MidiInput,
	MidiOutput,
	MidiUnknown
};

// There are 2 control types:
// A slider which gives the volume level
// and an "effect" which modifies the currently
// select effect parameter.
enum CtrlType
{
	 CT_LEVEL,
	 CT_EFFECT1,
	 CT_EFFECT2,
	 CT_EFFECT3,
	 CT_EFFECT4,
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
	BT_CUE=0,
	BT_SPLIT=1,
	BT_HOLD=2,
	BT_KEYS=3,
	BT_NORM=4,
	BT_SWITCH=5,
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
	 const char *GetMidiName(int n);

	 // Return type of MIDI device
	 MidiType GetMidiType(int n);

	 // Select a MIDI device
	 void SelectDevice(int n);

     // Poll for a MIDI message from the hardware
     bool PollMidi(int *code, int *val, int *status, int *channel);

	 // Mode
	 void SetLearningMode(bool bLearnMode);

	 // Send a midi clock tick (24 per beat)
	 // in ms milliseconds from now.
	 void SendClockTick(long ms, bool startnow);

	 void UpdateClockTicks();

	 void LoadConfiguration();

 protected:
	 // MIDI control codes, one for each control type
	 char m_ctrlcode[CONTROLS][N_CT];

	 // True if we are in learning mode, otherwise
	 // we are in play mode.
	 bool m_bLearning;
	 int m_nLearnCh;
	 int m_nLearnType;
	 int m_nLastCode;
	 bool m_bMidiCodesHaveChanged;
	 bool m_bMidiClockActive;
	 bool m_bMidiClockWaiting;

	 // Select which button is affected by MIDI buttons
	 int m_nButtonMode;

	 // Midi streams
	 RtMidiIn  *m_pMidiIn;
	 RtMidiOut *m_pMidiOut;

	 bool m_bInitialized;
};

#endif // _LD_MIDI_H_
