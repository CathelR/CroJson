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

	char* testString = malloc(10 * sizeof(char));
	for (int i = 0; i < 9; i++)
	{
		*(testString + i) = 'a';

	}
	*(testString + 9) = '\0';

	printf("%c", testString+4);



    
	return 0;
}



    