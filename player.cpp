
#include <stdio.h>
#include <string.h>
#include "player.h"

Player::Player()
{
	m_pLeftBuffer = NULL;
	m_pRightBuffer = NULL;
	
	m_pFillBuffers = NULL;
	m_Param = NULL;
}

Player::~Player()
{
	 rtaudio.closeStream();
}

int callback( char* buffer, int bufferSize, void* userData)
{
	 ((Player*)userData)->Mix((short*)buffer, bufferSize, 0);

	 // true to stop the stream
	 return false;
}

/*
int callback(  void *inputBuffer, void *outputBuffer,
                   unsigned long framesPerBuffer,
                   double outTime, void *userData )
{
	((Player*)userData)->Mix((short*)outputBuffer, framesPerBuffer, (int)outTime);
	return 0;
}
*/

bool Player::Initialize(void (FillBuffers)(void*, int), void* param)
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
	// RtAudio was initialized in contructor

	// Open stream.
	try {
		int bufferSize = BUFFER_SAMPLES;
		rtaudio.openStream(0, 2, 0, 0,
						   RTAUDIO_SINT16, 48000,//SAMPLE_RATE,
			&bufferSize, 3);

		rtaudio.setStreamCallback(callback, this);
	}
	catch (RtError e)
	{
		printf("RtError in Player::Initialize(): %s\n", e.getMessage().c_str());
		return false;
	}
		
	printf("RtAudio stream opened.\n");
	
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

void Player::Mix(short *outputBuffer, unsigned long framesPerBuffer, int outTimeSample)
{
	// call mix callback
	m_pFillBuffers(m_Param, outTimeSample);
	
	// fill soundbuffer
	static int c=0, i=0;
	for (i=0; i<(framesPerBuffer); i++)
	{
		outputBuffer[(i<<1)+0] = m_pRightBuffer[i];
		outputBuffer[(i<<1)+1] = m_pLeftBuffer[i];
	}

	/*
	// call mix callback
	m_pFillBuffers(m_Param, outTimeSample);
	
	// fill soundbuffer
	for (i=0; i<(framesPerBuffer); i++)
	{
		outputBuffer[(i<<0)+framesPerBuffer] = m_pRightBuffer[i];
		//outputBuffer[(i<<1)+1] = m_pLeftBuffer[i];
	}
	*/
}

void Player::Play()
{
	try {
		rtaudio.startStream();
		m_bPlaying=true;
	}
	catch (RtError e)
	{
		printf("RtError in Player::Play(): %s\n", e.getMessage().c_str());
		m_bPlaying=false;
	}
}

void Player::Stop()
{
	try {
		rtaudio.stopStream();
		m_bPlaying=false;
	}
	catch (RtError e)
	{
		printf("RtError in Player::Stop(): %s\n", e.getMessage().c_str());
	}
}

int Player::GetPlayPositionSamples()
{
	// TODO
	return 0;
}
