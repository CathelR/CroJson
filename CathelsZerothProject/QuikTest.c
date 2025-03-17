#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
 

#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)

bool ParseDigitString(char*, float*);
bool ParseFloat(char*, float*);



int main()
{
	char test1[] = "-99.9";
    char test2[] = "-0013.5";
    char test3[] = "-12.354";
    char test4[] = "-565.465154";
	
    float floatVal;
    float* fvPtr = &floatVal;
    if (ParseFloat(test1, fvPtr))
    {
        printf("result: %f\n", floatVal);
    }
    else printf("Test 1 failed");

    if (ParseFloat(test2, fvPtr))
    {
        printf("result: %f\n", floatVal);
    }
    else printf("Test 2 failed");

    if (ParseFloat(test3, fvPtr))
    {
        printf("result: %f\n", floatVal);
    }
    else printf("Test 3 failed");

    if (ParseFloat(test4, fvPtr))
    {
        printf("result: %0.10f\n", floatVal);
    }
    else printf("Test 4 failed");

	return 0;
}


/*
bool ParseNumber(char* content)
{
    int intVal;
    float floatVal;
    char dot[] = ".";
    bool isSuccess;

    if (strstr(content, dot))
    {
        floatVal = ParseFloat(content, &intVal);
    }
    else
    {
        intVal = ParseInt(content,&floatVal);
    }
}

bool ParseInt(char* content, int* intValPtr)
{
    //*intValPtr = ParseDigitString(content);
    return false;
}
*/


bool ParseFloat(char* input, float* floatValPtr)
{
    if (ParseDigitString(input, floatValPtr))
        return true;
    else return false;
}


/*This is the core number parsing logic - we always parse as a float, and if need an int, we cast it*/
bool ParseDigitString(char* input, float* floatValPtr)
{
    float result = 0;
    int multiplier = 1;
    int reductionFactor;
    for (int i = strlen(input) - 1; i >= 0; i--)
    {
        if (char_is_numeric(input[i]))
        {
            result = result + ((input[i] - 48) * multiplier);
            multiplier = multiplier * 10;
            //Kind of need a size limit on this - There is a maximum size of int 
        }
        else if (input[i] == '-' && i == 0)//I.e were at the first character
        {
            result = result * -1;
            //instead of doing this, just change the bit - does this need to happen after the fact?
        }
        else if (input[i] == '.' && i != 0)//need to check we dont have -.234 for example....
        {
            reductionFactor = (strlen(input) - 1) - i;
        }
        else return false;

        /*Wanted to do something clever like change the exponenent instead of */
    }
    for (int f = 0; f < reductionFactor; f++)
        result = result / 10;
    
    *floatValPtr = result;
    return true;
}

    