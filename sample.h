// Sample.h: interface for the Sample class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLE_H__A4994289_1F0A_490A_8AB6_9943B03DB4C6__INCLUDED_)
#define AFX_SAMPLE_H__A4994289_1F0A_490A_8AB6_9943B03DB4C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Sample
{
public:
	Sample();
	virtual ~Sample();

	bool LoadFromFile(char* filename);
	bool GenerateSine(int size);

	void MakeMinMax(int size);
	void GetMinMax(char** ppMin, char **ppMax, int *pSize);
	char* GetFilename() { return m_filename; }

	void Normalize();

public:
	short *m_pData;
	int m_nSamples;

	char *m_pMin, *m_pMax;
	int m_nMinMaxSize;

	char m_filename[1024];
};

#endif // !defined(AFX_SAMPLE_H__A4994289_1F0A_490A_8AB6_9943B03DB4C6__INCLUDED_)
