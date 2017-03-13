/*
 * apiutil.c
 *
 *  Created on: 23.10.2011
 *      Author: stefan
 */
#include "apiutil.h"


/**
 * Initalizes an instance of a MUTEX variable
 * @param mutex                 - Pointer to a variable of type MUTEX 
 * @param debugName             - Pointer to a unique name for debugging purpose (can be NULL)
 * @return 						- true for success/ false for failure
 */
BOOL mutexInit(PTR_MUTEX mutex, const char *debugName)
{
	int result = EINVAL;
	if (debugName == NULL)
	{
		debugName = "UNKNOWN";
	}

	if ( (mutex != NULL) )
	{
		mutex->debugName = debugName;
		result = pthread_mutex_init(&mutex->mutex, NULL);
		if (result != SUCCESS)
		{
			syslogWriteFormatDebug(lerror,"mutexInit failed: pthread_mutex_init failed with error code %i for mutex %s", result,  mutex->debugName);
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror,"mutexInit failed: pointer for mutex %s NULL",  mutex->debugName);
	}
    return (result == SUCCESS) ? true : false;
}

/**
 * Frees an instance of a MUTEX variable
 * @param mutex                 - Pointer to a variable of type MUTEX 
 * @return 						- true for success/ false for failure
 */
BOOL mutexFree(PTR_MUTEX mutex)
{
	HRESULT result = EINVAL;
	if (mutex != NULL)
	{
		#ifdef MUTEX_DEBUGGING
		syslogWriteFormatDebug(ldebug,"mutexFree: Freeing mutex: %s",  mutex->debugName);
		#endif
		result = pthread_mutex_destroy(&mutex->mutex);
		if (result != SUCCESS)
		{
			syslogWriteFormatDebug(lerror,"mutexFree failed: Freeing mutex %s failed with errorcode %i",  mutex->debugName, result);
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror,"mutexFree failed: pointer for mutex %s NULL",  mutex->debugName);
	}
    return (result == SUCCESS) ? true : false;
}

/**
 * Locks an instance of a MUTEX variable</br> Further locks will block until Mutex_UnLock() is called from the firt thread which calls Mutex_Lock().
 * @param mutex                 - Pointer to a variable of type MUTEX 
 * @return 						- true for success/ false for failure
 */
BOOL mutexLock(PTR_MUTEX mutex)
{
	HRESULT result = EINVAL;
	if (mutex != NULL)
	{
		#ifdef MUTEX_DEBUGGING
		syslogWriteFormatDebug(ldebug,"Locking mutex: %s",  mutex->debugName);
		#endif

		result = pthread_mutex_lock(&mutex->mutex);
		if (result != SUCCESS)
		{
			syslogWriteFormatDebug(lerror,"mutexLock failed: API call failed with errorcode %i for mutex %s", result, mutex->debugName);
		}

	}
	else
	{
		syslogWriteFormatDebug(lerror,"mutexLock failed: Mutex Pointer is Null!");
	}
    return (result == SUCCESS) ? true : false;
}

/**
 * Trys to lock an instance of a MUTEX variable</br> The function returns immediatly (returns EBUSY instread of blocking execution if mutex is already locked)
 * @param mutex                 - Pointer to a variable of type MUTEX 
 * @return 						- true for success/ false for failure
 */
BOOL mutexTryLock(PTR_MUTEX mutex)
{
	HRESULT result = EINVAL;
	if (mutex != NULL)
	{
		#ifdef MUTEX_DEBUGGING
		syslogWriteFormatDebug(ldebug,"TRY locking mutex: %s",  mutex->debugName);
		#endif

		result = pthread_mutex_trylock(&mutex->mutex);

		if (result == EBUSY)
		{
			#ifdef MUTEX_DEBUGGING
			syslogWriteFormatDebug(lerror,"mutexTryLock failed: Mutex %s is busy", mutex->debugName);
			#endif
		}
		else if ( result != SUCCESS )
		{
			syslogWriteFormatDebug(lerror,"mutexTryLock failed: API call failed with errorcode %i for mutex %s", result, mutex->debugName);
		}

	}
	else
	{
		syslogWriteFormatDebug(lerror,"mutexTryLock failed: Mutex Pointer is Null!");
	}
    return (result == SUCCESS) ? true : false;
}

/**
 * Releases a lock of a previous call to mutexLock().
 * @param mutex                 - Pointer to a variable of type MUTEX 
 * @return 						- true for success/ false for failure
 */
