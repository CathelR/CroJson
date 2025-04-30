#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "CroJson.h"



/*Struct Definitions*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/



Error gl_error;




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

/*
//Top level - Interface to recursive methods
TreeNode* GetJsonTree(char* jsonString)
{
    bool isSuccess = false;
    //Fine to use a local instance of JsonBuffer here as we're not reutrning it, its cleanup gets handled when the method returns
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
*/


/*
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
    while ()
    {

    }


}
*/



TokenPool* TokenizeJson(JsonBuffer* bPtr)
{
    TokenPool* tokens = malloc(sizeof(TokenPool));
    if (tokens == NULL)
    {
        SetError("Failed to allocate memory for TokenPool", name_of(TokenizeJson), 0);
        return NULL;
    }
    tokens->tokenPool  = malloc(bPtr->length * sizeof(Token));
    if (tokens->tokenPool == NULL)
    {
        SetError("Failed to allocate memory for TokenPool", name_of(TokenizeJson), 0);
        return NULL;
    }
    tokens->stringPool.pool = malloc(2*bPtr->length * sizeof(char));
    if (tokens->stringPool.pool == NULL)
    {
        SetError("Failed to allocate memory for StringPool", name_of(TokenizeJson), 0);
        return NULL;
    }
    bool errorFound = false;
    int cursorPos = 0;
    short countQuote = 0;
    short countColon = 0;
    
    for (bPtr->cursor; bPtr->cursor < bPtr->length; bPtr->cursor++)
    {
        if (errorFound)
        {
            AddErrorContext(name_of(TokenizeJson));
            break;
        }
        if (char_is_whitespace(buffer_at_cursor(bPtr)))
        {
            SkipWhiteSpace(bPtr);
        }
        switch (buffer_at_cursor(bPtr))
        {
        case '{':
            if (IsCurlyOpenValid(countQuote, countColon, &bPtr->cursor) || bPtr->cursor == 0)
            {
                AddSyntaxToken("{\0", tokens);
            }
            else errorFound=true;
            break;
        case '}':
            if (IsCurlyCloseValid(countQuote, countColon, &bPtr->cursor))
            {
                AddSyntaxToken("}\0", tokens);
                countColon = 0;
                countQuote = 0;
            }
            else  errorFound = true;
            break;
        case '"':
            if (IsQuoteValid(countQuote, countColon, &bPtr->cursor))
            {
                char* tempContent = ReadContent(bPtr,true,tokens->stringPool.nextBlock); //Returns the pointer to the position in the pool
                if (tempContent != NULL)
                {
                    AddContentToken( tokens);
                    bPtr->cursor += strlen(tempContent); //Jump forward
                    countQuote++;
                }
            }
            else  errorFound = true;
            break;
        case ':':
            if (IsColonValid(countQuote, countColon, &bPtr->cursor))
            {
                AddSyntaxToken(":\0", tokens);
                countColon++;
            }
            else  errorFound = true;
            break;
        case ',':
            if (IsCommaValid(countQuote, countColon, &bPtr->cursor))
            {
                AddSyntaxToken(",\0", tokens);
                countColon = 0;
                countQuote = 0;
            }
            else  errorFound = true;
            break;
        case '[':
            if (IsSquareOpenValid(countQuote,countColon, &bPtr->cursor))
            {
                AddSyntaxToken("[\0", tokens);
            }
            else  errorFound = true;
            break;
          
        case ']':
            if (IsSquareCloseValid(countQuote,countColon, &bPtr->cursor))
            {
                AddSyntaxToken("]\0", tokens);
            }
            else  errorFound = true;
            break;
        default:
            if (IsGenericCharValid(countQuote, countColon, &bPtr->cursor))
            {
                char* tempContent = ReadContent(bPtr, false, tokens->stringPool.nextBlock);//Convert to use string popol
                if (tempContent != NULL) {
                    AddContentToken(tokens);
                    bPtr->cursor += strlen(tempContent); //Jump forward
                }
            }
            else  errorFound = true;
            continue;
        }
    }
    if (errorFound)
    {
        free(tokens->stringPool.pool);
        free(tokens->tokenPool);
        free(tokens);
        return NULL;
    }
    else
    {
        return tokens;
    }
}




static bool IsQuoteValid(short countQuote, short countColon, int* cursorPos)
{
    if (countColon == 0 && (countQuote == 0 || countQuote == 1)) return true;
    else if (countColon == 1 && countQuote == 3) return true;//dont need if reading content
    else if (countColon == 1 && countQuote == 2) return true;
    else
    {
        SetError("Character: '\"' Invalid at this position", name_of(IsQuoteValid),*cursorPos);
        return false;
    }
}

static bool IsCommaValid(short countQuote, short countColon, int* cursorPos)
{
    if (countColon == 1 && countQuote == 2) return true;
    else if (countColon == 1 && countQuote == 4) return true;
    else
    {
        SetError("Character: ',' Invalid at this position", name_of(IsCommaValid), *cursorPos);
        return false;
    }
}

static bool IsColonValid(short countQuote, short countColon, int* cursorPos)
{
    if (countColon == 0 && countQuote == 2) return true;
    else
    {
        SetError("Character: ':' Invalid at this position", name_of(IsColonValid), *cursorPos);
        return false;
    }
}

