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
void Test2();
void Test3();
void Test4();
void Test5();
void Test6();


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
	Test2();
	Test3();
	Test4();
	Test5();
	Test6();

	return 0;
}

void Test6()
{
	PrintTestsStart("Test6");
	char* jsonString = "{\"testVal1\":  \"test\",\"testVal2\":{  \"subValName\" : 19.4 ,   \"subVal2Name\" :\n [\"word1\",\"word2\",\"word3\"]  }} }";
	TreeNode* root = GetJsonTree(jsonString);

	if (root == NULL)
	{
		PrintSxs(6, false);
	}

	TreeNode* result = SearchTree("subVal2Name", root);
	 result = GetListItem(1, result);
	if (result != NULL)
	{
		PrintRes(result->stringVal, P_STRING);
		if (strcmp(result->stringVal, "word2") == 0)
		{
			PrintSxs(6, true);
		}
		else
		{
			PrintSxs(6, false);
		}
	}
	PrintTestsEnd("Test6");
	return;

}





void Test5()
{
	PrintTestsStart("Test5");
	char* jsonString = "{\"testVal1\":  \"test\",\"testVal2\":{  \"subValName\" : 19.4 ,   \"subVal2Name\" :\n {  \"lowestObject\":\"testResult\"}   }} }";
	TreeNode* root = GetJsonTree(jsonString);

	if (root == NULL)
	{
		PrintSxs(5, false);
	}

	TreeNode* result = SearchTree("lowestObject", root);
	if (result != NULL)
	{
		PrintRes(result->stringVal, P_STRING);
		if (strcmp(result->stringVal, "testResult") == 0)
		{
			PrintSxs(5, true);
		}
		else
		{
			PrintSxs(5, false);
		}
	}
	PrintTestsEnd("Test5");
	return;

}
void Test4()
{
	PrintTestsStart("Test4");
	char* jsonString = "{\"testVal1\":  \"test\",\"testVal2\":{  \"subValName\" : 19.4 ,\"subVal2Name\" : true} }";
	TreeNode* root = GetJsonTree(jsonString);

	if (root == NULL)
	{
		PrintSxs(4, false);
	}

	TreeNode* result = SearchTree("subVal2Name", root);
	if (result != NULL)
	{
		//PrintRes(&result->boolVal, P_BOOL);
		if (result->boolVal == true)
		{
			PrintSxs(4, true);
		}
		else
		{
			PrintSxs(4, false);
		}
	}
	PrintTestsEnd("Test4");
	return;
}



void Test3()
{
	PrintTestsStart("Test3");
	char* jsonString = "{\"testVal1\":  \"test\",\"testVal2\":{  \"subValName\" : 19.4 ,\"subVal2Name\" : true} }";
	TreeNode* root = GetJsonTree(jsonString);

	if (root == NULL)
	{
		PrintSxs(3, false);
	}

	TreeNode* result = SearchTree("subVal2Name", root);
	if (result != NULL)
	{
		//PrintRes(&result->boolVal, P_BOOL);
		if (result->boolVal == true)
		{
			PrintSxs(3, true);
		}
		else
		{
			PrintSxs(3, false);
		}
	}
	PrintTestsEnd("Test3");
	return;
}


void Test2()
{
	PrintTestsStart("Test2");
	char* jsonString = "{\"testVal1\":  \"test\"  ,\"testVal2\":{\"subValName\": 19 }}";
	TreeNode* root = GetJsonTree(jsonString);

	if (root == NULL)
	{
		PrintSxs(2, false);
	}

	TreeNode* result = SearchTree("subValName", root);
	if (result != NULL)
	{
		PrintRes(&result->intVal, P_INT);
		if (result->intVal == 19)
		{
			PrintSxs(2, true);
		}
		else
		{
			PrintSxs(2, false);
		}
	}
	PrintTestsEnd("Test2");
	return;
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

	TreeNode* result = SearchTree("testVal2", root);
	if (result != NULL)
	{
		PrintRes(result->stringVal, P_STRING);
		if (strcmp(result->stringVal, "test2")==0)
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