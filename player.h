#ifndef PLAYER_H_
#define PLAYER_H_

#ifdef WIN32
#define __WINDOWS_DS__
#endif
#include <RtAudio.h>

#define SAMPLE_RATE 44100
#define BUFFER_SAMPLES 1024	               // in samples
#define BYTES_PER_SAMPLE (sizeof(short)*2) // stereo

class Player
{
public:
	Player();
	~Player();

	bool Initialize(void (FillBuffers)(void*, int), void* param);

	short* StereoBuffer() { return m_pStereoBuffer; }
	int BufferSizeBytes() { return m_nBufferLengthBytes; }
	int BufferSizeSamples() { return m_nBufferLengthBytes / BYTES_PER_SAMPLE; }
	bool ReadyForData();
	void Mix(short *outputBuffer, unsigned long framesPerBuffer, int outTimeSample=0);
	void Play();
	void Stop();
	int GetPlayPositionSamples();
	bool IsPlaying() { return m_bPlaying; }

protected:
	RtAudio *m_pRtAudio;

	int m_nBufferLengthBytes;
	short *m_pStereoBuffer;
	bool m_bPlaying;
	int m_nSide;	// side of buffer to write next (1 or 2)

	void (*m_pFillBuffers)(void* param, int outTimeSample);
	void* m_Param;
};


#endif
