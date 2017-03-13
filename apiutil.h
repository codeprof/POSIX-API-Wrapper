/*
 * apiutil.h
 *
 *  Created on: 23.10.2011
 *      Author: stefan
 */
#ifndef APIUTIL_H_
#define APIUTIL_H_

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h> 

#include "customtypes.h"
#include "syslog.h"

//#define MUTEX_DEBUGGING

//----------------------------------- Mutex Definitions -----------------------------------
typedef struct _MUTEX
{
	pthread_mutex_t mutex;
	char* debugName;
} MUTEX;

typedef MUTEX* PTR_MUTEX;

BOOL mutexInit(PTR_MUTEX mutex, const char *debugName);
BOOL mutexFree(PTR_MUTEX mutex);
BOOL mutexLock(PTR_MUTEX mutex);
BOOL mutexTryLock(PTR_MUTEX mutex);
BOOL mutexUnLock(PTR_MUTEX mutex);


//Initialisation:
// static MUTEX test_mutex;
// ...
// mutexInit(&test_mutex, "TEST MUTEX");

//Usage:
// mutexLock(&test_mutex) <-- Enter critical section
// ...
// mutexUnLock(&test_mutex) <-- Leave critcal section

//-----------------------------------------------------------------------------------------

//---------------------------------- Thread Definitions -----------------------------------
typedef struct _THREAD
{
  pthread_t thread;
} THREAD;

typedef THREAD* PTR_THREAD;

BOOL threadCreate(PTR_THREAD thread, const void *startAddr, const void* startParam);
BOOL threadWait(PTR_THREAD thread);
void threadExit();
//-----------------------------------------------------------------------------------------

//---------------------------------- TLS Definitions -----------------------------------
typedef struct _TLS
{
  pthread_key_t key;
} TLS;

typedef TLS* PTR_TLS;

BOOL TLSAlloc(PTR_TLS tls);
BOOL TLSFree(PTR_TLS tls);
BOOL TLSSetValue(PTR_TLS tls, const void* value);
void* TLSGetValue(PTR_TLS tls);
//-----------------------------------------------------------------------------------------

//------------------------------- Shared Memory Definitions -------------------------------
typedef struct _SHAREDMEM
{
	int fd;        /* file descriptor handle */
	char* name;    /* unique name of the shared memory */
	void* pointer; /* pointer to the content of the shared memory */
	size_t size;   /* size of the locked buffer */
} SHAREDMEM;

typedef SHAREDMEM* PTR_SHAREDMEM;

void* SharedMemOpen(PTR_SHAREDMEM mem, const char* name);
BOOL SharedMemClose(PTR_SHAREDMEM mem);
size_t SharedMemGetMappedSize(PTR_SHAREDMEM mem);
//-----------------------------------------------------------------------------------------


//-------------------------------------- CGI Definitions ----------------------------------
struct _CGIPROCESS
{
    BOOL closed;
	int stdinPipe[2];
	int stdoutPipe[2];
	pid_t pid;
	char* file;
	//FILE *tochild;
	//FILE *fromchild;
};

typedef struct _CGIPROCESS CGIPROCESS;
typedef CGIPROCESS* PTR_CGIPROCESS;

#define CGIPROCESS_READUNTIL_NL 0
#define CGIPROCESS_READUNTIL_NL_TWICE 1

BOOL CGIProcess_Exec(PTR_CGIPROCESS p, const char* filepath, const char** args);
BOOL CGIProcess_WaitClose(PTR_CGIPROCESS p);
BOOL CGIProcess_SendToClient(PTR_CGIPROCESS p, const char* msgBuffer, size_t length);
size_t CGIProcess_RecvFromClient(PTR_CGIPROCESS p, size_t maxlength, char* msgBuffer);
size_t CGIProcess_RecvStringLinesClient(PTR_CGIPROCESS p, size_t maxlength, char* msgBuffer, int readUntil);
//-----------------------------------------------------------------------------------------


#endif

