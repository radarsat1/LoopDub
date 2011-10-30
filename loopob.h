// LoopOb.h: interface for the LoopOb class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _LOOPOB_H_
#define _LOOPOB_H_

#include "gui/scrobui.h"
//using namespace ScrobUI;
#include "sample.h"
#include "filter.h"

//////////////////////////////////////////////////////
// WaveOb -> display the wave on the screen
//           contains a pointer to a sample.

class WaveOb : public Scrob
{
public:
	WaveOb();
	WaveOb(Scrob* pParent, const Rect& r, Sample *pSample=NULL);
	virtual ~WaveOb();

	void SetSample(Sample *pSample);
	void SetColor(int color) { m_nColor = color; }

	virtual void OnMouseDown(Point mouse);
	virtual void OnMouseUp(Point mouse);
	virtual void OnMouseMove(Point mouse);

public:
	virtual void Draw();

	int m_nParts;
	int m_nLoopStart;
	int m_nLoopEnd;

protected:
	Sample *m_pSample;
	int m_nColor;
	bool m_bGotMouse;
	Point m_ptMouse;
};

////////////////////////////////////////////////////////
// IndicatorOb -> indicate current play position

class IndicatorOb : public Scrob
{
public:
	IndicatorOb() : Scrob() {}
	IndicatorOb(Scrob* pParent, const Rect& r);
	virtual ~IndicatorOb() {}

public:
	virtual void Draw();
	virtual bool Create(Scrob *pParent, const Rect& r);

	bool m_bDrawn;

protected:
};

////////////////////////////////////////////////////////
// VisibleLabel -> extention of Label which draws
//                 outlines in black around the text

class VisibleLabel : public Label
{
  public:
	VisibleLabel() : Label() {}
	VisibleLabel(Scrob *pParent, const Rect& r, char *strText, int color, int bkcolor=0)
		: Label(pParent, r, strText, color, bkcolor) {}
	virtual ~VisibleLabel() {}

  public:
	virtual void Draw();
};

////////////////////////////////////////////////////////
// LoopOb -> a loop controller widget, contains a
//           sample and a WaveOb for displaying it.

class LoopOb : public Scrob
{
public:
	LoopOb();
	LoopOb(Scrob* pParent, const Rect& r, int number, Sample *pSample=NULL);
	virtual ~LoopOb();

	Sample* SetSample(Sample *pSample);
	Sample* GetSample() { return m_pSample; }
	short GetSampleValue(int pos);
	short GetNextSample();

	bool IsMuted() { return m_bMuted; }
	void SetMute(bool bMute) { m_bMuted = bMute; m_pWaveOb->SetColor(bMute ? 1 : 2); }

	void SetVolume(int vol_percent);
	int GetVolume();

	// For midi control
	Slider* GetEffectSlider(int effect);
	Slider* GetVolumeSlider() { return m_pVolumeSlider; }

	bool IsHolding() { return m_bHolding; }
	void SetHolding(bool holding);

	bool IsSwitching() { return m_bSwitching; }
	void SetSwitching(bool switching);

	bool HasKeys();  // report status of key button
	void LoseKeys(); // keys are given to another channel

	void Split();	 // split parts to next power of two

	bool IsCue() { return m_pCueButton->IsPressed(); }

	bool IsSelected() { return m_bSelected; }
	void SetSelected(bool select) { if (m_bSelected!=select){m_bSelected=select; SetDirty();} }

	void ResetFxParams();

	// For program change
	// Sample will be switched to "background sample" when volume hits zero.
	// This way the user can cut the volume to trigger the switch.
	Sample* SetBackgroundSample(Sample *pSample);
	Sample* GetBackgroundSample() { return m_pBackgroundSample; }

	// This must be called on each cycle so that background sample can be switched in when
	// the time is right.
	void CheckBackgroundSample();

public:
	virtual void Draw();
	virtual bool Create(Scrob *pParent, const Rect& r, int number, Sample *pSample=NULL);

	// Hold volume (mute for now) until the next split, or beginning of loop.
	bool m_bHolding;

	// Switch to background sample at the next split, or beginning of loop.
	bool m_bSwitching;

	// exposed so dir can be set from other browsers
	FileBrowser *m_pFileBrowser;

protected:
	// objects
	Sample *m_pSample;
  public:
	Sample *m_pBackgroundSample;
	WaveOb *m_pWaveOb;
	Slider *m_pVolumeSlider;
	Slider *m_pCutoffSlider;
	Slider *m_pResonanceSlider;
	Slider *m_pDelayLengthSlider;
	Slider *m_pFeedbackSlider;
	Button *m_pCueButton;
	Button *m_pCloseButton;
	Button *m_pNormalizeButton;
	Button *m_pKeysButton;
	Button *m_pSplitButton;
	Button *m_pHoldButton;
	Button *m_pSwitchButton;
	VisibleLabel *m_pFilenameLabel;
	VisibleLabel *m_pBgFilenameLabel;
	IndicatorOb *m_pIndicator[2];

	// bank of filters
	enum { N_FILTERS=2 };
	Filter *m_filterbank[N_FILTERS];
	Lowpass lowpass;
	Delay delay;

	// attributes
	int m_nPos;               // current play position
	int m_nPosOffset;		  // offset from requested position
	bool m_bMuted;            // muted or not
	int m_nLastPos;           // position at which indicator was last drawn

	int m_nLoopStart;         // loop subpart of loop
	int m_nLoopEnd;

	int m_nParts;

	bool m_bSelected;
};

#endif // _LOOPOB_H_

