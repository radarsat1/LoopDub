
#include "platform.h"
#include <stdio.h>

/*!
 * Set current thread to a lower priority
 */
void LOWPRIORITY()
{
#ifndef CYGWIN
	 sched_param param;
	 int policy, rc;
	 pthread_getschedparam(pthread_self(), &policy, &param);
	 param.sched_priority = 1;
	 rc = pthread_setschedparam(pthread_self(), policy, &param);
//	 printf("pthread_setschedparam(): %d\n", rc);
#endif
}

/*!
 * Set current thread to a higher priority
 */
void HIGHPRIORITY()
{
#ifndef CYGWIN
	 sched_param param;
	 int policy, rc;
	 pthread_getschedparam(pthread_self(), &policy, &param);
	 param.sched_priority = 1;
	 policy = SCHED_FIFO;
	 rc = pthread_setschedparam(pthread_self(), policy, &param);
	 printf("HI pthread_setschedparam(): %d\n", rc);
#endif
}

#ifdef CYGWIN


int main(int argc, char* argv[]);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine,int CmdShow)
{
  char *cmd[0] = {};
  printf("Here!\n");
  main(0, cmd);
  exit(0);
  return 0;
}

/* SDL depends on these two functions which are defined in MSVCRT.DLL,
   which we are not linking to, so provide stubs here. We are not using
   them anyways. */
unsigned long  _beginthreadex  (void *, unsigned, unsigned (__stdcall *) (void *),
								void*, unsigned, unsigned*)
{
}
void _endthreadex (unsigned)
{
}

#endif
