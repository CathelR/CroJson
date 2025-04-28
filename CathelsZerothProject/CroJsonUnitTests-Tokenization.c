#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "FileOps.h"
#include "CroJson.h"


enum PrintType
{
	P_STRING,
	P_FLOAT,
	P_INT,
	P_BOOL
};
void PrintTestsStart(char*);
void PrintTestsEnd(char*);
void PrintSxs(int, bool);
void PrintRes(void*, enum PrintType);


void TokenizationTests_Valid();


void PrintTokens(TokenPool* pool)
{
	if (pool != NULL)
	{
		for (int i = 0; i < pool->tokenCount; i++)
		{
			printf("%s | ", (pool->tokenPool + i)->content);
		}
	}
	else
	{
		PrintError("Resultant pool was NULL");
	}
	
}

int main(int argc, char* argv[])
{

	return 0;
}



void TokenizationTests_Valid()
{
	PrintTestsStart("Tokenization - Valid Json");

	char testJson1[] = "{\"TestString1\": \"AWord\"   ,\"TestValue1\" : true\n,\n\"TestValue2\" : 135,  \"TestValue3\" : 135.89    }\"}";
	JsonBuffer bufferT1 = {testJson1, 0, strlen(testJson1), 0};

	char testJson2[] = "{\"TestString1\": \"AWord\",  \"TestValue1\" :\n true,\"TestValue2\" : 135   ,\"TestObject1\" :\n {\"TestStringSub1\": \"SubWord\", \"TestObjectSub1\" :\n {\"TestStringSubSub1\": \"GettingSilly\"} }}";
	JsonBuffer bufferT2 = { testJson1, 0, strlen(testJson1), 0 };
	/*
	char testJson3[] = "";
	JsonBuffer bufferT3 = { testJson1, 0, strlen(testJson1), 0 };

	char testJson4[] = "";
	JsonBuffer bufferT4 = { testJson1, 0, strlen(testJson1), 0 };

	char testJson5[] = "";
	JsonBuffer bufferT5 = { testJson1, 0, strlen(testJson1), 0 };
	*/

	TokenPool* testResult1 = TokenizeJson(&bufferT1);

	PrintTokens(testResult1);


	PrintTestsEnd("Tokenization - Valid Json");
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