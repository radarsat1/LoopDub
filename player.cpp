
#include <stdio.h>
#include <string.h>
#include "player.h"

Player::Player()
{
	paStream = NULL;
	m_pLeftBuffer = NULL;
	m_pRightBuffer = NULL;
	
	m_pFillBuffers = NULL;
	m_Param = NULL;
}

Player::~Player()
{
	if (paStream)
	{
		Pa_CloseStream(paStream);
		paStream = NULL;
		
		Pa_Terminate();
	}
}

int callback(  void *inputBuffer, void *outputBuffer,
                   unsigned long framesPerBuffer,
                   double outTime, void *userData )
{
	((Player*)userData)->Mix((short*)outputBuffer, framesPerBuffer);
	return 0;
}

bool Player::Initialize(void (FillBuffers)(void*), void* param)
{
	/* Initialize sound player (PortAudio) & buffers */
	m_pLeftBuffer=NULL;
	m_pRightBuffer=NULL;
	
	m_pFillBuffers = FillBuffers;
	m_Param = param;
	if (!m_pFillBuffers)
	{
		printf("No FillBuffers callback specified.\n");
		return false;
	}

	// Initialize sound
	int rc;
	if ((rc=Pa_Initialize()) != paNoError)
	{
		 printf("Couldn't initialize PortAudio: %d\n", rc);
		return false;
	}

	// Open stream. 
	if (Pa_OpenDefaultStream( &paStream, 0, 2, paInt16, SAMPLE_RATE, BUFFER_SAMPLES,
							  0, callback, (void*)this) != paNoError)
	{
		 printf("Couldn't open PortAudioStream.\n");
		return false;
	}
	
	printf("PortAudioStream opened.\n");
	
	// buffer size
	m_nBufferLengthBytes = BUFFER_SAMPLES * BYTES_PER_SAMPLE;

	// left & right buffers
	m_pLeftBuffer = new short[BUFFER_SAMPLES];
	m_pRightBuffer = new short[BUFFER_SAMPLES];
	memset(m_pLeftBuffer, 0, sizeof(short)*BUFFER_SAMPLES);
	memset(m_pRightBuffer, 0, sizeof(short)*BUFFER_SAMPLES);

	m_nSide = 1;

	return true;
}

#include <math.h>
void Player::Mix(short *outputBuffer, unsigned long framesPerBuffer)
{
	// call mix callback
	m_pFillBuffers(m_Param);
	
	// fill soundbuffer
	static int c=0;
	for (int i=0; i<(framesPerBuffer); i++)
	{
		outputBuffer[(i<<1)+0] = m_pRightBuffer[i];
		outputBuffer[(i<<1)+1] = m_pLeftBuffer[i];
	}
}

void Player::Play()
{
	PaError err;
	if (paStream)
		err = Pa_StartStream(paStream);

	m_bPlaying=(err==paNoError);
}

void Player::Stop()
{
	if (paStream) m_bPlaying=!(Pa_StopStream(paStream)==paNoError);
}

int Player::GetPlayPositionSamples()
{
	/*
	DWORD dwPlayPos, dwWritePos;
	m_pBuffer->GetCurrentPosition(&dwPlayPos, &dwWritePos);
	return (int)dwPlayPos/4;
	*/
	
	return 0;
}