static bool IsCurlyOpenValid(short countQuote, short countColon, int* cursorPos)
{
    if (countColon == 1 && countQuote == 2) return true;
    else
    {
        SetError("Character: '{' Invalid at this position", name_of(IsCurlyOpenValid), *cursorPos);
        return false;
    }
}

static bool IsCurlyCloseValid(short countQuote, short countColon, int* cursorPos)
{
    if (countColon == 1 && countQuote == 2) return true;
    else if (countColon == 1 && countQuote == 4) return true;
    else
    {
        SetError("Character: '}' Invalid at this position", name_of(IsCurlyCloseValid), *cursorPos);
        return false;
    }
}

static bool IsSquareOpenValid(short countQuote, short countColon, int* cursorPos)
{
    if (countColon == 1 && countQuote == 2) return true;
    else
    {
        SetError("Character: '[' Invalid at this position", name_of(IsSquareOpenValid), *cursorPos);
        return false;
    }
}

static bool IsSquareCloseValid(short countQuote, short countColon, int* cursorPos)
{
    if (countColon == 1 && countQuote == 2) return true;
    else if (countColon == 1 && countQuote == 4) return true;
    else
    {
        SetError("Character: ']' Invalid at this position", name_of(IsSquareCloseValid), *cursorPos);
        return false;
    }
}

static bool IsGenericCharValid(short countQuote, short countColon, int* cursorPos)
{
    if (countColon == 1 && countQuote == 2) return true;
    else
    {
        SetError("Non Syntax Character invalid at this position", name_of(IsGenericCharValid), *cursorPos);
        return false;
    }
}

//While the current char is whitespace, skips the whitespace and ends on the first non whitespace char
void SkipWhiteSpace(JsonBuffer* bPtr)
{
    while (buffer_can_advance(bPtr))
    {
        char nextChar = buffer_at_offset(bPtr,1);
        if (char_is_whitespace(nextChar))
        {
            buffer_advance(bPtr);
        }
        else
        {
            break;
        }
    }
    if (buffer_can_advance(bPtr))
    {
        buffer_advance(bPtr);
    }
}


static bool AddSyntaxToken(char* tokenVal, TokenPool* tokens)
{
    int lengthToAdd = strlen(tokenVal) + 1;
    char* result = strcpy(tokens->stringPool.nextBlock, tokenVal);
    if (result == NULL)
    {
        SetError("Error copying string", name_of(AddSyntaxToken), 0);
    }
    
    (tokens->tokenPool + tokens->tokenCount)->content = tokens->stringPool.nextBlock; //This line is the issue
    printf("here");
    tokens->stringPool.nextBlock = tokens->stringPool.nextBlock + lengthToAdd;
    tokens->tokenCount += 1;
    
    return true;
}

//Designed to work with passing in the string, not if the string is already  there
static bool AddContentToken(TokenPool* tokens)
{
    //No, handier to have it where it was
    int lengthToAdd = strlen(tokens->stringPool.nextBlock) + 1;
    (tokens->tokenPool + tokens->tokenCount)->content = tokens->stringPool.nextBlock;
    tokens->stringPool.nextBlock = tokens->stringPool.nextBlock + lengthToAdd;
    tokens->tokenCount += 1;
    return true;
}

//Now returns position of next free content
char* ReadContent(JsonBuffer* bPtr, bool isString, char* stringBuff)//would be better to just pass in the location of the memory to store it in up front surely. 
{
    int index = 0;
    Byte byte;
    byte.flags = 0;
    void (*CheckChar)(JsonBuffer*, char*, int*, Byte*);
    //We have already established that we want whatever char the cursor was on - so we add
    AddCharToContent(buffer_at_cursor(bPtr), stringBuff, &index);

    if (isString && buffer_can_advance(bPtr))
    {
        CheckChar = &CheckCharString;
    }
    else
    {
        CheckChar = &CheckCharNonString;
    }

    while (buffer_can_advance(bPtr))
    {
        buffer_advance(bPtr); /*Need to advance before checking, otherwise we lose the last character in the string*/
        CheckChar(bPtr, stringBuff, &index, &byte);
        if (byte.flags & read_finished) break;
    }

    if (byte.flags & read_success)
    {
        *(stringBuff + index) = '\0';//Basically we cap it off - when we return we return the pointer to? This string, how do we get the pointer to the next string?
        return stringBuff;
    }
    else
    {
        SetError("Failed to read content", name_of(ReadContent), bPtr->cursor);
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
            AddCharToContent('\"', content, indexPtr);
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
//^ May have to change
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


void PrintError(Error* error)
{
    printf("\n!!!->Error: %s || Character: %d\n   ->Call Stack: %s\n", error->errorMessage, error->charPos, error->methodName);
}

Error* GetError()
{
    return &gl_error;
}

void SetError(char* errorMessage, char* methodName, int charPos)
{
    strcpy(gl_error.errorMessage, errorMessage);
    strcpy(gl_error.methodName, methodName);
    gl_error.charPos = charPos;

}

void AddErrorContext(char* methodName)
{
    strcat(gl_error.methodName, "<-");
    strcat(gl_error.methodName, methodName);
}