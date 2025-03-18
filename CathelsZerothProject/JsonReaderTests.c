#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "FileOps.h"
#include "CroJson.h"

int main(int argc, char* argv[])
{
	
	char testDir[] = "C:\\Users\\CathelRobertson\\source\\ownRepos\\CathelsZerothProject\\JsonReaderTests\\";
	char filepath[256];
	int numTests = 13;
	char tests[13][64] = {
		"Test1_EmptyObject_I",
		"Test1a_SingleCurlyOpen_I",
		"Test1b_SingleCurlyClose_I",
		"Test2_BasicString_V",
		"Test3_BasicValue_V",
		"Test4_BasicSubObjectWithString_V",
		"Test5_BasicSubObjectWithValue_V",
		"Test6_BasicStringList_1Entry_V",
		"Test7_BasicStringList_2Entry_V",
		"Test8_BasicValueList_1Entry_V",
		"Test9_BasicValueList_2Entry_V",
		"Test10_BasicObjectList_StandardObj_1Entry_V",
		"Test11_BasicObjectList_StandardObj_2Entry_V"
	};

	for (int i = 0;i < numTests;i++)
	{
		//Basically now it will be if we return null or not - Can also have some proper tests
		//Best way to test would probably be to have some jsonTrees, and some dud versionsm and then check what the final parsing results are.
		//Should be null for any invalid json, and correct for correct json
		char validity = tests[i][strlen(tests[i]-1)];
		printf("performing test: %s\n", tests[i]);
		strcpy(filepath, "");
		strcpy(filepath, testDir);
		char* jsonString = GetFileContents(strcat(strcat(filepath, tests[i]), ".txt"));
		if (jsonString == NULL)
		{
			PrintErr("File contents retrieval failed at test: %s", tests[i]);
			return -1;
		}
		
		if (!JsonValid(jsonString))
		{
			if (validity == 'V')
			{
				printf("\n->Test Failed: %s", tests[i]);
			}
		}
		else if (JsonValid(jsonString))
		{
			if (validity == 'I')
			{
				printf("\n->Test Failed: %s", tests[i]);
			}
		}
		free(jsonString);
	}

	return 0;
}

