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

void ParseIntTests();
void ParseFloatTests();
void ReadContentTests();
void ParseNonStringTests();
void ParseValueTests();



int main(int argc, char* argv[])
{
	//ParseFloatTests();
	//ParseIntTests();
	//ReadContentTests();
	//ParseNonStringTests();
	ParseValueTests();

	return 0;
}



void ParseValueTests()
{
	PrintTestsStart("ParseValue");
	TreeNode* node = malloc(sizeof(TreeNode));
	if (node == NULL)
	{
		printf("memory allocation failure");
		return;
	}

	char test1[] = ",\"test1\":true,";
	char test2[] = ",true,";
	char test3[] = ",\"test3\":999,";
	char test4[] = ",999,";

	JsonBuffer testBuff1 = { test1,0,strlen(test1),0 };
	if (ParseValue(&testBuff1, &node, true) != NULL)
	{
		if (node->boolVal == (bool)true && (strcmp(node->name, "test1")==0))
		{
			PrintSxs(1, true);
		}
		else
		{
			PrintSxs(1, false);
			PrintRes(&(node->boolVal), P_BOOL);
			printf("%s\n", node->name);
		}
	}
	else
	{
		PrintSxs(1, false);
	}
	//-----------------------------------------------
	JsonBuffer testBuff2 = { test2,0,strlen(test2),0 };
	if (ParseValue(&testBuff2, &node, false) != NULL)
	{
		if (node->boolVal == (bool)true)
		{
			PrintSxs(2, true);
		}
		else
		{
			PrintSxs(2, false);
			PrintRes(&(node->boolVal), P_BOOL);
		}
	}
	else
	{
		PrintSxs(2, false);
	}
	//-----------------------------------------------
	JsonBuffer testBuff3 = { test3,0,strlen(test3),0 };
	if (ParseValue(&testBuff3, &node, true) != NULL)
	{
		if (node->intVal == (int)999 && (strcmp(node->name, "test3")==0))
		{
			PrintSxs(3, true);
		}
		else
		{
			PrintSxs(3, false);
			PrintRes(&(node->intVal), P_INT);
			printf("%s\n", node->name);
		}
	}
	else
	{
		PrintSxs(3, false);
	}
	//-----------------------------------------------
	JsonBuffer testBuff4 = { test4,0,strlen(test4),0 };
	if (ParseValue(&testBuff4, &node, false) != NULL)
	{
		if (node->intVal == (int)999 )
		{
			PrintSxs(4, true);
		}
		else
		{
			PrintSxs(4, false);
			PrintRes(&(node->intVal), P_INT);
		}
	}
	else
	{
		PrintSxs(4, false);
	}
	
	
	PrintTestsEnd("ParseValue");


	return;
}




void ParseNonStringTests()
{
	PrintTestsStart("ParseNonString");
	TreeNode* node = malloc(sizeof(TreeNode));
	if (node == NULL)
	{
		printf("memory allocation failure");
		return;
	}
		

	char test1[] = ":true,";
	char test2[] = ":false ";
	char test3[] = ":999 ";
	char test4[] = ":99.99} ";
	char test5[] = ":,";
	char test6[] = ":32a321]";


	JsonBuffer testBuff1 = { test1,0,strlen(test1),0 };
	if (ParseNonString(&testBuff1, node))
	{
		if (node->boolVal == true)
		{
			PrintSxs(1, true);
		}
		else
		{
			PrintSxs(1, false);
		}
	}
	else
	{
		PrintSxs(1, false);
	}
	//------------------------------------------------
	JsonBuffer testBuff2 = { test2,0,strlen(test2),0 };
	if (ParseNonString(&testBuff2, node))
	{
		if (node->boolVal == false)
		{
			PrintSxs(2, true);
		}
		else
		{
			PrintSxs(2, false);
		}
	}
	else
	{
		PrintSxs(2, false);
	}
	//------------------------------------------------
	JsonBuffer testBuff3 = { test3,0,strlen(test3),0 };
	if (ParseNonString(&testBuff3, node))
	{
		if (node->intVal == 999)
		{
			PrintSxs(3, true);
		}
		else
		{
			PrintSxs(3, false);
			PrintRes(&(node->intVal), P_INT);
		}
	}
	else
	{
		PrintSxs(3, false);
	}
	//------------------------------------------------
	JsonBuffer testBuff4 = { test4,0,strlen(test4),0 };
	if (ParseNonString(&testBuff4, node))
	{
		if (node->floatVal == (float)99.99)
		{
			PrintSxs(4, true);
		}
		else
		{
			PrintSxs(4, false);
			PrintRes(&(node->floatVal), P_FLOAT);
		}
	}
	else
	{
		PrintSxs(4, false);
	}


	//Shouldnt work
	JsonBuffer testBuff5 = { test5,0,strlen(test5),0 };
	if (ParseNonString(&testBuff5, node))
	{	
		PrintSxs(5, false);
	}
	else
	{
		PrintSxs(5, true);
	}

	//------------------------------------------------
	JsonBuffer testBuff6 = { test6,0,strlen(test6),0 };
	if (ParseNonString(&testBuff6, node))
	{
		PrintSxs(6, false);
	}
	else
	{
		PrintSxs(6, true);
	}

	PrintTestsEnd("ParseNonString");
	return;
}













