
#include <stdio.h>
#include <string.h>
#include <sndfile.h>
#include "player.h"

int Player::m_nSampleRate;

Player::Player()
{
	m_pStereoBuffer = NULL;
	m_pFillBuffers = NULL;
	m_Param = NULL;

    m_pSRC = NULL;

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
// TODO: upgrade to newest rtaudio
//         if (m_pRtAudio->isStreamOpen())
             m_pRtAudio->closeStream();
		  delete m_pRtAudio;
		  m_pRtAudio = NULL;
	 }

     if (m_pSRC)
         src_delete(m_pSRC);
}

int callback( char* buffer, int bufferSize, void* userData)
{
	 ((Player*)userData)->Mix((float*)buffer, bufferSize, 0);

	 // true to stop the stream
	 return false;
}

bool Player::Initialize(void (FillBuffers)(void*, int), void* param, int samplerate, int hw_samplerate)
{
    m_nSampleRate = samplerate;
    m_nHwSampleRate = hw_samplerate;
	m_pFillBuffers = FillBuffers;
	m_Param = param;

    // stereo buffer will be allocated on first call to Mix()
    m_pStereoBuffer = NULL;
	m_nBufferLengthBytes = 0;

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
                               RTAUDIO_FLOAT32, m_nHwSampleRate,
							   &bufferSize, 3);
		
		m_pRtAudio->setStreamCallback(callback, this);
	}
	catch (RtError& e)
	{
		printf("RtError in Player::Initialize(): %s\n", e.getMessage().c_str());
		return false;
	}
		
	printf("RtAudio stream opened.\n");
	
	m_nSide = 1;

    /* Set up sample rate converter, if necessary. */
    if (m_nHwSampleRate != m_nSampleRate) {
        int error;
        m_pSRC = src_new(SRC_SINC_FASTEST, 2, &error);
        if (!m_pSRC)
            printf("Error initializing libsamplerate.\n");

        m_SRC_data.src_ratio = (double)m_nHwSampleRate / (double)m_nSampleRate;
        m_SRC_data.input_frames = 0;
        m_SRC_data.end_of_input = 0;
    }

	return true;
}

void Player::Mix(float *outputBuffer, unsigned long framesPerBuffer, int outTimeSample)
{
    // determine samples of pre-mixed sample rate
    unsigned long src_frames = framesPerBuffer;
    if (m_pSRC)
    {
        if (m_SRC_data.input_frames==0) {
            src_frames = (unsigned long)(src_frames / m_SRC_data.src_ratio + 0.5);
            m_SRC_data.input_frames = src_frames;
            printf("src_ratio = %f\n", m_SRC_data.src_ratio);
            printf("src_frames = %d, output_frames = %d\n", src_frames, framesPerBuffer);
        }
        else
            src_frames = m_SRC_data.input_frames;
    }

    // allocate the mixing buffer, if necessary
    if (!m_pStereoBuffer || BufferSizeSamples() != src_frames)
    {
        if (m_pStereoBuffer) {
            printf("Re-a");
            delete m_pStereoBuffer;
        }
        else
            printf("A");
        m_nBufferLengthBytes = src_frames * BYTES_PER_SAMPLE;
        m_pStereoBuffer = new float[src_frames * 2];
        m_SRC_data.data_in = m_pStereoBuffer;
        printf("llocated mixing buffer. (%d)\n", src_frames);
    }

	// call mix callback
	m_pFillBuffers(m_Param, outTimeSample);

    // samplerate conversion if necessary
    if (m_pSRC)
    {
        m_SRC_data.data_out = outputBuffer;
        m_SRC_data.output_frames = framesPerBuffer;
        src_process(m_pSRC, &m_SRC_data);
    }

	// or fill soundbuffer otherwise
    else {
        int i;
        for (i=0; i<(framesPerBuffer); i++)
        {
            outputBuffer[(i<<1)+0] = m_pStereoBuffer[(i<<1)+0];
            outputBuffer[(i<<1)+1] = m_pStereoBuffer[(i<<1)+1];
        }
    }

    /* Clip boundaries of sample data. */
    int i;
    for (i=0; i<(framesPerBuffer); i++)
    {
        if (outputBuffer[(i<<1)+0] > 1)
            outputBuffer[(i<<1)+0] = 1;
        if (outputBuffer[(i<<1)+0] < -1)
            outputBuffer[(i<<1)+0] = -1;
        if (outputBuffer[(i<<1)+1] > 1)
            outputBuffer[(i<<1)+1] = 1;
        if (outputBuffer[(i<<1)+1] < -1)
            outputBuffer[(i<<1)+1] = -1;
    }
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
