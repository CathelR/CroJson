#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
 

#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)

int main()
{
	char test1[] = "999";
    char test2[] = "00135";
    char test3[] = "-12354";
    char test4[] = "565.465154";
	
    
	return 0;
}


bool ParseNumber(char* content)
{
    int intVal;
    float floatVal;
    char dot[] = ".";
    bool isSuccess;

    if (strstr(content, dot))
    {
        floatVal = ParseFloat(&intVal);
    }
    else
    {
        intVal = ParseInt();
    }
}

bool ParseInt(char* content, int* intValPtr)
{
    *intValPtr = ParseDigitString(content);

}

bool ParseFloat(char* content, int* intValPtr)
{
    *intValPtr = ParseDigitString(content);

}


//Needs to deal with dots. COuld use this as s component of a larger method...
int ParseDigitString(char* input)
{
    int result = 0;
    int multiplier = 1;
    for (int i = strlen(input)-1; i >= 0; i--)
    {
        if (char_is_numeric(input[i]))
        {
            result = result + ((input[i] - 48) * multiplier);
            multiplier = multiplier * 10;
            //Kind of need a size limit on this - not kind of - I do
        }
        else if (input[i] == '-' && i == 0)//I.e were at the first character
        {
            result = result * -1;
        }
        else if (input[i] == '.' && i != 0)
        {
            //Everything we have up till now should now be less than 1.
            //In order to add that as a decimal we need the first digit to be les sthan one, wihc means dividing the whole thing by x 
            //e.g if we have 123, that  needs divided by 
        }
        else return -666; //what? Do we do here - need a way to signal that there was a mistake
    }

    //What would be nice is if we had a way of just sending back anything an convertig to int if need be... like if we worked that out somehow...
    return result;
}

