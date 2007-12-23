
#include <stdio.h>
#include <string.h>
#include "player.h"

Player::Player()
{
	m_pStereoBuffer = NULL;
	m_pFillBuffers = NULL;
	m_Param = NULL;

	try {
		 m_pRtAudio = new RtAudio();
	}
	catch (RtError &e) {
		 if (m_pRtAudio) delete m_pRtAudio;
		 m_pRtAudio = NULL;
		 printf("RtError in Player::Player(): %s\n", e.getMessage().c_str());
	}
}

Player::~Player()
{
	 if (m_pRtAudio) {
		  m_pRtAudio->closeStream();
		  delete m_pRtAudio;
		  m_pRtAudio = NULL;
	 }
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
	m_pStereoBuffer=NULL;
	
	m_pFillBuffers = FillBuffers;
	m_Param = param;
	if (!m_pFillBuffers)
	{
		printf("No FillBuffers callback specified.\n");
		return false;
	}

	// Initialize sound
	// RtAudio was initialized in contructor
	if (!m_pRtAudio) return false;

	// Open stream.
	try {
		int bufferSize = BUFFER_SAMPLES;
		m_pRtAudio->openStream(0, 2, 0, 0,
				       RTAUDIO_SINT16, 48000,//SAMPLE_RATE,
							   &bufferSize, 3);
		
		m_pRtAudio->setStreamCallback(callback, this);
	}
	catch (RtError& e)
	{
		printf("RtError in Player::Initialize(): %s\n", e.getMessage().c_str());
		return false;
	}
		
	printf("RtAudio stream opened.\n");
	
	// buffer size
	m_nBufferLengthBytes = BUFFER_SAMPLES * BYTES_PER_SAMPLE;

	// stereo buffer
	m_pStereoBuffer = new short[BUFFER_SAMPLES*2];
	memset(m_pStereoBuffer, 0, BufferSizeBytes());

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
		outputBuffer[(i<<1)+0] = m_pStereoBuffer[(i<<1)+0];
		outputBuffer[(i<<1)+1] = m_pStereoBuffer[(i<<1)+1];
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
		 m_pRtAudio->startStream();
		 m_bPlaying=true;
	}
	catch (RtError& e)
	{
		printf("RtError in Player::Play(): %s\n", e.getMessage().c_str());
		m_bPlaying=false;
	}
}

void Player::Stop()
{
	try {
		 m_pRtAudio->stopStream();
		 m_bPlaying=false;
	}
	catch (RtError& e)
	{
		 printf("RtError in Player::Stop(): %s\n", e.getMessage().c_str());
	}
}

int Player::GetPlayPositionSamples()
{
	// TODO
	return 0;
}
