/*
 * catalog.c
 *
 *  Created on: 27.11.2011
 *      Author: stefan
 */

#include "catalog.h"

static LOADER_GLOBALS loader;
static BOOL initialized = false;
static BOOL catalogLoaded = false;

/**
 * Initializes and starts the loader process
 * @param loaderFile	        - Filename and path to the loader executable
 * @param catalogPath	        - Path to the catalog files
 * @return 			- true for success/ false for failure
 */
BOOL LoaderInit(const char* loaderFile, const char* catalogPath)
{
	BOOL result = false;

	if (loaderFile == NULL)
		loaderFile = "loader";

	if (catalogPath == NULL)
		catalogPath =  "catalogs/";

	char* args[] = {loaderFile, "-d", catalogPath, NULL};

	initialized = false;
	catalogLoaded = false;
	loader.fileList = NULL;
	loader.fileListSize = 0;
	loader.numFiles = 0;
	loader.sharedMemPtr = NULL;
	loader.sharedMemSize = 0;

	loader.fileList = malloc(MAX_BROWSE_RESPONSE);
	if (loader.fileList != NULL)
	{
		result = CGIProcess_Exec(&loader.process, loaderFile, (const char**)args);
		if (result == true)
		{
			initialized = true;
		}
		else
		{
			syslogWriteFormatDebug(lerror, "LoaderInit: failed to start loader!\n");
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror, "LoaderInit: failed to allocate memory!\n");
	}

	if (result == false)
	{
		if (loader.fileList != NULL)
			free(loader.fileList);
		loader.fileList = NULL;

		initialized = false;
		catalogLoaded = false;
		loader.fileListSize = 0;
		loader.numFiles = 0;
		loader.sharedMemPtr = NULL;
		loader.sharedMemSize = 0;
	}
	return result;
}
/**
 * Terminates the loader process and frees reserved resources
 * @return 						- true for success/ false for failure
 */
BOOL LoaderFree()
{
	BOOL result = false;
	if (initialized == true)
	{
		CGIProcess_WaitClose(&loader.process);
		if (loader.fileList != NULL)
			free(loader.fileList);
		loader.fileList = NULL;

		result = true;
	}
	return result;
}
/**
 * Executes the BROWSE command to get the list of catalog files
 * @return 						- true for success/ false for failure
 */
BOOL LoaderBrowseCatalogs()
{
	char* command = "BROWSE\n";
	int len = 0;
	int i;
	BOOL result = false;
	if (initialized == true)
	{
		if ( CGIProcess_SendToClient(&loader.process, command, strlen(command)) )
		{
			len = CGIProcess_RecvStringLinesClient(&loader.process, MAX_BROWSE_RESPONSE, loader.fileList, CGIPROCESS_READUNTIL_NL_TWICE);
			if (len > 0)
			{
				if (len < MAX_BROWSE_RESPONSE -1)
				{
					loader.numFiles = 0;
					for (i = 0; i < len; i++)
					{
						if (loader.fileList[i] == '\n')
						{
							loader.fileList[i] = '\0';
							loader.numFiles++;
						}
					}
					loader.numFiles--;
					loader.fileListSize = len;
					result = true;
				}
				else
				{
					syslogWriteFormatDebug(lerror, "LoaderBrowseCatalogs failed: Overflow of file list buffer!\n");
				}

			}
			else
			{
				syslogWriteFormatDebug(lerror, "LoaderBrowseCatalogs failed: failed to read BROWSE result!\n");
			}
		}
		else
		{
			syslogWriteFormatDebug(lerror, "LoaderBrowseCatalogs failed: to send BROWSE command!\n");
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror, "LoaderBrowseCatalogs failed: Loader not initialized!\n");
	}
	return result;
}
/**
 * Returns the list of catalog files. After each filename there is a 0-byte at the ending there are two 0-bytes.
 * @return 						- Pointer to list of filenames
 */
const char* LoaderGetFileList()
{
	if (initialized)
	{
		return loader.fileList;
	}
	else
	{
		syslogWriteFormatDebug(lerror, "LoaderGetFileList failed: Catalog was not loaded!\n");
		return NULL;
	}
}
/**
 * Executes the LOAD command which loads the declared catalog into a shared memory
 * @param filename	            - Filename of the catalog
 * @return 						- true for success/ false for failure
 */
