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
    char* content;
} Token;

typedef struct StringPool
{
    char* pool;
    char* nextBlock;
    int size;
} StringPool;

typedef struct TokenPool
{
    Token* tokenPool;
    int tokenNextIdx;

    StringPool stringPool;
}TokenPool;


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


//Top level - Interface to recursive methods
TreeNode* GetJsonTree(char* jsonString)
{
    bool isSuccess = false;
    /*Fine to use a local instance of JsonBuffer here as we're not reutrning it, its cleanup gets handled when the method returns*/
    JsonBuffer buffer = { jsonString, 0,strlen(jsonString),0 };
    JsonBuffer* bPtr = &buffer;

    Token* pool = TokenizeJson(bPtr);

    //Everything else works on the token chain
    TreeNode* root = malloc(sizeof(TreeNode));
    if (root == NULL) return NULL;
    root->child = NULL;
    root->next = NULL;
    root->name = malloc(5 * sizeof(char));
    strcpy(root->name, "root");
    SkipWhiteSpace(bPtr, false);
    if ((buffer_at_cursor(bPtr)) == '{')
    {
        isSuccess = ParseObject(bPtr, root);
    }
    if (isSuccess) {
        return root;
    }
    else {
        PrintError();
        return NULL;
    }
}

bool AddToken(char* tokenVal, TokenPool* tokens)
{
    int lengthToAdd = strlen(tokenVal)+1;
    strcpy(*tokens->stringPool.nextBlock, tokenVal);
    *(tokens->tokenPool + tokens->tokenNextIdx)->content = tokens->stringPool.nextBlock;
    tokens->stringPool.nextBlock = tokens->stringPool.nextBlock + lengthToAdd;
    tokens->tokenNextIdx += 1;
    return true;
}

bool TokenizeJson(JsonBuffer* bPtr)
{
    TokenPool tokens;
    tokens.tokenPool  = malloc(bPtr->length * sizeof(Token));
    tokens.stringPool.pool = malloc(2*bPtr->length * sizeof(char));

    int cursorPos = 0;
    short countQuote = 0;
    short countColon = 0;

    for (int i = 0; i < bPtr->length; i++)
    {
        switch (bPtr->jsonString[i])
        {
        case '{':
            if (IsCurlyOpenValid(countQuote, countColon) || i == 0)
            {
                AddToken("{\0", &tokens);
            }
            else return false;
            break;
        case '}':
            if (IsCurlyCloseValid(countQuote, countColon))
            {
                AddToken("}\0", &tokens);
                countColon = 0;
                countQuote = 0;
            }
            else return false;
        case '"':
            if (IsQuoteValid(countQuote, countColon))
            {
                char* tempContent = ReadContent(); //Decide on how to handle. If we return a char* then we pass that to add token,
                //Need to also advance the cursor here - jump forward by string length -easy..
                if (tempContent != NULL)
                {
                    AddToken(tempContent, &tokens);
                    countQuote++;
                }
                free(tempContent);
            }
            else return false;

            break;
        case ':':
            if (IsColonValid(countQuote, countColon))
            {
                AddToken(":\0", &tokens);
                countColon++;
            }

            break;
        case ',':
            if (IsCommaValid(countQuote, countColon))
            {
                AddToken(", \0", &tokens);
                countColon = 0;
                countQuote = 0;
            }

            break;
        case '[':

            break;
          
        case ']':

            break;
        default:

            continue;
        }
    }
}
//Check valid could be something like a tree for each character.

bool IsQuoteValid(short countQuote, short countColon)
{
    if (countColon == 0 && (countQuote == 0 || countQuote == 1)) return true;
    else if (countColon == 1 && countQuote == 3) return true;
    else if (countColon == 1 && countQuote == 2) return true;
    else return false;
}

bool IsCommaValid(short countQuote, short countColon)
{
    if (countColon == 1 && countQuote == 2) return true;
    else if (countColon == 1 && countQuote == 4) return true;
    else return false;
}

bool IsColonValid(short countQuote, short countColon)
{
    if (countColon == 0 && countQuote == 2) return true;
    else return false;
}

bool IsCurlyOpenValid(short countQuote, short countColon)
{
    if (countColon == 1 && countQuote == 2) return true;
    else return false;
}

bool IsCurlyCloseValid(short countQuote, short countColon)
{
    if (countColon == 1 && countQuote == 2) return true;
    else if (countColon == 1 && countQuote == 4) return true;
    else return false;
}


void ParseObject(char* jsonString)
{
    Token* pool = InitializePool(strlen(jsonString));
    TokenizeObject(pool, jsonString);
    if (pool == NULL)
    {

    }
    //If theres sub objects handle that here
    // 
    //So here we're dealing with a string if tokens
    while (/*Theres still tokens*/)
    {

    }
}







