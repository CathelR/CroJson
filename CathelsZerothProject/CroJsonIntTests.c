#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "FileOps.h"
#include "CroJson.h"


void PrintTestsStart(char*);
void PrintTestsEnd(char*);
void PrintSxs(int, bool);
void PrintRes(void*, enum PrintType);

void Test1();

enum PrintType
{
	P_STRING,
	P_FLOAT,
	P_INT,
	P_BOOL
};


int main(int argc, char* argv[])
{
	Test1();

	return 0;
}


void Test1()
{
	PrintTestsStart("Test1");
	char* jsonString = "{\"testVal1\":\"test\",\"testVal2\":\"test2\"}";
	TreeNode* root = GetJsonTree(jsonString);
	
	if (root == NULL)
	{
		PrintSxs(1, false);
	}

	TreeNode* result = SearchTree("testVal1", root);
	if (result != NULL)
	{
		PrintRes(result->stringVal, P_STRING);
		if (strcmp(result->stringVal, "test")==0)
		{
			PrintSxs(1,true);
		}
	}
	PrintTestsEnd("Test1");
	return;
}






void PrintSxs(int testNum, bool success)
{
	if (success)
	{
		printf("Test %d successful\n", testNum);
	}
	else
	{
		printf("Test %d failed\n", testNum);
	}
	return;
}

void PrintRes(void* input, enum PrintType pType)
{
	switch (pType)
	{
	case P_STRING:
		printf("->actual result: %s\n", (char*)input);
		break;
	case P_INT:
		printf("->actual result: %d\n", *(int*)input);
		break;
	case P_FLOAT:
		printf("->actual result: %f\n", *(float*)input);
		break;
	case P_BOOL:
		printf("->actual result: %d\n", *(bool*)input);
		break;
	default:
		printf("->Could'nt print actual result\n");
		break;
	}
	return;
}

void PrintTestsStart(char* testName)
{
	printf("Start of %s Tests\n-------------------------------------------------------------------------\n", testName);
	return;
}

void PrintTestsEnd(char* testName)
{
	printf("\n->End of %s Tests\n-------------------------------------------------------------------------\n\n\n", testName);
	return;
}