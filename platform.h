// Platform-specific stuff
#ifdef WIN32
#include <windows.h>
#include <direct.h>

#ifndef MAX_PATH
#define MAX_PATH _MAX_PATH
#endif

#define HTHREAD DWORD
#define HMUTEX HANDLE

#define THREADFUNC DWORD WINAPI 
#define LOCKMUTEX(mtx)    WaitForSingleObject(mtx, INFINITE)
#define UNLOCKMUTEX(mtx)  ReleaseMutex(mtx)

#define CREATEMUTEX(mutex) mutex = CreateMutex(NULL, FALSE, NULL)
#define DESTROYMUTEX(mutex) CloseHandle(mutex)

// return true on success
#define CREATETHREAD(threadID, thread_function, param) \
	(CreateThread(NULL, 0, thread_function, param, 0, &threadID)!=INVALID_HANDLE_VALUE)

#define USLEEP(n) Sleep(n);

#define chdir _chdir

#else				/* Linux */
#include <pthread.h>
#include <unistd.h>

#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif

#define HTHREAD pthread_t
#define HMUTEX pthread_mutex_t

#define THREADFUNC void*
#define LOCKMUTEX(mtx)   pthread_mutex_lock(&mtx)
#define UNLOCKMUTEX(mtx) pthread_mutex_unlock(&mtx)

#define CREATEMUTEX(mutex) pthread_mutex_init(&mutex, NULL)
#define DESTROYMUTEX(mutex) pthread_mutex_destroy(&mutex)

// return true on success
#define CREATETHREAD(threadID, thread_function, param) \
	 (!(pthread_create(&threadID, NULL, thread_function, param)))

#define USLEEP(n) usleep(n);

#endif				/*  */

void LOWPRIORITY();
void HIGHPRIORITY();