BOOL mutexUnLock(PTR_MUTEX mutex)
{
	HRESULT result = EINVAL;
	if (mutex != NULL)
	{
		#ifdef MUTEX_DEBUGGING
		syslogWriteFormatDebug(lerror,"Unlocking mutex: %s",  mutex->debugName);
		#endif

		result = pthread_mutex_unlock(&mutex->mutex);
		if (result != SUCCESS)
		{
			syslogWriteFormatDebug(lerror,"mutexUnLock failed: API call failed with errorcode %i for mutex %s", result, mutex->debugName);
		}

	}
	else
	{
		syslogWriteFormatDebug(lerror,"mutexUnLock failed: Mutex Pointer is Null!");
	}
    return (result == SUCCESS) ? true : false;
}


/**
 * Creates a new thread
 * @param thread                - Pointer to a variable of type THREAD 
 * @param startAddr             - Pointer to a function with prototype void function(void* startParam)
 * @param startParam            - Pointer to additional start parameter (can be NULL) 
 * @return 			- true for success/ false for failure
 */
BOOL threadCreate(PTR_THREAD thread, const void *startAddr, const void *startParam)
{
  HRESULT result = EINVAL;
  if (thread != NULL)
  {
    if (startAddr != NULL)
    {
      result = pthread_create(&thread->thread, NULL, startAddr, startParam);
      if (result)
      {
         syslogWriteFormatDebug(lerror, "threadCreate failed: pthread_create failed with error code %i", result);
      }
    }
    else
    {
       syslogWriteFormatDebug(lerror, "threadCreate failed: start address cannot be NULL");
    }
  }
  else
  {
     syslogWriteFormatDebug(lerror, "threadCreate failed: address form THREAD object is NULL");
  } 
  return (result == SUCCESS) ? true : false;
}
/**
 * Waits until a thread terminates
 * @param thread                - Pointer to a variable of type THREAD 
 * @return 						- true for success/ false for failure
 */
BOOL threadWait(PTR_THREAD thread)
{
  HRESULT result = EINVAL;
  if (thread != NULL)
  {
    result = pthread_join(thread->thread, NULL);
  }
  else
  {
     syslogWriteFormatDebug(lerror, "threadWait failed: Thread object is NULL");
  }
  return (result == SUCCESS) ? true : false;
}
/**
 * Terminates the calling thread
 * @return 						- nothing (Function does not return!)
 */
void threadExit()
{
  pthread_exit(NULL);
}





/**
 * Creates a new thread local storage
 * @param tls                	- Pointer to a variable of type TLS 
 * @return 						- true for success/ false for failure
 */
BOOL TLSAlloc(PTR_TLS tls)
{
   HRESULT result = EINVAL;
   if (tls != NULL)
   {
	   result = pthread_key_create(&tls->key, NULL); //no destructor
	   if (result == SUCCESS)
	   {
			result = pthread_setspecific(tls->key, NULL); //make sure TLS is initalized with NULL
			if (result != SUCCESS)
			{
				pthread_key_delete(&tls->key); // Free already created TLS
				syslogWriteFormatDebug(lerror,"TLSAlloc failed: setting key value failed with error code %i!", result);
			}
	   }
	   else
	   {			
			syslogWriteFormatDebug(lerror,"TLSAlloc failed: creation of key failed with error code %i!", result);
	   }
   }
   else
   {
	  syslogWriteFormatDebug(lerror,"TLSAlloc failed: pointer for TLS is NULL!");
   }
   return (result == SUCCESS) ? true : false;
}
/**
 * Frees a thread local storage previously alloacted with TLSAlloc()
 * @param tls	                - Pointer to a variable of type TLS 
 * @return 						- true for success/ false for failure
 */
BOOL TLSFree(PTR_TLS tls)
{
  HRESULT result = EINVAL;;
  if (tls != NULL)
  {
    result = pthread_key_delete(tls->key);
	if (result != SUCCESS)
	{
	  syslogWriteFormatDebug(lerror,"TLSFree failed: deletion of key failed with error code %i!", result);
	}
  }
  else
  {
	 syslogWriteFormatDebug(lerror,"TLSFree failed: pointer for TLS is NULL!");
  }
  return (result == SUCCESS) ? true : false;
}
/**
 * Sets the value of a thread local storage previously alloacted with TLSAlloc()
 * @param tls	                - Pointer to a variable of type TLS
 * @param value	                - Pointer to a variable which should be stored in the TLS
 * @return 						- true for success/ false for failure
 */
