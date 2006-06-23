
// LoopOb.cpp: implementation of the LoopOb class.
//
//////////////////////////////////////////////////////////////////////

#include "loopdub.h"
#include "loopob.h"
#include <string.h>

#define LOOP_X 115
#define LOOP_W 260
#define FX_W   80

/////////////////////////////////
// WaveOb

WaveOb::WaveOb() : Scrob()
{
	m_pSample = 0;
	m_bGotMouse = false;
}

WaveOb::WaveOb(Scrob* pParent, const Rect& r, Sample *pSample) : Scrob(pParent, r)
{
	m_pSample = 0;
	SetSample(pSample);
	m_bGotMouse = false;
}


WaveOb::~WaveOb()
{
}

void WaveOb::SetSample(Sample *pSample)
{
	m_pSample = pSample;
	if (m_pSample) {
		m_pSample->MakeMinMax(m_Rect.Width());
		m_nParts = 1;
		m_nLoopStart = 0;
		m_nLoopEnd = m_pSample->m_nSamples;
	}
	SetDirty();
}

void WaveOb::Draw()
{
	if (!m_pSample)
		return;

	dt.SetCurrentObject(this);

	char *pMin, *pMax;
	int nMinMaxSize;
	m_pSample->GetMinMax(&pMin, &pMax, &nMinMaxSize);

	if (!pMin || !pMax)
	{
		m_pSample->MakeMinMax(m_Rect.Width());
		m_pSample->GetMinMax(&pMin, &pMax, &nMinMaxSize);
	}

	// draw wave
	int i, center = m_Rect.Height()/2;
	Rect r;
	for (i=0; i<nMinMaxSize; i++)
	{
		r.x1 = i;
		r.x2 = r.x1+1;
		r.y1 = center - pMax[i]*(m_Rect.Height())/127;
		r.y2 = center - pMin[i]*(m_Rect.Height())/127 + 1;
		int sample_pos = i * m_pSample->m_nSamples / nMinMaxSize;
		if (sample_pos >= m_nLoopStart && sample_pos <= m_nLoopEnd)
			dt.FillRect(r, m_nColor);
		else
			dt.FillRect(r, m_nColor-1);
	}
	
	int partsize = (m_Rect.Width()<<8)/m_nParts;
	if (partsize > 0 && partsize < (m_Rect.Width()<<8))
		 for (int x=partsize; x<(m_Rect.Width()<<8); x+=partsize) {
			  r.x1 = x>>8;
			r.x2 = r.x1+1;
			r.y1 = 0;
			r.y2 = m_Rect.Height();
			dt.FillRect(r, m_nColor+1);
		}
}

void WaveOb::OnMouseDown(Point mouse)
{
	gui.CaptureMouse(this);
	m_bGotMouse = true;
	m_ptMouse = mouse;
	
	if (m_pSample) {
		int sample_x = mouse.x * m_pSample->m_nSamples / m_Rect.Width();
		int partsize = m_pSample->m_nSamples / m_nParts;
		m_nLoopStart = sample_x / partsize * partsize;
		m_nLoopEnd = m_nLoopStart + partsize;
		m_pParent->SetDirty();
	}
}

void WaveOb::OnMouseUp(Point mouse)
{
	gui.ReleaseMouse();
	m_bGotMouse = false;
}

void WaveOb::OnMouseMove(Point mouse)
{
	if (m_pSample && m_bGotMouse) {
		int sample_x = mouse.x * m_pSample->m_nSamples / m_Rect.Width();
		int point_x  = m_ptMouse.x * m_pSample->m_nSamples / m_Rect.Width();
		int partsize = m_pSample->m_nSamples / m_nParts;
		int start = sample_x / partsize * partsize;
		int point = point_x / partsize * partsize;

		m_nLoopStart = (start < point) ? start : point;
		m_nLoopEnd = ((start < point) ? point : start) + partsize;

		m_pParent->SetDirty();
	}
}


/////////////////////////////////
// IndicatorOb

IndicatorOb::IndicatorOb(Scrob* pParent, const Rect& r)
	 : Scrob(pParent, r)
{
	 Create(pParent, r);
}

bool IndicatorOb::Create(Scrob *pParent, const Rect& r)
{
	if (!Scrob::Create(pParent, r))
		return false;

	m_bDrawn = false;

	return true;
}

