#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
 

#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)

bool ParseFloat(char*, float*);
bool ParseInt(char*, int*);



int main()
{
	int num = 0;
	
	num += 2;
	num += 8;

	if (num == 2 & 8)
	{
		printf("That worked");
	}

    
	return 0;
}



    