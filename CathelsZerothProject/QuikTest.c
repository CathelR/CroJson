#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
 

#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)

bool ParseFloat(char*, float*);
bool ParseInt(char*, int*);



int main()
{
	char test1[] = "test";

	
	if (strcmp(test1,"test")==0)
	{
		printf("Thatworked");
	}


    
	return 0;
}



    