void IndicatorOb::Draw()
{
	 dt.SetCurrentObject(this);

	 int x = 0;
	 for (int y = 0; y < m_Rect.Height(); y++)
	 {
		  dt.PutPixel(Point(x, y),
					  5 - dt.GetPixel(Point(x, y)));
	 }

	 m_bDrawn = !m_bDrawn;
}

/////////////////////////////////
// VisibleLabel

void VisibleLabel::Draw()
{
	dt.SetCurrentObject(this);
	Point p(0, (m_Rect.Height()-dt.GetFontHeight())/2);
	
	if (m_nBkColor < 0)
		 m_nBkColor = 0;

	if (m_strText) {
		 dt.TextOut(Point(p.x-1, p.y-0), m_strText, m_nBkColor);
		 dt.TextOut(Point(p.x+1, p.y+0), m_strText, m_nBkColor);
		 dt.TextOut(Point(p.x-0, p.y-1), m_strText, m_nBkColor);
		 dt.TextOut(Point(p.x+0, p.y+1), m_strText, m_nBkColor);
		 dt.TextOut(p, m_strText, m_nColor);
	}
}

/////////////////////////////////
// LoopOb

LoopOb::LoopOb() : Scrob()
{
	m_pSample = 0;
	m_bMuted = false;
	m_pBackgroundSample = 0;
	m_bSwitching = false;
	m_bHolding = false;
}

LoopOb::LoopOb(Scrob* pParent, const Rect& r, int number, Sample* pSample) : Scrob(pParent, r)
{
	m_pSample = 0;
	m_bMuted = false;
	m_pBackgroundSample = 0;
	m_bSwitching = false;
	m_bHolding = false;
	Create(pParent, r, number, pSample);
}

LoopOb::~LoopOb()
{
	 if (m_pSample)
		  delete m_pSample;
	 if (m_pBackgroundSample)
		  delete m_pBackgroundSample;
}

