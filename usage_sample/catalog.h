/*
 * catalog.h
 *
 *  Created on: 27.11.2011
 *      Author: stefan
 */
#ifndef CATALOG_H_
#define CATALOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "apiutil.h"
#include "customtypes.h"
#include "syslog.h"
#include "server_loader_protocol.h"


#define MAX_COMMAND 0xFF
#define MAX_LOAD_RESPONSE 0xFF
#define MAX_BROWSE_RESPONSE 0xFFFF

typedef struct _LOADER_GLOBALS
{
	char* fileList;
	int fileListSize;
	int numFiles;
	CGIPROCESS process;
	SHAREDMEM sharedMem;
	void* sharedMemPtr;
	size_t sharedMemSize;
} LOADER_GLOBALS;

BOOL CatalogInit(const char* loaderFile, const char* catalogPath);
BOOL CatalogFree();
int CatalogGetNumCatalogs();
const char* CatalogGetFilename(int idx);
BOOL CatalogLoad(const char* filename);
BOOL CatalogClose();
const char* CatalogGetCatalogPtr();
size_t CatalogGetCatalogByteSize();

#endif
