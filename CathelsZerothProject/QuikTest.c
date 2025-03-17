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
        floatVal = ParseFloat(content, &intVal);
    }
    else
    {
        intVal = ParseInt(content,&floatVal);
    }
}

bool ParseInt(char* content, int* intValPtr)
{
    *intValPtr = ParseDigitString(content);

}

bool ParseFloat(char* content, int* floatValPtr)
{
    if (ParseDigitString(content, floatValPtr))
        return true;
    else return false;
}


/*This is the core number parsing logic - we always parse as a float, and if need an int, we cast it*/
bool ParseDigitString(char* input, float* floatVal)
{
    float result = 0;
    int multiplier = 1;
    int decimalPos;
    for (int i = strlen(input)-1; i >= 0; i--)
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
            //instead of doing this, is it possible to just change the bit? Maybe thats all that will happen anyway
        }
        else if (input[i] == '.' && i != 0 )//need to check we dont have -.234 for example....
        {
            decimalPos = i;
        }
        else return false;

        //So change exponsnent based on decimal position.
    }
    *floatVal = result;
    return true;