bool LoopOb::Create(Scrob *pParent, const Rect& r, int number, Sample *pSample)
{
	if (!Scrob::Create(pParent, r))
		return false;

	m_pWaveOb = new WaveOb;
	if (!m_pWaveOb->Create(this, Rect(LOOP_X+2, 2, r.Width()-LOOP_W-2, r.Height()-2)))
		return false;
	AddChild(m_pWaveOb);

	m_pWaveOb->SetColor(2);

	int i;
	for (i=0; i<2; i++) {
		 m_pIndicator[i] = new IndicatorOb;
		 if (!m_pIndicator[i]->Create(this, Rect(LOOP_X, 2, 61, r.Height()-3)))
			  return false;
		 AddChild(m_pIndicator[i]);
	}

	m_pFilenameLabel = new VisibleLabel;
	if (!m_pFilenameLabel->Create(this, Rect(LOOP_X+5, 5, r.Width()-LOOP_W-4, 20), "", 7, 0))
		 return false;
	AddChild(m_pFilenameLabel);

	m_pBgFilenameLabel = new VisibleLabel;
	if (!m_pBgFilenameLabel->Create(this, Rect(LOOP_X+5, r.Height()-20, r.Width()-LOOP_W-4, r.Height()-2), "", 7, 0))
		return false;
	AddChild(m_pBgFilenameLabel);

	m_pFileBrowser = new FileBrowser;
	m_pFileBrowser->SetExtension(".wav");
	m_pFileBrowser->SetBase(".");
	if (!m_pFileBrowser->Create(this, m_pWaveOb->GetRect(), ".", CMD_FILECLICK+number, CMD_DIRCLICK, false))
		 return false;
	m_pFileBrowser->SetVisible(false);
	AddChild(m_pFileBrowser);

	int y = 0;
	int x = r.Width()-LOOP_W+5;
	AddChild(new Label(this, Rect(x, y, x+40, y+15), "Vol", 3));
	m_pVolumeSlider = new Slider;
	if (!m_pVolumeSlider->Create(this, Rect(x+40, y, x+40+FX_W, y+15), false))
		return false;
	AddChild(m_pVolumeSlider);

	y += 17;
	AddChild(new Label(this, Rect(x, y, x+40, y+15), "Cutoff", 3));
	m_pCutoffSlider = new Slider;
	if (!m_pCutoffSlider->Create(this, Rect(x+40, y, x+40+FX_W, y+15), false))
		return false;
	AddChild(m_pCutoffSlider);

	y += 17;
	AddChild(new Label(this, Rect(x, y, x+40, y+15), "Rez", 3));
	m_pResonanceSlider = new Slider;
	if (!m_pResonanceSlider->Create(this, Rect(x+40, y, x+40+FX_W, y+15), false))
		return false;
	AddChild(m_pResonanceSlider);

	x = x+45+FX_W;
	y = 0;

	AddChild(new Label(this, Rect(x, y, x+40, y+15), "Delay", 3));
	m_pDelayLengthSlider = new Slider;
	if (!m_pDelayLengthSlider->Create(this, Rect(x+40, y, x+40+FX_W, y+15), false))
		return false;
	AddChild(m_pDelayLengthSlider);

	y += 17;
	AddChild(new Label(this, Rect(x, y, x+40, y+15), "Fdbak", 3));
	m_pFeedbackSlider = new Slider;
	if (!m_pFeedbackSlider->Create(this, Rect(x+40, y, x+40+FX_W, y+15), false))
		return false;
	m_pFeedbackSlider->SetValue(0);
	AddChild(m_pFeedbackSlider);

	y = 2;
	m_pCueButton = new Button;
	if (!m_pCueButton->Create(this, Rect(3, y, 55, y+15), "Cue", 0, 2, 0, 0, true))
		return false;
	AddChild(m_pCueButton);

	y += 17;
	m_pKeysButton = new Button;
	if (!m_pKeysButton->Create(this, Rect(3, y, 55, y+15), "Keys", 0, 2, CMD_KEYS, (void*)number, true))
		return false;
	AddChild(m_pKeysButton);

	y += 17;
	m_pNormalizeButton = new Button;
	if (!m_pNormalizeButton->Create(this, Rect(3, y, 55, y+15), "Norm", 0, 2, CMD_NORMALIZE, (void*)number, false))
		return false;
	AddChild(m_pNormalizeButton);

	y = 2;
	m_pSplitButton = new Button;
	if (!m_pSplitButton->Create(this, Rect(57, y, 112, y+15), "Split", 0, 2, CMD_SPLIT, (void*)number, false))
		return false;
	AddChild(m_pSplitButton);

	y += 17;
	m_pHoldButton = new Button;
	if (!m_pHoldButton->Create(this, Rect(57, y, 112, y+15), "Hold", 0, 2, CMD_HOLD, (void*)number, true))
		return false;
	AddChild(m_pHoldButton);

	y += 17;
	m_pSwitchButton = new Button;
	if (!m_pSwitchButton->Create(this, Rect(57, y, 112, y+15), "Switch", 0, 2, CMD_SWITCH, (void*)number, true))
		return false;
	AddChild(m_pSwitchButton);

	m_pCloseButton = new Button;
	if (!m_pCloseButton->Create(this, Rect(r.Width()-LOOP_W-20, 5, r.Width()-LOOP_W-10, 16), " ", 0, 2, CMD_CLOSE+number, 0))
		 return false;
	AddChild(m_pCloseButton);

	SetSample(pSample);
	SetVolume(100);

	m_nPos = 0;
	m_nLastPos = 0;

	m_filterbank[0] = &lowpass;
	m_filterbank[1] = &delay;
	for (i=0; i<N_FILTERS; i++)
		 m_filterbank[i]->Initialize();

	return true;
}

Sample* LoopOb::SetSample(Sample *pSample)
{
	 // note: we must call m_pWaveOb::SetSample()
	 //       *before* setting m_pSample, otherwise
     //       the player thread may assume there is
	 //       a sample present and try to use the
	 //       m_pWaveOb::m_nLoopEnd value before
     //       it has been initialized properly.
	 Sample *pOldSample = m_pSample;
	 m_pWaveOb->SetSample(pSample);
	 m_pSample = pSample;
	 if (m_pSample)
	 {
		  int o = 0, l = strlen(m_pFileBrowser->GetBase());
		  if (strncmp(m_pSample->GetFilename(), m_pFileBrowser->GetBase(), l)==0)
			   o = l+1; // include trailing '/'
		  m_pFilenameLabel->SetText(m_pSample->GetFilename() + o);
		  m_pFileBrowser->SetVisible(false);
		  m_pCloseButton->SetVisible(true);
		  m_pFilenameLabel->SetVisible(true);
		  m_pIndicator[0]->SetVisible(true);
		  m_pIndicator[1]->SetVisible(true);
		  m_pIndicator[0]->m_bDrawn = false;
		  m_pIndicator[1]->m_bDrawn = false;
		  m_nPos = m_nLastPos = 0;
		  m_nParts = 1;
		  m_nLoopStart = 0;
		  m_nLoopEnd = m_pSample->m_nSamples;
		  m_nPosOffset = 0;
		  SetHolding(false);
		  SetSwitching(false);
		  if (app.m_pAutoCueButton->IsPressed())
			   m_pCueButton->SetPressed(true);
		  ResetFxParams();
	 }
	 else
	 {
		  m_pFileBrowser->SetVisible(true);
		  m_pCloseButton->SetVisible(false);
		  m_pFilenameLabel->SetVisible(false);
		  m_pIndicator[0]->SetVisible(false);
		  m_pIndicator[1]->SetVisible(false);
	 }
	 
	 SetDirty();
	 return pOldSample;
}

