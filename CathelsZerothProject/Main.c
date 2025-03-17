#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Models.h"


#define NAME_OF(var) (#var)

int main(int argc, char* argv[])
{
	bool filePathValid;
	char filePath[128];
	filePathValid =GetFilePath(filePath);

	if (!filePathValid)
	{
		printf("Invalid File Path. Aborting Execution");
		return 1;
	}

	printf("Parsing JSON Now...\n");

	char* jsonString = GetFileContents(filePath);
	if (jsonString == NULL)
	{
		PrintErr("File contents retrieval failed");
		return 2;
	}

	
	if (JsonValid(jsonString))
	{
		printf("\nJson Is Valid");
	}
	else
	{
		printf("\nJson Is Not Valid");
	}
	
	


	free(jsonString);
	jsonString = NULL;

	
	return 0;
}


