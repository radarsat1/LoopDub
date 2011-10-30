
#include "filter.h"
#include "player.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define PI 3.141592654
#define FLTEQUAL(a, b)  fabs(a-b)<=0.01

void Lowpass::Initialize()
{
	 // note: why does max cutoff of 22050 fall to zero db?

	 cutoff = targetCutoff = 22000.0f;
	 resonance = targetResonance = 1.0f;
	 maxCutoff = 22000.0f;
	 maxResonance = 40.0f;//126.0f;
	 minCutoff = 550.0f;
	 minResonance = 0.1f;
	 inertia = 0.2f;
	 lastTargetC100 = 0;
	 lastTargetR100 = 0;

	 int i;
	 for (i=0; i<5; i++)
		  coef[i] = 0;

	 m_bParamsChanged = false;

	 cutoffPow = log10(maxCutoff-minCutoff);
	 resonancePow = log10(maxResonance-minResonance);
}

void Lowpass::SetParams(float targetC100, float targetR100)
{
	 if (!FLTEQUAL(lastTargetC100, targetC100) || !(FLTEQUAL(lastTargetR100, targetR100))) {
		  targetCutoff = pow(targetC100 / 10.0, cutoffPow) + minCutoff;
		  targetResonance = pow(targetR100 / 10.0, resonancePow) + minResonance;
		  m_bParamsChanged = true;

		  lastTargetC100 = targetC100;
		  lastTargetR100 = targetR100;
	 }
}

short Lowpass::Work(short sample)
{
	// go to target via inertia ratio
	float C = (targetCutoff*inertia + cutoff*(1.0f-inertia));
	float R = (targetResonance*inertia + resonance*(1.0f-inertia));

	// filter
	float temp_y;
	float alpha, omega, sn, cs;
	float a0, a1, a2, b0, b1, b2;

	 if (m_bParamsChanged)
	 {
	// Calculate filter coefficients (with interpolation)
	omega = 2.0f * (float)PI * C / Player::m_nSampleRate;
	sn = (float)sin (omega);
	cs = (float)cos (omega);
	alpha = sn / R;
	b0 = (1.0f - cs) / 2.0f;
	b1 = 1.0f - cs;
	b2 = (1.0f - cs) / 2.0f;
	a0 = 1.0f + alpha;
	a1 = -2.0f * cs;
	a2 = 1.0f - alpha;
	coef[0] = b0/a0;
	coef[1] = b1/a0;
	coef[2] = b2/a0;
	coef[3] = -a1/a0;
	coef[4] = -a2/a0;
	 }

	// perform filter function
	temp_y = coef[0] * sample +
		 coef[1] * history[0] + //lx1 +
		 coef[2] * history[1] + //lx2 +
		 coef[3] * history[2] + //ly1 +
		 coef[4] * history[3];  //ly2;

	// shift buffer
	history[3] = history[2];
	history[2] = temp_y;
	history[1] = history[0];
	history[0] = sample;

	cutoff = C;
	resonance = R;

	if (FLTEQUAL(cutoff,targetCutoff) && FLTEQUAL(resonance,targetResonance))
		 m_bParamsChanged = false;

	if (temp_y > 32767) sample = 32767;
	else if (temp_y < -32768) sample = -32768;
	else sample = (short)temp_y;
	return sample;
}

void Delay::Initialize()
{
	 m_nPos = 0;
	 memset(buffer, 0, MAX_DELAY*sizeof(short));
	 m_nDelayLength = MAX_DELAY;
	 m_nFeedback = 60;
}

short Delay::Work(short sample)
{
	 int output = sample + ((buffer[m_nPos] * m_nFeedback) / 100);
	 if (output > 32767) output = 32767;
	 if (output < -32767) output = -32767;

	 buffer[m_nPos] = output;
	 if (m_nDelayLength > 0)
		  m_nPos = (m_nPos+1) % m_nDelayLength;
	 return (short)output;
}

void Delay::SetParams(int delay100, int feedback100)
{
	 m_nDelayLength = delay100 * MAX_DELAY / 100;
	 m_nFeedback = feedback100;
}
