#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "CroJson.h"
 

#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)
#define buffer_at_offset(buffer, offset) ((buffer->cursor+1+offset)<buffer->length)?   *(buffer->jsonString+(buffer->cursor+offset)) : NULL




int main()
{
	char text[] = "THis is some text";
	JsonBuffer buff = { text,0,strlen(text) - 1,0 };
	JsonBuffer* bPtr = &buff;
	while (buffer_at_offset(bPtr, 0) != NULL)
	{
		printf(buffer_at_offset(bPtr, 0));
	}
	return 0;
}



    