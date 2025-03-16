#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Models.h"
//#include "Utils.h"
#include "FileFuncs.h"

//Would be good if it was case in-sensitive

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
	
	/*
	for (int i = 0; i < strlen(jsonString); i++)
	{
		if(jsonString[i]=='"' || jsonString[i] == ':' || jsonString[i] == ',' || jsonString[i] == '{' || jsonString[i] == '}' || jsonString[i] == '[' || jsonString[i] == ']')
		{
			printf("%c", jsonString[i]);
		}
	}
	*/

	//So, 
	//Need to run through it
	//Need to take different actions on finding certain characters
	//IT does matter what precedes the special characters, for instance a  '"' must precede a :
	//Stacks are very useful for knowing what came before
	//So it would be something like: If find something interesting, chekc the thing befor eis what we expected
	//So don't register everything on the stack, only "interesting" stuff
	// " { } [ ] , :
	//How will I implement the stack? Put it on the heap ironically, but assign extra mem in batches
	//strcat() -- Define a push method (no need to pop) -- could us epointers, cant return new char array as it will be a diff size - static
	//Two stages: If in one category (interesting) put it on stack. If in second categroy, check stack and take action
	//{"":"","":,"":{"":"","":,"":""},"":[{X},{X},{X},{X}],"":""}
	//For the X object in the above stack, would want to be checking that everything in th elist has the same format
	//Now sure how I'd do naming convention...
		//00> Then want to have buckets available for the actual values



	//Can you do an if typeof() type thing? And have different strategies for each?


	free(jsonString);
	jsonString = NULL;
	
	
	
	//Need to call out format errors, commas in wrong place etc
	//Perhaps keep a stack record, that way can compare , i.e if current top of stack is... then...
	
	return 0;
}

//For ASCII image generation:
//Can we use a mini version of the JPEG compression algorithm to get the signiature of each character?
//This would actually involve generating bitmaps in memory - fine.
//Binary search to find th ematch? i.e cloer to this or that?
//Great opportunity to implement a binary search tree and learn about them
//Use a hash table for brightness mode
//Recusrsion for the tree and Timmmberrr()
//There was others
//Was also thinking it would be really funky to do a real timeweb cam thing, but to actually use characters.
//Also could have it soe that when it goes  dark,  i,.e cover webacm it shuts off
//That could develop into a realtime image processing engine, i.e edge detection etc