BOOL TLSSetValue(PTR_TLS tls, const void* value)
{
  HRESULT result = EINVAL;
  if (tls != NULL)
  { 
	result = pthread_setspecific(tls->key, value);
	if (result != SUCCESS)
	{
		syslogWriteFormatDebug(lerror,"TLSSetValue failed: setting key value failed with error code %i!", result);
	}
  }
  else
  {
	 syslogWriteFormatDebug(lerror,"TLSSetValue failed: pointer for TLS is NULL!");
  }
  return (result == SUCCESS) ? true : false;
}
/**
 * Gets the value of a thread local storage
 * @param tls	                - Pointer to a variable of type TLS
 * @return 						- Value stored previously with TLSSetValue()
 */
void* TLSGetValue(PTR_TLS tls)
{
  if (tls != NULL)
  { 
     return pthread_getspecific(tls->key);
  }
  else
  {
	 syslogWriteFormatDebug(lerror,"TLSGetValue failed: pointer for TLS is NULL!");
     return NULL;	 
  }
}









/**
 * Maps an already existing shared memory into the current process. 
 * @param mem	                - Pointer to a variable of type SHAREDMEM
 * @param name	                - Name of the shared memory
 * @return 						- the pointer to the first byte of the mapped memory
 */
void* SharedMemOpen(PTR_SHAREDMEM mem, const char* name)
{
	int fd = 0;
	void *pointer = NULL;
	HRESULT result = EINVAL;
	size_t size;
	struct stat st;
	
	if (mem != NULL)
	{
	    if (name != NULL)
		{
			fd = shm_open(name,  O_RDWR, (mode_t)0600); /*600???*/  //O_RDWR
			if (fd >= 0)
			{
				if (fstat(fd, &st) == SUCCESS)
				{
					size = (int)st.st_size;
				}
				if ( (size == st.st_size) && (size != 0) )
				{
					pointer = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 );
					if (pointer != MAP_FAILED)  //MAP_FAILED != NULL!
					{
						mem->name = name;
						mem->pointer = pointer;
						mem->fd = fd;
						mem->size = size;
						result = SUCCESS;
					}
					else
					{
						syslogWriteFormatDebug(lerror,"SharedMemOpen failed: unable to map shared memory '%s'!", name);
					}
				}
				else
				{
					syslogWriteFormatDebug(lerror,"SharedMemOpen failed: invalid size of shared memory '%s'!", name);
				}
			}
			else
			{
				syslogWriteFormatDebug(lerror,"SharedMemOpen failed: unable to open shared memory '%s'!", name);
			}
		}
		else
		{
			syslogWriteFormatDebug(lerror,"SharedMemOpen failed: parameter name is NULL!");
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror,"SharedMemOpen failed: pointer for SHAREDMEM is NULL!");
	}

	if (result != SUCCESS)
	{
		if (pointer != NULL)
		{
			/* should never be called... */
			munmap(pointer, size);
		}
		if (fd != 0)
		{
			close(fd);
			shm_unlink(name);
		}
		return NULL;
	}
	else
	{
		return pointer;
	}
}
/**
 * Maps an already existing shared memory into the current process. 
 * @param mem	                - Pointer to a variable of type SHAREDMEM
 * @return 						- true for success/ false for failure
 */
BOOL SharedMemClose(PTR_SHAREDMEM mem)
{
	if (mem != NULL)
	{
		if (mem->pointer != NULL)
		{
			munmap(mem->pointer, mem->size);
		}
		if (mem->fd != 0)
		{
			close(mem->fd);
			shm_unlink(mem->name);
		}    
		return true;
	}
	else
	{
		syslogWriteFormatDebug(lerror,"SharedMemClose failed: pointer for SHAREDMEM is NULL!");
		return false;
	}	
}

size_t SharedMemGetMappedSize(PTR_SHAREDMEM mem)
{
	if (mem != NULL)
	{
		return mem->size;
	}
	else
	{
		syslogWriteFormatDebug(lerror,"SharedMemGetMappedSize failed: pointer for SHAREDMEM is NULL!");
		return 0;
	}
}



 /**
 * Starts an asynchronous child process and redirects stdout and stdin
 * @param p	                - Pointer to a variable of type CGIPROCESS
 * @param filepath             	- Path to the executable
 * @param args      	       	- Null-terminated arrays of parameters
 * @return 			- true for success/ false for failure
 */
