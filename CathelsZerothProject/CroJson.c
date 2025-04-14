#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "CroJson.h"



/*Struct Definitions*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/


typedef struct Error
{
    char errorMessage[1024];
    char methodName[64];
    int charPos;
}Error;
static Error gl_error;

typedef struct Token
{
    int index;
    char* value;
} Token;

typedef struct MemPool
{
    void* pool;
};
/*==================================================================================================================================================*/

/*Macro definition*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
#define buffer_can_advance(buffer) (buffer->cursor+1<buffer->length)  
#define buffer_at_cursor(buffer) *(buffer->jsonString+buffer->cursor)
#define buffer_at_offset(buffer, offset) (buffer->cursor+offset<buffer->length)? *(buffer->jsonString+buffer->cursor+offset) : -1
#define buffer_advance(buffer) (buffer->cursor++) 
#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)  
#define char_is_whitespace(inChar)  (inChar <= 32 && inChar>=0)
#define name_of(object) #object

#define read_finished (1<<0)
#define read_success (1<<1)



void TokenizeObject(char* jsonString, int cursor)
{
    int length = strlen(jsonString);
    for (int i = 0; i < length; i++)
    {
        switch (jsonString[i])
        {
        case '{':
            //Check Valid
            // //ParseObject
            //add token
            break;
        case '}':
            //Check Valid
            //add token
            break;
        case '"':
            //readcontent
            //Add content as token
            break;
        case ':':
            //Do read content
            //add token
            break;
        case ',':
            //addtoken
            break;
        case '[':
            //add token
            break;
        case ']':
            //addtoken
            break;
        default:
            //readcontent
            continue;
        }
    }
}
//Check valid could be something like a tree for each character.

void CheckValid()
{
    
}

