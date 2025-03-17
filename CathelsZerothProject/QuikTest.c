#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
 

#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)

bool ParseFloat(char*, float*);
bool ParseInt(char*, int*);



int main()
{
	char test1[] = "-99900";
    char test2[] = "45621";
    char test3[] = "12.354";
    char test4[] = "-.465154";

    char tests[][4] = { test1,test2,test3,test4 };
	
    float floatVal;
    float* fvPtr = &floatVal;
    int intVal;
    int* intPtr = &intVal;
    
    for(int i = 0;i< 4; i++)
    if (ParseInt(test1, intPtr))
    {
        printf("result: %d\n", intVal);
    }
    else printf("Test 1 failed\n");
    

    
	return 0;
}



bool ParseNumber(char* input) //How do we handle erros here then?//I think we dont need this - just decide at parse time
{
    int intVal;
    float floatVal;
    char dot[] = ".";
    bool isSuccess;

    if (strstr(input, dot))
    {
        isSuccess = ParseFloat(input, &floatVal);
    }
    else
    {
        isSuccess = ParseInt(input,&intVal);
    }

    if()
}

/*This is essentially a decorator over the ParseDigitString method - converts the result to int*/
bool ParseInt(char*input, int* intValPtr)
{
    float floatVal;

    if(!ParseFloat(input, &floatVal))
        return false;
    *intValPtr = (int)floatVal;

    return true;;
}


/*This is the core number parsing logic - we always parse as a float, and if need an int, we cast it*/
bool ParseFloat(char* input, float* floatValPtr)
{
    float result = 0;
    int multiplier = 1;
    int reductionFactor=0;
    bool hasDecPoint = false;
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
        else if (input[i] == '.' && i != 0 && hasDecPoint==false)//need to check we dont have -.234 for example....
        {
            reductionFactor = (strlen(input) - 1) - i;
            hasDecPoint = true;
        }
        else return false;

    }
    for (int f = 0; f < reductionFactor; f++)
        result = result / 10;
    
    *floatValPtr = result;
    return true;
}

    