BOOL CGIProcess_Exec(PTR_CGIPROCESS p, const char* filepath, const char** args)
{
	BOOL result = false;
	
	if (p != NULL)
	{
		p->closed = false;
		p->pid = -1;
		p->file = NULL;
		p->stdinPipe[0] = -1;
		p->stdinPipe[1] = -1;
		p->stdoutPipe[0] = -1;
		p->stdoutPipe[1] = -1;		
		
		if (filepath != NULL)
		{
			//Test if file exists
			FILE *f = fopen(filepath, "r");
			if (f != NULL)
			{
				fclose(f);
				pipe(p->stdinPipe);
				pipe(p->stdoutPipe);

				if(pipe(p->stdinPipe) != -1 && pipe(p->stdoutPipe) != -1)
				{

					p->pid = fork();
					if(p->pid == 0)
					{	
						//Child process
						/* Redirection of stdin */
						if(dup2(p->stdinPipe[0], STDIN_FILENO) == -1)
						{
						  syslogWriteFormatDebug(lerror,"CGIProcess_Exec failed: dup2(stdinPipe[0], STDIN_FILENO) failed, error: %s\n", strerror( errno ));
						}

						/* Redirection of stdout */
						if(dup2(p->stdoutPipe[1], STDOUT_FILENO) == -1)
						{
						  syslogWriteFormatDebug(lerror,"CGIProcess_Exec failed: dup2(stdoutPipe[1], STDOUT_FILENO) failed, error: %s\n", strerror( errno ));
						}
						close(p->stdinPipe[0]); close(p->stdinPipe[1]);
						close(p->stdoutPipe[0]); close(p->stdoutPipe[1]);

						execv(filepath, args);
						syslogWriteFormatDebug(lerror,"CGIProcess_Exec failed: execute child '%s' failed! Reason is: %s", filepath, strerror( errno ));  // <-- this will be called just if execv failed
						exit(EXIT_FAILURE); //Terminate if execv failed!
					}
					else if (p->pid > 0)
					{
						//Parent process	
						close(p->stdinPipe[0]);
						close(p->stdoutPipe[1]);

						p->file = filepath;
						p->closed = false;
						result = true;
					}
					else
					{
						syslogWriteFormatDebug(lerror, "CGIProcess_Exec failed: fork() failed for '%s'!\n", filepath);
					}			
				}
				else
				{
					syslogWriteFormatDebug(lerror, "CGIProcess_Exec failed: creating pipe failed for '%s'!\n", filepath);
					if (p->stdinPipe[0] == -1)
						close(p->stdinPipe[0]);
					if (p->stdinPipe[1] == -1)
						close(p->stdinPipe[1]);	

					if (p->stdinPipe[0] == -1)
						close(p->stdinPipe[0]);
					if (p->stdinPipe[1] == -1)
						close(p->stdinPipe[1]);					
				}
			}
			else
			{
				syslogWriteFormatDebug(lerror, "CGIProcess_Exec failed: cannot find file '%s'!\n",filepath);
			}
		}
		else
		{
			syslogWriteFormatDebug(lerror, "CGIProcess_Exec failed: file is NULL!\n");
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror, "CGIProcess_Exec failed: p is NULL!\n");
	}
	return result;
}
 /**
 * Waits until the child process started with CGIProcess_Exec() terminates
 * @param p	            - Pointer to a variable of type CGIPROCESS
 * @return 				- true for success/ false for failure 
 */
BOOL CGIProcess_WaitClose(PTR_CGIPROCESS p)
{
	BOOL result = false;
	if (p != NULL)
	{
		if (p->closed == false)
		{
			p->closed = true;

			//Close pipes before waitpid!
			if (p->stdinPipe[1] >= 0)
			{
				close(p->stdinPipe[1]);
				p->stdinPipe[1] = -1;
			}
			if (p->stdoutPipe[0] >= 0)
			{
				close(p->stdoutPipe[0]);
				p->stdoutPipe[0]  = -1;
			}	
			
			if (p->pid > 0)
			{	
				waitpid(p->pid, &result, 0); //wait until process exits
				p->pid = -1;
			}
			else
			{
				syslogWriteFormatDebug(lerror, "CGIProcess_WaitClose failed: invalid pid\n");
			}
				
			p->file = NULL;
			result = true;
	
		}	
		else
		{
			syslogWriteFormatDebug(lerror, "CGIProcess_WaitClose failed: Process already closed\n");
		}
	}
}
 /**
 * Sends data to the child process
 * @param p	                	- Pointer to a variable of type CGIPROCESS
 * @param msgBuffer	                - Pointer to an char array
 * @param length	            	- size of the char array in bytes
 * @return 				- true for success/ false for failure 
 */
