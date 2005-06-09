// Program.cpp: implementation of the ProgramChanger class.
//
//////////////////////////////////////////////////////////////////////

#include "program.h"
#include <stdio.h>
#include <string.h>

/////////////////////////////////
// Program

Program::Program()
{
	 int i;
	 for (i=0; i<N_LOOPS; i++)
		  m_strFile[i][0] = 0;
}

/////////////////////////////////
// ProgramChanger

ProgramChanger::ProgramChanger()
{
	 m_Program = NULL;
	 m_hThread = 0;
	 m_nLoadingQueueW = m_nLoadingQueueR = 0;
}

ProgramChanger::~ProgramChanger()
{
	 if (m_Program)
		  delete[] m_Program;
}

/*!
** Load programs from programs.ini
*/
void ProgramChanger::LoadPrograms()
{
	 FILE *file = fopen("programs.ini", "r");
	 if (!file) {
		  printf("No programs found.\n");
		  return;
	 }

	 char line[1024], param[256], value[MAX_PATH];
	 int len, pos, mark, n;
	 int number=1, loop=0;

	 // count number of programs
	 m_nPrograms = 0;
	 fgets(line, 1024, file);
	 while (!feof(file)) {
		  if (line[0]=='[')
			   m_nPrograms++;
		  fgets(line, 1024, file);
	 }

	 printf("%d program%s found.\n", m_nPrograms, m_nPrograms==1 ? "" : "s");
	 
	 // allocate
	 m_Program = new Program[m_nPrograms];

	 // rewind
	 fseek(file, 0, SEEK_SET);

	 n = -1;
	 fgets(line, 1024, file);
	 while (!feof(file)) {
		  len=strlen(line);

		  // strip LF
		  while (line[len-1]=='\r' || line[len-1]=='\n') {
			   line[len-1] = 0;
			   len--;
		  }

		  // strip comments
		  pos = 0;
		  while ((pos < len) && (line[pos]!=';'))
			   pos++;
		  if (pos < len) {
			   line[pos]=0;
			   len=pos;
		  }

		  // check for name
		  if (line[0]=='[') {
			   n++;
			   if (n >= m_nPrograms) break;
			   pos = 1;
			   while (line[pos] && line[pos]!=']' && pos<=64)
					m_Program[n].m_strName[pos-1] = line[pos++];

			   // Is it better to auto-increment program number
			   // or load it explictly from the file?
			   m_Program[n].m_nProgramNumber = number++;

			   loop = 1;
		  }
		  // check for parameter
		  else if (strchr(line, '=')!=NULL) {
			   pos=0;
			   // skip whitespace
			   while (line[pos] && (line[pos]==' ' || line[pos]=='\t'))
					pos++;
			   mark = pos;
			   // get parameter name
			   while (line[pos] && line[pos]!=' ' && line[pos]!='\t' && line[pos]!='=' && ((pos-mark)<256))
					param[pos-mark] = line[pos++];
			   param[pos-mark] = 0;
			   // skip to value
			   while (line[pos] && (line[pos]==' ' || line[pos]=='\t' || line[pos]=='='))
					pos++;
			   // get value (end at ;)
			   mark = pos;
			   while (line[pos] && line[pos]!=';' && ((pos-mark)<MAX_PATH))
					value[pos-mark] = line[pos++];
			   value[pos-mark] = 0;

			   // recognize parameter
			   if (strcmp(param, "Dir")==0) {
					strcpy(m_Program[n].m_strDir, value);
			   }
			   /*
			   else if (strcmp(param, "Program")==0) {
					m_Program[n].m_nProgramNumber = atoi(value);
			   }
			   */
			   else if (strncmp(param, "Loop", 4)==0) {
					//pos = atoi(param+4);
					pos = loop++;
					if (pos >= 1 && pos <= 8)
						 strcpy(m_Program[n].m_strFile[pos-1], value);
			   }
		  }

		  fgets(line, 1024, file);
	 }

/*
	 for (pos=0; pos<m_nPrograms; pos++) {
		  printf("[Program %d] Name: %s\n", m_Program[pos].m_nProgramNumber, m_Program[pos].m_strName);
		  for (n=0; n<N_LOOPS; n++)
			   if (m_Program[pos].m_strFile[n][0])
					printf("[Program %d] Loop %d: %s\n", m_Program[pos].m_nProgramNumber, n, m_Program[pos].m_strFile[n]);
	 }
*/

	 fclose(file);
	 printf("%d program%s loaded.\n", m_nPrograms, m_nPrograms==1 ? "" : "s");
}

/*!
 * Load the samples in the queue in order, and drop them into their corresponding LoopOb instances as
 * "background samples" once they are loaded.
 */

THREADFUNC ProgramChanger::loadBackgroundSamplesThread(void* param)
{
#ifndef WIN32
	 pthread_detach(pthread_self());
#endif
	 LOWPRIORITY();

	 ProgramChanger &pc = *((ProgramChanger*)param);

	 while (pc.m_nLoadingQueueR != pc.m_nLoadingQueueW) {
		  char *strFilename = (char*)pc.m_pLoadingQueue[pc.m_nLoadingQueueR++];
		  LoopOb *pLoopOb = (LoopOb*)pc.m_pLoadingQueue[pc.m_nLoadingQueueR++];
		  pc.m_nLoadingQueueR = pc.m_nLoadingQueueR % 20;

		  // Skip it if it has the same filename as foreground sample or previous background sample
		  if (pLoopOb->GetSample() && strcmp(pLoopOb->GetSample()->m_filename, strFilename)==0)
			   continue;
		  if (pLoopOb->GetBackgroundSample() && strcmp(pLoopOb->GetBackgroundSample()->m_filename, strFilename)==0)
			   continue;
		  
		  Sample *pSample = new Sample();
		  if (pSample->LoadFromFile(strFilename)) {
			   pLoopOb->SetBackgroundSample(pSample);
		  }
		  else
			   printf("Error loading %s as background sample.\n", strFilename);
	 }

	 pc.m_hThread = 0;
}

/*!
** Setup m_pLoopOb array to switch to new program on zero volume.
*/
void ProgramChanger::ProgramChange(int program, LoopOb* m_pLoopOb[N_LOOPS])
{
	 // Find the program
	 int i;
	 for (i=0; i<m_nPrograms; i++)
		  if (m_Program[i].m_nProgramNumber == program) break;
	 if (i>=m_nPrograms)
		  return;

	 // If we are already loading something, clear the queue.
	 m_nLoadingQueueR = m_nLoadingQueueW;

	 // Put each sample filename onto the queue along with its
	 // associated LoopOb object.
	 int loop;
	 for (loop=0; loop<N_LOOPS; loop++)
	 {
		  if (m_Program[i].m_strFile[loop][0]) {
			   m_pLoadingQueue[m_nLoadingQueueW] = m_Program[i].m_strFile[loop];
			   m_pLoadingQueue[m_nLoadingQueueW+1] = m_pLoopOb[loop];
			   m_nLoadingQueueW = (m_nLoadingQueueW+2) % 20;
		  }
		  if (m_Program[i].m_strDir[0]) {
			   m_pLoopOb[loop]->m_pFileBrowser->SetDirectoryFromBase(m_Program[i].m_strDir);
		  }
	 }

	 printf("Changing to program %d.\n", program);
	 if (!m_hThread)
		  CREATETHREAD(m_hThread, loadBackgroundSamplesThread, (void*)this);
}
