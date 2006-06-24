// Sample.cpp: implementation of the Sample class.
//
//////////////////////////////////////////////////////////////////////

#include "platform.h"
#include "sample.h"
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
	unsigned long dw, size;

	struct
	{
		short          wFormatTag;
		unsigned short wChannels;
		unsigned long  dwSamplesPerSec;
		unsigned long  dwAvgBytesPerSec;
		unsigned short wBlockAlign;
		unsigned short wBitsPerSample;
	} fmt;
	memset(&fmt, 0, sizeof(fmt));

	FILE *f = fopen(filename, "rb");
	if (f)
	{
	printf("Loading %s\n", filename);
		fread(&dw, 4, 1, f);
		char *s = (char*)&dw;
		dw = LITTLEENDIAN32(dw);
		if (dw!='RIFF')
		{
			fclose(f);
			return false;
		}

		fread(&dw, 4, 1, f);
		fread(&dw, 4, 1, f);
		dw = LITTLEENDIAN32(dw);
		if (dw!='WAVE')
		{
			fclose(f);
			return false;
		}

		fread(&dw, 4, 1, f);
		while (!feof(f))
		{
			dw = LITTLEENDIAN32(dw);
			if (dw=='fmt ')
			{
				fread(&size, 4, 1, f);
				ENDIANFLIP32(size);
				fread(&fmt, (size < sizeof(fmt)) ? size : sizeof(fmt), 1, f);
				size -= sizeof(fmt);
				while (size-- > 0)
					fread(&dw, 1, 1, f);
				ENDIANFLIP16(fmt.wFormatTag);
				ENDIANFLIP16(fmt.wChannels);
				ENDIANFLIP32(fmt.dwSamplesPerSec);
				ENDIANFLIP32(fmt.dwAvgBytesPerSec);
				ENDIANFLIP16(fmt.wBlockAlign);
				ENDIANFLIP16(fmt.wBitsPerSample);
			}
			else if (dw=='data' && !m_pData)
			{
				fread(&size, 4, 1, f);
				ENDIANFLIP32(size);
				size /= sizeof(short)*fmt.wChannels;
				m_nSamples = size;
				m_pData = new short[m_nSamples];
				if (m_pData)
				{
					short s, *p=m_pData;
					int i;
					while (size > 0)
					{
						*p = 0;
						for (i=0; i<fmt.wChannels; i++)
						{
							fread(&s, sizeof(short), 1, f);
							ENDIANFLIP16(s);
							*p += s/fmt.wChannels;
						}
						p++;
						size--;
					}
				}
				else
				{
					fclose(f);
					m_nSamples = 0;
					return false;
				}
			}
			else
			{
				fread(&size, 4, 1, f);
				while (size-- > 0)
					fread(&dw, 1, 1, f);
			}

			USLEEP(10);

			fread(&dw, 4, 1, f);
		}

		strcpy(m_filename, filename);
		fclose(f);
	}
	else
		return false;

	return true;
}
