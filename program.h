
#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include "def.h"
#include "platform.h"
#include "loopob.h"

class Program
{
  public:
	Program();
    static const int m_nNameSize = 64;
	char m_strName[m_nNameSize];
    static const int m_nDirSize = 256;
	char m_strDir[m_nDirSize];
	int m_nProgramNumber;
    static const int m_nFileNameSize = MAX_PATH;
	char m_strFile[N_LOOPS][m_nFileNameSize];
};

class ProgramChanger
{
  public:
	ProgramChanger();
	~ProgramChanger();

	void LoadPrograms();
	void ProgramChange(int program, LoopOb* m_pLoopOb[N_LOOPS]);
	static THREADFUNC loadBackgroundSamplesThread(void*);

	int NumPrograms() { return m_nPrograms; }
	char* GetProgramName(int n) { if (n >=0 && n < m_nPrograms) return m_Program[n].m_strName; else return NULL; }

  protected:
	Program *m_Program;      // Array of Program objects
	int m_nPrograms;         // Number of programs in m_Program

    //! Circular queue of samples to load.  Alternates between
	//  char* for filename and LoopOb* for the object it should
	//  be loaded into.
	void* m_pLoadingQueue[20];

	// Read and write pointers for m_pLoadingQueue
	int m_nLoadingQueueR;
	int m_nLoadingQueueW;

    // Handle to thread handling sample loading.  Thread will be created
    // if m_hThread is NULL.  Thread should set m_hThread to NULL before
    // exiting.
	HTHREAD m_hThread;
};

#endif // _PROGRAM_H_