BOOL LoaderLoadCatalog(const char* filename)
{
	BOOL result = false;
	char command[MAX_COMMAND +1] = {'\0'};  //+1 not really needed here
	char answer[MAX_LOAD_RESPONSE +1] = {'\0'}; //+1 to make sure there is a 0-Byte at the end (needed for logging)
	int length;
	if (initialized == true)
	{
		snprintf(command, MAX_COMMAND, "LOAD %s\n", filename);  //Should be always 0-terminated
		if ( CGIProcess_SendToClient(&loader.process, command, strlen(command)) > 0 )
		{
			length = CGIProcess_RecvStringLinesClient(&loader.process, MAX_LOAD_RESPONSE, &answer[0], CGIPROCESS_READUNTIL_NL);
			if (length > 0)
			{
				syslogWriteFormatDebug(lerror, "LoaderLoadCatalong result: %s\n", answer);

				if ( memcmp(answer, "LOADED", strlen("LOADED")) == 0 )
				{
					result = true;
				}
				else
				{
					syslogWriteFormatDebug(lerror, "LoaderLoadCatalong error: LOAD command failed\n");
				}
			}
			else
			{
				syslogWriteFormatDebug(lerror, "LoaderLoadCatalong error: reading failed\n");
			}
		}
		else
		{
			syslogWriteFormatDebug(lerror, "LoaderLoadCatalong error: writing failed\n");
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror, "LoaderLoadCatalog failed: Loader not initialized!\n");
	}
	return result;
}
/**
 * Opens the shared memory of the with LoaderLoadCatalog() loaded catalog
 * @return 						- true for success/ false for failure
 */
BOOL LoaderOpenCatalog()
{
	BOOL result = false;
	if (initialized)
	{
		loader.sharedMemPtr = SharedMemOpen(&loader.sharedMem, SHMEM_NAME);
		if (loader.sharedMemPtr != NULL)
		{
			loader.sharedMemSize = SharedMemGetMappedSize(&loader.sharedMem);
			catalogLoaded = true;
			result = true;
		}
		else
		{
			syslogWriteFormatDebug(lerror, "LoaderOpenCatalog failed: failed to open shared memory!\n");
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror, "LoaderOpenCatalog failed: Loader not initialized!\n");
	}
	return result;
}
/**
 * Closes the shared memory previously opened with LoaderOpenCatalog()
 * @return 						- true for success/ false for failure
 */
BOOL LoaderCloseCatalog()
{
	BOOL result = false;
	if ( catalogLoaded == true )
	{
		SharedMemClose(&loader.sharedMem);
		catalogLoaded = false;
		result = true;
	}
	else
	{
		syslogWriteFormatDebug(lwarn, "LoaderCloseCatalog: Cannot close catalog, because no catalog was opened!\n");
	}
	return result;
}
/**
 * Starts the loader process to list and open catalog files
 * @param loaderFile	        - Filename and path to the loader executable, can be NULL (defaults to "loader")
 * @param catalogPath	        - Path to the catalog files, can be NULL (defaults to "catalogs/")
 * @return 						- true for success/ false for failure
 */
BOOL CatalogInit(const char* loaderFile, const char* catalogPath)
{
	BOOL result = false;
	if ( LoaderInit(loaderFile, catalogPath) == true )
	{
		if ( LoaderBrowseCatalogs() == true )
		{
			result = true;
		}
		else
		{
			syslogWriteFormatDebug(lerror, "CatalogInit failed: to browse for catalog files in '%s'!\n", catalogPath);
		}
	}
	else
	{
		syslogWriteFormatDebug(lerror, "CatalogInit failed: Failed to initialize loader '%s'!\n", loaderFile);
	}
	return result;
}
/**
 * Terminates the loader process and frees reserved resources
 * @return 						- true for success/ false for failure
 */
