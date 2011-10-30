// Sample.cpp: implementation of the Sample class.
//
//////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "sample.h"

#include <sndfile.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

Sample::Sample()
{
	m_pData = 0;
	m_nSamples = 0;
	m_pMin = m_pMax = 0;
}

Sample::~Sample()
{
	if (m_pData)
		delete m_pData;
}

bool Sample::GenerateSine(int size)
{
	m_pData = new short[size];
	if (!m_pData) return false;
	m_nSamples = size;

	int i;
	for (i=0; i<size; i++)
		m_pData[i] = (short)(sin(i/100) * (65536/2));

	return true;
}

void Sample::MakeMinMax(int size)
{
	if (!m_pData)
		return;

	if (!m_pMin || !m_pMax || (m_nMinMaxSize!=size))
	{
		if (m_pMin) delete m_pMin;
		if (m_pMax) delete m_pMax;

		m_nMinMaxSize = size;
		m_pMin = new char[m_nMinMaxSize];
		m_pMax = new char[m_nMinMaxSize];
	}

	memset(m_pMin, 127, m_nMinMaxSize);
	memset(m_pMax, -128, m_nMinMaxSize);

	int i, pos;
	char s;
	for (i=0; i<m_nSamples; i++)
	{
		 pos = i * m_nMinMaxSize / m_nSamples;
		 s = m_pData[i] >> 9;
		 if (s > m_pMax[pos])
			  m_pMax[pos] = s;
		 if (s < m_pMin[pos])
			  m_pMin[pos] = s;
	}
}

void Sample::GetMinMax(char** ppMin, char **ppMax, int *pSize)
{
	if (ppMin)
		*ppMin = m_pMin;
	if (ppMax)
		*ppMax = m_pMax;
	if (pSize)
		*pSize = m_nMinMaxSize;
}

void Sample::Normalize()
{
	 int i;
	 short max=0, abs;
	 for (i=0; i<m_nSamples; i++) {
		  abs = (m_pData[i]<0) ? -m_pData[i] : m_pData[i];
		  if (abs > max)   max = abs;
	 }

	 for (i=0; i<m_nSamples; i++)
		  m_pData[i] = ((int)m_pData[i])*32767/max;
}

//////////////////////////
// .wav file loading

bool Sample::LoadFromFile(char *filename)
{
	SF_INFO fileinfo;
	SNDFILE *file = sf_open(filename, SFM_READ, &fileinfo);
	if (!file)
		return false;

	strncpy(m_filename, filename, 1024);

	m_nSamples = fileinfo.frames;
	m_pData = new short[m_nSamples];
	if (!m_pData) {
		sf_close(file);
		printf("Not enough memory!\n");
		m_pData = NULL;
		m_nSamples = 0;
		return false;
	}

	int i=0, t, c, k;
	short buf[8192];
	while (i<m_nSamples)
	{
	  int ntoread = 8192, nread;
	  if ((m_nSamples-i)*fileinfo.channels < ntoread)
	    ntoread = (m_nSamples-i)*fileinfo.channels;

	  nread = sf_read_short(file, buf, ntoread);
	  k=0;
	  for (t=0; t<nread;) {
	    int sample=0;
	    for (c=0; c<fileinfo.channels; c++)
	      sample += buf[t++];
	    m_pData[i+(k++)] = (short)(sample / fileinfo.channels);
	  }

	  i += k;
	}

	sf_close(file);
	return true;
}