void LoopOb::Split()
{
	if (m_pSample) {
		m_nParts *= 2;
		if (m_nParts > MAX_PARTS)
			m_nParts = 1;

		m_pWaveOb->m_nParts = m_nParts;
		SetDirty();	
	}
}

void LoopOb::Draw()
{
	dt.SetCurrentObject(this);

	Rect r(LOOP_X,0,m_Rect.Width()-LOOP_W,m_Rect.Height());
	dt.DrawEdge(r,true,false);

	r.x1++; r.y1++; r.x2--; r.y2--;
	dt.DrawEdge(r,false,false);

	r.x1++; r.y1++; r.x2--; r.y2--;
	dt.FillRect(r, m_pSample ? (m_bSelected ? 5 : 4) : 0);
}

short LoopOb::GetSampleValue(int pos)
{
	 if (m_pSample)
	 {
		  // get subloop positions when current subloop finishes or it is already in new subloop
		  if (pos==0 || m_nPos==(m_nLoopEnd-1)
			  || (m_nPos >= m_pWaveOb->m_nLoopStart && m_nPos < m_pWaveOb->m_nLoopEnd)) {
			   m_nLoopStart = m_pWaveOb->m_nLoopStart;
			   m_nLoopEnd = m_pWaveOb->m_nLoopEnd;
		  }

		  // set the new position
		  m_nPos = (pos % (m_nLoopEnd-m_nLoopStart)) + m_nLoopStart;

		  // on split start or loop start
		  if (m_nPos % (m_pSample->m_nSamples/m_pWaveOb->m_nParts)==0) {

			   // unhold
			   SetHolding(false);
			   
			   if (m_bSwitching && m_pBackgroundSample) {
					// switch to background sample
					Sample *pOldSample = SetSample(m_pBackgroundSample);
					if (pOldSample)
						 delete pOldSample;
					m_pBackgroundSample = NULL;
			   }
		  }

		  // set position indicator
		  IndicatorOb *pIndicator = m_pIndicator[0];
		  if (pIndicator->m_bDrawn)
			   pIndicator = m_pIndicator[1];
		  
		  Rect r = pIndicator->GetRect();
		  r.x1 = m_nPos * (m_pWaveOb->GetRect().Width()) / m_pSample->m_nSamples + LOOP_X;
		  r.x2 = r.x1 + 1;
		  pIndicator->SetRect(r);
		  m_pIndicator[0]->SetDirty();
		  m_pIndicator[1]->SetDirty();

		  // get the sample
		  short sample = m_pSample->m_pData[m_nPos];
		  sample = sample * !m_bMuted * m_pVolumeSlider->GetValue() / m_pVolumeSlider->GetMaxValue();
		  if (m_bHolding)
			   sample = 0;

		  // set filter parameters according to sliders
		  lowpass.SetParams(m_pCutoffSlider->GetValue() * 100.0f / m_pCutoffSlider->GetMaxValue(),
							m_pResonanceSlider->GetValue() * 100.0f / m_pResonanceSlider->GetMaxValue());

		  delay.SetParams(m_pDelayLengthSlider->GetValue() * 100 / m_pDelayLengthSlider->GetMaxValue(),
							m_pFeedbackSlider->GetValue() * 100 / m_pFeedbackSlider->GetMaxValue());
		  
		  // pass it through the filter bank		
		  for (int i=0; i<N_FILTERS; i++)
			   sample = m_filterbank[i]->Work(sample);

		  return sample;
	 }
	 
	 return 0;
}