void ReadContentTests()
{
	PrintTestsStart("ReadContent");
	/*For these tests we don't test what happens when */
	JsonBuffer test1 = { ":\"test1\"excess",0,14,0 };
	JsonBuffer test2 = { ":\"tes\\\"t2\"",0,10,0 };
	JsonBuffer test3 = { ":\"tes\\bt2\"excess",0,16,0 };
	JsonBuffer test4 = { ":\"tes\\ft2\"excess",0,16,0 };
	JsonBuffer test5 = { ":\"tes\\nt2\"excess",0,16,0 };
	JsonBuffer test6 = { ":\"tes\\rt2\"excess",0,16,0 };
	JsonBuffer test7 = { ":\"tes\\tt2\"excess",0,16,0 };

	JsonBuffer test8 = { ":test8 excess",0,13,0 };
	JsonBuffer test9 = { ":test9,excess",0,13,0 };
	JsonBuffer test10 = { ":\"test10\" excess",0,15,0 };

	JsonBuffer test11 = { ":\"test6excess",0,13,0 };
	JsonBuffer test12 = { ":\"tes\\2\"excess",0,13,0 };

	char* result;
	//Should work
	result = ReadContent(&test1,true);
	if (result!=NULL)
	{
		if (strcmp(result,"test1")==0)
			PrintSxs(1, true);
		else {
			PrintSxs(1, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(1, false);
	}
	free(result);
	//
	result = ReadContent(&test2, true);
	if (result != NULL)
	{
		if (strcmp(result, "tes\"t2") == 0)
			PrintSxs(2, true);
		else {
			PrintSxs(2, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(2, false);
	}
	free(result);
	//
	result = ReadContent(&test3,true);
	if (result != NULL)
	{
		if (strcmp(result, "tes\bt2") == 0)
			PrintSxs(3, true);
		else {
			PrintSxs(3, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(3, false);
	}
	free(result);
	//
	result = ReadContent(&test4,true);
	if (result != NULL)
	{
		if (strcmp(result, "tes\ft2") == 0)
			PrintSxs(4, true);
		else {
			PrintSxs(4, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(4, false);
	}
	free(result);
	//
	result = ReadContent(&test5,true);
	if (result != NULL)
	{
		if (strcmp(result, "tes\nt2") == 0)
			PrintSxs(5, true);
		else {
			PrintSxs(5, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(5, false);
	}
	free(result);
	//
	result = ReadContent(&test6,true);
	if (result != NULL)
	{
		if (strcmp(result, "tes\rt2") == 0)
			PrintSxs(6, true);
		else {
			PrintSxs(6, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(6, false);
	}
	free(result);
	//
	result = ReadContent(&test7, true);
	if (result != NULL)
	{
		if (strcmp(result, "tes\tt2") == 0)
			PrintSxs(7, true);
		else {
			PrintSxs(7, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(7, false);
	}
	free(result);


	//Non String
	result = ReadContent(&test8,false);
	if (result != NULL)
	{
		if (strcmp(result, "test8") == 0)
			PrintSxs(8, true);
		else {
			PrintSxs(8, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(8, false);
	}
	free(result);
	//
	result = ReadContent(&test9, false);
	if (result != NULL)
	{
		if (strcmp(result, "test9") == 0)
			PrintSxs(9, true);
		else {
			PrintSxs(9, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(9, false);
	}
	free(result);
	//
	result = ReadContent(&test10, false);
	if (result != NULL)
	{
		if (strcmp(result, "\"test10\"") == 0)
			PrintSxs(10, true);
		else {
			PrintSxs(10, false);
			PrintRes(result, P_STRING);
		}
	}
	else
	{
		PrintSxs(10, false);
	}
	free(result);

	//Should Not Work
	result = ReadContent(&test11, true);
	if (result == NULL)
	{
		PrintSxs(11, true);
	}
	else
	{
		PrintSxs(11, false);
	}

	//Should Not Work
	result = ReadContent(&test12, true);
	if (result == NULL)
	{
		PrintSxs(12, true);
	}
	else
	{
		PrintSxs(12, false);
	}


	PrintTestsEnd("ReadContent");
	return;
}

void ParseFloatTests()
{
	PrintTestsStart("ParseFloat");
	char* test1 = "12.340";
	char* test2 = "-0.98";
	char* test3 = "-123.00";
	char* test4 = "-.234234";
	char* test5 = "-234234";
	char* test6 = "-0.0";
	char* test7 = "a8787";

	float result = 0;
	//Should work
	if (ParseFloat(test1, &result) == true)
	{
		if (result == (float)12.340000)
			PrintSxs(1, true);
		else {
			PrintSxs(1, false);
			PrintRes(&result, P_FLOAT);
		}
	}
	if (ParseFloat(test2, &result) == true)
	{
		if (result == (float)-0.980000)
			PrintSxs(2, true);
		else {
			PrintSxs(2, false);
			PrintRes(&result, P_FLOAT);
		}
	}
	if (ParseFloat(test3, &result) == true)
	{
		if (result == (float) -123)
			PrintSxs(3, true);
		else {
			PrintSxs(3, false);
			PrintRes(&result, P_FLOAT);
		}
	}
	if (ParseFloat(test4, &result) == true)
	{
		if (result == (float)-0.234234)
			PrintSxs(4, true);
		else {
			PrintSxs(4, false);
			PrintRes(&result, P_FLOAT);
		}
	}
	if (ParseFloat(test5, &result) == true)
	{
		if (result == (float)-234234)
			PrintSxs(5, true);
		else {
			PrintSxs(5, false);
			PrintRes(&result, P_FLOAT);
		}
	}
	if (ParseFloat(test6, &result) == true)
	{
		if (result == (float)0)
			PrintSxs(6, true);
		else {
			PrintSxs(6, false);
			PrintRes(&result, P_FLOAT);
		}
	}

	//Should Not Work
	if (ParseFloat(test7, &result) != true)
	{
		PrintSxs(7, true);
	}
	else
	{
		PrintSxs(7, false);
	}
	PrintTestsEnd("ParseFloat");
	return;
}





void ParseIntTests()
{

	PrintTestsStart("ParseInt");
	char* test1 = "1234";
	char* test2 = "098";
	char* test3 = "-12355";
	char* test4 = "-234j234";
	int result = 0;
	//Should Work
	if (ParseInt(test1, &result) == true)
	{
		if (result == 1234)
			PrintSxs(1, true);
		else PrintSxs(1, false);
	}
	if (ParseInt(test2, &result) == true)
	{
		if (result == 98)
			PrintSxs(2, true);
		else PrintSxs(2, false);
	}
	if (ParseInt(test3, &result) == true)
	{
		if (result == -12355)
			PrintSxs(3, true);
		else PrintSxs(3, false);
	}
	//Should Not Work
	if (ParseInt(test4, &result) != true)
	{
		PrintSxs(4, true);
	}
	else
	{
		PrintSxs(4, false);
	}

	PrintTestsEnd("ParseInt");
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
	printf("Start of %s Tests\n-------------------------------------------------------------------------\n",testName);
	return;
}

void PrintTestsEnd(char* testName)
{
	printf("\n->End of %s Tests\n-------------------------------------------------------------------------\n\n\n",testName);
	return;
}