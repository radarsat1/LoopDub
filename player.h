#ifndef PLAYER_H_
#define PLAYER_H_

#ifdef WIN32
#define __WINDOWS_DS__
#endif
#include <RtAudio.h>
#include <samplerate.h>

#define SAMPLE_RATE      44100
#define BUFFER_SAMPLES   800	           // in samples
#define BYTES_PER_SAMPLE (sizeof(short)*2) // stereo

class Player
{
public:
	Player();
	~Player();

	bool Initialize(void (FillBuffers)(void*, int), void* param, int hw_samplerate=44100);

	float* StereoBuffer() { return m_pStereoBuffer; }
	int BufferSizeBytes() { return m_nBufferLengthBytes; }
	int BufferSizeSamples() { return m_nBufferLengthBytes / BYTES_PER_SAMPLE; }
	bool ReadyForData();
	void Mix(float *outputBuffer, unsigned long framesPerBuffer, int outTimeSample=0);
	void Play();
	void Stop();
	int GetPlayPositionSamples();
	bool IsPlaying() { return m_bPlaying; }

protected:
	RtAudio *m_pRtAudio;

	int m_nBufferLengthBytes;
	float *m_pStereoBuffer;
	bool m_bPlaying;
	int m_nSide;	// side of buffer to write next (1 or 2)

    int m_nHwSampleRate;
    SRC_STATE* m_pSRC;
    SRC_DATA m_SRC_data;

	void (*m_pFillBuffers)(void* param, int outTimeSample);
	void* m_Param;
};


#endif
