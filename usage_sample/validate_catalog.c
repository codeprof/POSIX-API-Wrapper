#include <stdio.h>
#include "catalog.h"

BOOL validateCatalogModule()
{
	int i;

	printf("==============\n\STARTING CATALOG MODULE VAILIDATION\n");
	if (!CatalogInit("loader", "catalogs/"))
	{
		printf("ERROR: INITALIZING FAILED!\n");
		return false;
	}


	if (CatalogGetNumCatalogs() <= 0)
	{
		printf("ERROR: NO CATALOG FOUND!\n");
		CatalogFree();
		return false;
	}
	printf("Num catalogs: %i\n", CatalogGetNumCatalogs());

	for (i = 0; i < CatalogGetNumCatalogs();i++)
	{
		printf("%s\n", CatalogGetFilename(i));
		if ( CatalogGetFilename(i) == NULL)
		{
			printf("ERROR: CATALOG NAME PTR IS NULL!\n");
			CatalogFree();
			return false;
		}
	}

	shm_unlink("/quiz_example_solution"); //Make sure shared memory is deleted...
	if ( !CatalogLoad("simple.cat") )
	{
		printf("ERROR: CATALOG simple.cat NOT LOADED!\n");
		CatalogFree();
		return false;
	}
	if (CatalogGetCatalogPtr() != NULL)
	{
		printf("content of catalog is %s\n", CatalogGetCatalogPtr());
	}
	else
	{
		printf("ERROR: CATALOG POINTER IS NULL!");
		CatalogFree();
		return false;
	}
	if (CatalogGetCatalogByteSize() > 0)
	{
		printf("content length of catalog is %i\n", (int)CatalogGetCatalogByteSize());
	}
	else
	{
		printf("ERROR: BAD CATALOG LENGTH!");
		CatalogFree();
		return false;
	}
	CatalogFree();
	printf("===================\n");
	printf("ALL OK!");
	return true;
}


int main()
{
	validateCatalogModule();
	return 0;
}