BOOL CGIProcess_SendToClient(PTR_CGIPROCESS p, const char* msgBuffer, size_t length)
{
	BOOL result = false;
	size_t writtenBytes = -1;
	if (p != NULL)
	{
		//update closed state
		if ( waitpid(p->pid, &result, WNOHANG) != 0) // WNOHANG to return immediately
		{
			p->closed = true;
		}
	
		if (p->closed == false )
		{
			if (msgBuffer != NULL)
			{
					if (p->stdinPipe[1] >= 0)
					{
						writtenBytes = write(p->stdinPipe[1], msgBuffer, length);
						if (writtenBytes == length)
						{	
							result = true;
						}
						else
						{
							syslogWriteFormatDebug(lerror, "CGIProcess_SendToClient failed: error while writing, %i of %i bytes written, error: %s\n", (int)writtenBytes, (int)length, strerror( errno ));
						}
					}
					else
					{
						syslogWriteFormatDebug(lerror, "CGIProcess_SendToClient failed: stdinPipe[1] is invalid (%i)\n", p->stdinPipe[1]);
					}
			}
			else
			{
				syslogWriteFormatDebug(lerror, "CGIProcess_SendToClient failed: msgBuffer is NULL\n");
			}
		}
		else
		{
			syslogWriteFormatDebug(lerror, "CGIProcess_SendToClient failed: child process is closed\n");
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror, "CGIProcess_SendToClient failed: p is NULL\n");
	}
	return result;
}
 /**
 * Receives data from the child process
 * @param p	                	- Pointer to a variable of type CGIPROCESS
 * @param maxlength	                - maximum number of bytes to read
 * @param msgBuffer                	- Pointer to an char array which will be filled
 * @return 				- Bytes read (could be less than parameter maxlength but at least 1, or 0 if EOF reached, -1 if failed )
 */
size_t CGIProcess_RecvFromClient(PTR_CGIPROCESS p, size_t maxlength, char* msgBuffer)
{
	BOOL result = false;
	int readBytes = -1;
	if (p != NULL)
	{	
		//update closed state
		if ( waitpid(p->pid, &result, WNOHANG) != 0)
		{
			p->closed = true;
		}
		
		if (p->closed == false)
		{
			if (msgBuffer != NULL)
			{
				/* When reading from  pipe: read() returns 0 (end of file) when the
				 write end is closed. If write over the end and the pipe is still open read() will block.
				 If read() tries to read more data than available, it will return only the number of bytes available. */
				if (p->stdoutPipe[0] >= 0)
				{
					readBytes = read(p->stdoutPipe[0], msgBuffer, maxlength);
					if (readBytes == -1)
					{
						syslogWriteFormatDebug(lerror, "CGIProcess_RecvFromClient failed: error while reading - %s\n", strerror( errno ));
					}
				}				
				else
				{
					syslogWriteFormatDebug(lerror, "CGIProcess_RecvFromClient failed: stdoutPipe[0] is invalid (%i)\n", p->stdoutPipe[0]);
				}
			}
			else
			{
				syslogWriteFormatDebug(lerror, "CGIProcess_RecvFromClient failed: msgBuffer is NULL\n");
			}			
		}
		else
		{
			syslogWriteFormatDebug(lerror, "CGIProcess_RecvFromClient failed: child process is closed\n");
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror, "CGIProcess_RecvFromClient failed: p is NULL\n");
	}
	return readBytes;	
}


 /**
 * Receives string lines from the child process (until \n\n or \n occurs). However this works only if \n or \n\n are the last characters in the Pipe!
 * @param p	                	- Pointer to a variable of type CGIPROCESS
 * @param maxlength	            - maximum number of bytes to read
 * @param msgBuffer             - Pointer to an char array which will be filled
 * @param readUntil				- when reading should be aborted. CGIPROCESS_READ_NL (for 1 new line char) or CGIPROCESS_READ_NL_TWICE (for 2 new line char)
 * @return 						- Bytes read (length or -1 if failed )
 */
size_t CGIProcess_RecvStringLinesClient(PTR_CGIPROCESS p, size_t maxlength, char* msgBuffer, int readUntil)
{
	size_t i;
	size_t rest = maxlength;
	size_t readBytes = 0, length = 0;
	do
	{
		readBytes = CGIProcess_RecvFromClient(p, rest, msgBuffer + length);
		if (readBytes < 0)
		{
			length = -1;
			break; //Error happened
		}

		rest -= readBytes;
		length += readBytes;

		if (readUntil == CGIPROCESS_READUNTIL_NL_TWICE)
		{
			if (length >= 2)
			{
				if ( (msgBuffer[length - 2] == '\n') && (msgBuffer[length - 1] == '\n') )
				{
					break;
				}
			}
		}
		else
		{
			if (length >= 1)
			{
				if ( (msgBuffer[length - 1] == '\n') )
				{
					break;
				}
			}
		}

	}
	while (rest > 0 && readBytes > 0);
	return length;
}