BOOL CatalogFree()
{
	BOOL resultClose = false;
	BOOL resultFree = false;
	syslogWriteDebug(ldebug, "CatalogFree called...");
	if (initialized)
	{
		if ( catalogLoaded == true )
		{
			resultClose = LoaderCloseCatalog();
		}
		else
		{
			//No Catalog loaded...
			resultClose = true;
		}
		resultFree = LoaderFree();
	}
	else
	{
		syslogWriteDebug(lerror, "CatalogFree failed: loader was never successfully initialized before by a previous call to CatalogInit()");
	}
	return resultClose && resultFree;
}
/**
 * Returns the number of available catalogs (must be called after CatalogInit())
 * @return 						- number of catalog files, or 0 if failed
 */
int CatalogGetNumCatalogs()
{
	if (initialized)
	{
		return loader.numFiles;
	}
	else
	{
		syslogWriteDebug(lerror, "CatalogGetNumCatalogs failed: loader was never successfully initialized before by a previous call to CatalogInit()");
		return 0;
	}
}
/**
 * Returns a pointer to catalog file name with the declared index (must be called after CatalogInit()). The pointer must not be freed.
 * @return 						- char pointer to a filename
 */
const char* CatalogGetFilename(int idx)
{
	char* result = NULL;
	int i, count = 0;
	if (initialized)
	{
		if (loader.fileList != NULL)
		{
			if ( (idx >= 0) && (idx < loader.numFiles) )
			{
				i = 0;
				do
				{
					if (count == idx)
					{
						result = &loader.fileList[i];
						break;
					}
					if (loader.fileList[i] == '\0')
						count++;
					i++;
				} while (i < loader.fileListSize);
			}
			else
			{
				syslogWriteFormatDebug(lerror, "CatalogGetFilename error: invalid index\n");
			}
		}
		else
		{
			syslogWriteFormatDebug(lerror, "CatalogGetFilename error: no file list available\n");
		}
	}
	else
	{
		syslogWriteDebug(lerror, "CatalogGetFilename failed: loader was never successfully initialized before by a previous call to CatalogInit()");
	}
	return result;
}
/**
 * Executes the LOAD command which opens a catalog file into a shared memory
 * @return 						- true for success/ false for failure
 */
BOOL CatalogLoad(const char* filename)
{
	BOOL result = false;
	syslogWriteFormatDebug(ldebug, "CatalogLoad called for file %s", filename);
	if (initialized)
	{
		if (catalogLoaded == false)
		{
			if ( LoaderLoadCatalog(filename) )
			{
				if ( LoaderOpenCatalog() )
				{
					result = true;
				}
			}
		}
		else
		{
			syslogWriteDebug(lerror, "CatalogLoad failed: cannot load another catalog file!");
		}
	}
	else
	{
		syslogWriteDebug(lerror, "CatalogLoad failed: loader was never successfully initialized before by a previous call to CatalogInit()");
	}
	return result;
}
/**
 * Closes an already opened shared memory
 * @return 						- true for success/ false for failure
 */
BOOL CatalogClose()
{
	BOOL result = false;
	syslogWriteDebug(ldebug, "CatalogClose called...");
	if (initialized)
	{
		result = LoaderCloseCatalog();
		if (result == false)
			syslogWriteDebug(lerror, "CatalogClose failed: Close catalog failed!");
	}
	else
	{
		syslogWriteDebug(lerror, "CatalogClose failed: loader was never successfully initialized before by a previous call to CatalogInit()");
	}
	return result;
}
/**
 * Start pointer to the mapped shared memory
 * @return 						- Pointer to catalog data or NULL if failed
 */
const char* CatalogGetCatalogPtr()
{
	if (catalogLoaded)
	{
		return loader.sharedMemPtr;
	}
	else
	{
		syslogWriteDebug(lerror, "CatalogGetCatalogPtr failed: no catalog was loaded previously!");
		return NULL;
	}
}
/**
 * The size (in bytes) of the mapped shared memory
 * @return 						- Size in bytes or 0 if failed
 */
size_t CatalogGetCatalogByteSize()
{
	if (catalogLoaded)
	{
		return loader.sharedMemSize;
	}
	else
	{
		syslogWriteDebug(lerror, "CatalogGetCatalogByteSize failed: no catalog was loaded previously!");
		return 0;
	}
}