short LoopOb::GetNextSample()
{
	 if (m_pSample)
	 {
		  // set position indicator
		  IndicatorOb *pIndicator = m_pIndicator[0];
		  if (pIndicator->m_bDrawn)
			   pIndicator = m_pIndicator[1];
		  
		  Rect r = pIndicator->GetRect();
		  r.x1 = m_nPos * (m_pWaveOb->GetRect().Width()-6) / m_pSample->m_nSamples + LOOP_X;
		  r.x2 = r.x1 + 1;
		  pIndicator->SetRect(r);
		  m_pIndicator[0]->SetDirty();
		  m_pIndicator[1]->SetDirty();

		  short sample = m_pSample->m_pData[m_nPos];

		  sample = sample * !m_bMuted * m_pVolumeSlider->GetValue() / m_pVolumeSlider->GetMaxValue();

		  // set filter parameters according to sliders
		  lowpass.SetParams(m_pCutoffSlider->GetValue() * 100.0f / m_pCutoffSlider->GetMaxValue(),
							m_pResonanceSlider->GetValue() * 100.0f / m_pResonanceSlider->GetMaxValue());

		  // pass it through the filter bank
		  for (int i=0; i<N_FILTERS; i++)
			   sample = m_filterbank[i]->Work(sample);

		  // update position
		  m_nPos = (m_nPos+1) % m_pSample->m_nSamples;
		  if (m_nPos == m_pWaveOb->m_nLoopEnd)
		  	m_nPos = m_pWaveOb->m_nLoopStart;

		  return sample;
	 }
	 else
		  return 0;
}

void LoopOb::SetVolume(int vol_percent)
{
	 m_pVolumeSlider->SetValue(vol_percent*m_pVolumeSlider->GetMaxValue()/100);
}

int LoopOb::GetVolume()
{
	 return m_pVolumeSlider->GetValue() * 100 / m_pVolumeSlider->GetMaxValue();
}

Slider* LoopOb::GetEffectSlider(int effect)
{
	 switch (effect)
	 {
	 case 0:  return m_pCutoffSlider;
	 case 1:  return m_pResonanceSlider;
	 case 2:  return m_pDelayLengthSlider;
	 case 3:  return m_pFeedbackSlider;
	 default: return NULL;
	 }
}

void LoopOb::SetHolding(bool holding)
{
	 m_bHolding = holding;
	 m_pHoldButton->SetPressed(m_bHolding);
}

void LoopOb::SetSwitching(bool switching)
{
	 m_bSwitching = switching;
	 m_pSwitchButton->SetPressed(m_bSwitching);
}

bool LoopOb::HasKeys()
{
	 return m_pKeysButton->IsPressed();
}

void LoopOb::LoseKeys()
{
	 m_pKeysButton->SetPressed(false);
}

void LoopOb::ResetFxParams()
{
	m_pCutoffSlider->SetValue(m_pCutoffSlider->GetMaxValue());
	m_pResonanceSlider->SetValue(0);
	m_pDelayLengthSlider->SetValue(m_pDelayLengthSlider->GetMaxValue()/4);
	m_pFeedbackSlider->SetValue(0);
}

Sample* LoopOb::SetBackgroundSample(Sample *pSample)
{
	 Sample *pOldSample = m_pBackgroundSample;
	 m_pBackgroundSample = pSample;
	 if (!m_pSample) {
		  SetSample(m_pBackgroundSample);
		  m_pBackgroundSample = NULL;
	 }
	 else
	 {
		  int o = 0, l = strlen(m_pFileBrowser->GetBase());
		  if (strncmp(pSample->GetFilename(), m_pFileBrowser->GetBase(), l)==0)
			   o = l+1; // include trailing '/'

		  char *s = pSample->GetFilename() + o;
		  m_pBgFilenameLabel->SetText(s);
		  m_pBgFilenameLabel->SetVisible(true);

		  int w = dt.GetTextWidth(s, strlen(s));
		  Rect r = m_pBgFilenameLabel->GetRect();
		  r.x1 = r.x2 - w;
		  m_pBgFilenameLabel->SetRect(r);
	 }

	 return pOldSample;
}

/*!
 * Check if it's time to switch to background sample.
 * (ie., no foreground sample loaded, or volume is zero)
 */
void LoopOb::CheckBackgroundSample()
{
	 if (m_pBackgroundSample && (!m_pSample || GetVolume()==0)) {
		  Sample *pOldSample = SetSample(m_pBackgroundSample);
		  if (pOldSample)
			   delete pOldSample;
		  SetVolume(0);
		  m_pBackgroundSample = NULL;
		  m_pBgFilenameLabel->SetVisible(false);
		  m_pWaveOb->SetDirty();
	 }
}

