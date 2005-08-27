#ifndef PLAYER_H_
#define PLAYER_H_

#include "portaudio.h"

#define SAMPLE_RATE 44100
#define BUFFER_SAMPLES 2048	// in samples
#define BYTES_PER_SAMPLE (sizeof(short)*2)

class Player
{
public:
	Player();
	~Player();

	bool Initialize(void (FillBuffers)(void*, int), void* param);

	short* LeftBuffer() { return m_pLeftBuffer; }
	short* RightBuffer() { return m_pRightBuffer; }
	int BufferSizeBytes() { return m_nBufferLengthBytes>>1; }
	int BufferSizeSamples() { return m_nBufferLengthBytes>>2; }
	bool ReadyForData();
	void Mix(short *outputBuffer, unsigned long framesPerBuffer, int outTimeSample=0);
	void Play();
	void Stop();
	int GetPlayPositionSamples();
	bool IsPlaying() { return m_bPlaying; }

protected:
	PortAudioStream *paStream;

	int m_nBufferLengthBytes;
	short *m_pLeftBuffer;
	short *m_pRightBuffer;
	bool m_bPlaying;
	int m_nSide;	// side of buffer to write next (1 or 2)

	void (*m_pFillBuffers)(void* param, int outTimeSample);
	void* m_Param;
};


#endif