//Now returns position of next free content
int ReadContent(JsonBuffer* bPtr, bool isString, char* contentSpace)
{
    int index = 0;
    Byte byte;
    byte.flags = 0;
    void (*CheckChar)(JsonBuffer*, char*, int*, Byte*);

    if (isString && buffer_can_advance(bPtr))
    {
        CheckChar = &CheckCharString;
        if ((buffer_at_offset(bPtr, 1)) != ('\"'))
        {
            SetError("Syntax Error, missing open quote", name_of(ReadContent), bPtr->cursor);
            free(string);
            return NULL;
        }
        else
        {
            buffer_advance(bPtr);
        }
    }
    else
    {
        CheckChar = &CheckCharNonString;
    }
    while (buffer_can_advance(bPtr))
    {
        buffer_advance(bPtr); /*Need to advance before checking, otherwise we lose the last character in the string*/
        CheckChar(bPtr, string, &index, &byte);
        if (byte.flags & read_finished) break;
    }

    if (byte.flags & read_success)
    {
        *(string + index) = '\0';
        char* holder = string;
        string = realloc(string, (index + 1) * sizeof(char));
        //printf("%s\n", string);
        if (string == NULL)
        {
            SetError("Memory Allocation failure", name_of(ReadContent), bPtr->cursor);
            free(holder);
            return NULL;
        }
        else return string;
    }
    else
    {
        SetError("Failed to read content", name_of(ReadContent), bPtr->cursor);
        free(string);
        return NULL;
    }
}


/*This is one possible strategy for reading content*/
/*Not super happy with the nested switch case here - could have been done neater...*/
void CheckCharString(JsonBuffer* bPtr, char* content, int* indexPtr, Byte* byte)
{
    char currChar = buffer_at_cursor(bPtr);
    switch (currChar)
    {
    case'\"':
        if (buffer_can_advance(bPtr))
        {
            //printf("bef: %c\n", buffer_at_cursor(bPtr));
            buffer_advance(bPtr);
            //printf("aft: %c\n", buffer_at_cursor(bPtr));
        }
        byte->flags = byte->flags | read_finished;
        byte->flags = byte->flags | read_success;
        break;
    case '\\':
        if (buffer_can_advance(bPtr))
        {
            buffer_advance(bPtr);
            currChar = buffer_at_cursor(bPtr);
            switch (currChar)
            {
            case '\"':
                AddCharToContent('\"', content, indexPtr);
                break;
            case '\\':
                AddCharToContent('\\', content, indexPtr);
                break;
            case 'b':
                AddCharToContent('\b', content, indexPtr);
                break;
            case 'f':
                AddCharToContent('\f', content, indexPtr);
                break;
            case 'n':
                AddCharToContent('\n', content, indexPtr);
                break;
            case 'r':
                AddCharToContent('\r', content, indexPtr);
                break;
            case 't':
                AddCharToContent('\t', content, indexPtr);
                break;
            default:
                SetError("Syntax Error, invalid escape sequence", name_of(CheckCharString), bPtr->cursor);
                byte->flags = byte->flags | read_finished;
                break;
            }
        }
        break;
    default:
        AddCharToContent(currChar, content, indexPtr);
        break;
    }
    return;
}



/*We don't worry about whether the char is valid here, we're just looking to read content*/
void CheckCharNonString(JsonBuffer* bPtr, char* content, int* indexPtr, Byte* byte)
{
    char currChar = buffer_at_cursor(bPtr);
    if (currChar == ' ' || currChar == ',' || currChar == '}' || currChar == ']')
    {
        byte->flags = byte->flags | read_finished;
        byte->flags = byte->flags | read_success;
    }
    else
    {
        AddCharToContent(currChar, content, indexPtr);
    }
    return;
}



void AddCharToContent(char currChar, char* string, int* indexPtr)
{
    *(string + *indexPtr) = currChar;
    *indexPtr += 1;
    return;
}


void PrintError()
{
    printf("\n!!!->Error: %s || Character: %d\n   ->Call Stack: %s\n", gl_error.errorMessage, gl_error.charPos, gl_error.methodName);
}


void SetError(char* errorMessage, char* methodName, int charPos)
{
    strcpy(gl_error.errorMessage, errorMessage);
    strcpy(gl_error.methodName, methodName);
    gl_error.charPos = charPos;

}

void AddErrorCallStack(char* methodName)
{
    strcat(gl_error.methodName, "<-");
    strcat(gl_error.methodName, methodName);
}