#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "CroJson.h"
 

#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)
#define buffer_at_offset(buffer, offset) (buffer->cursor+offset<buffer->length)? *(buffer->jsonString+buffer->cursor+offset) : -1




int main()
{
	JsonBuffer testBuff2 = { " :",0,2,0 };
	JsonBuffer* bPtr = &testBuff2;
	if ((buffer_at_offset(bPtr, 1)) == ':')
	{
		printf("what the fuck");
	}
	return 0;
}



    