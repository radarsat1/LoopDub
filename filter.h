
#ifndef _FILTER_H_
#define _FILTER_H_

class Filter
{
  public:
	virtual void Initialize() {}
	virtual short Work(short sample) {}
};

class Lowpass : public Filter
{
	float history[4];
	float cutoff;
	float resonance;
	float targetCutoff;
	float targetResonance;
	float minCutoff, maxCutoff;
	float minResonance, maxResonance;
	float inertia; // ratio of approach to target

  public:
	void Initialize();
	short Work(short sample);

    // parameters are passed as percentage of range
	void SetParams(float targetC100, float targetR100);
};

#define MAX_DELAY 22050
class Delay : public Filter
{
	short buffer[MAX_DELAY];
	int m_nPos;

	int m_nFeedback;
	int m_nDelayLength;

public:
	virtual void Initialize();
	virtual short Work(short sample);

    // parameters are passed as percentage of range
	void SetParams(int delay100, int feedback100);
};

#endif // _FILTER_H_
