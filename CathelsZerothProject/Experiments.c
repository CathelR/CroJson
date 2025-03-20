#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
 

#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)

bool ParseFloat(char*, float*);
bool ParseInt(char*, int*);



int main()
{
	char test1[] = "test\btest\n";

	char test2[] = "test\\test\n";

	char test3[] = "test\rest\n";

	printf("\ntest1:\n");
	printf("%s",test1);
	printf("\ntest2:\n");
	printf("%s", test2);
	printf("\ntest3:\n");
	printf("%s", test3);

	



    
	return 0;
}



    