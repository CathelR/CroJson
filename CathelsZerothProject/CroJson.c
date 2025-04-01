#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "CroJson.h"

/*
NOTE ON THIS WORK:
This library is based on the popular C Json parsing library "cJson".
I'm writing this purely for my own learning so it is by no means fully featured, and does not (and probably will not) meet all the requirements of the JSON standard.
*/

/*Struct Definitions*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/


typedef struct Error
{
    char errorMessage[1024];
    char methodName[64];
    int charPos;
}Error;
static Error gl_error;



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
/*==================================================================================================================================================*/


/*Json Methods*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/


/*
TO DO :
---------------------------------
- Make better use of error printing to user
- Add support for scientific numbers
- Add support for doubles
- Add limit for int size to prevent parse error

/*Notes on freeing in case of error:
-->In case of error we simply return. We call the FreeTree() method which will clear the entire tree...
*/


/*Returns the node, and the calling method gets to select what it wants*/
TreeNode* SearchTree(char* targetName, TreeNode* node)
{       
    printf("node name: %s\n", node->name);
    if (strcmp(node->name, targetName) == 0)
    {
        return node;
    }
    else
    {
        if (node->child != NULL)
        {
            return SearchTree(targetName, node->child);
        }
        if (node->next != NULL)
        {
            return SearchTree(targetName, node->next);
        }
    }
    return NULL;
}


//Top level - Interface to recursive methods
TreeNode* GetJsonTree(char* jsonString)
{   
    bool isSuccess=false;
    /*Fine to use a local instance of JsonBuffer here as we're not reutrning it, its cleanup gets handled when the method returns*/
    JsonBuffer buffer = { jsonString, 0,strlen(jsonString),0 };
    JsonBuffer* bPtr = &buffer;
    TreeNode* root = malloc(sizeof(TreeNode));
    if (root == NULL) return NULL;
   
    root->name = malloc(5*sizeof(char));
    strcpy(root->name, "root");
    SkipWhiteSpace(bPtr,false);
    if ((buffer_at_cursor(bPtr)) == '{')
    {        
        isSuccess = ParseObject(bPtr, root);
    }
    if (isSuccess) {
        return root;
    }
    else {
        PrintError();
        return NULL;//Don't free rootNode obviously because we actually wan it
    }
}

/*gets the component values of an object, and attatches them to the root "object" node (relRoot)*/
bool ParseObject(JsonBuffer* bPtr, TreeNode* relRoot)
{
    relRoot->nodeType = OBJECT;

    //In the first instance I pass in a pointer to the root objects "child" pointer
    TreeNode** nextNodePtr = &(relRoot->child); 
    do
    {
        //TODO: Read name should come here - take out of parse value - maybe just attatch the node here and pass it in???
        //Each subsequent call uses a pointer to the previous values "next" pointer
        nextNodePtr = ParseValue(bPtr, nextNodePtr, true);
        if (nextNodePtr == NULL) {
            AddErrorCallStack(name_of(ParseObject));
            return false;
        }
        SkipWhiteSpace(bPtr,true);
    } while ((buffer_at_cursor(bPtr)) == ','); //Because after parsing each value we should have a comma

    SkipWhiteSpace(bPtr,true);
    if ((buffer_at_cursor(bPtr)) != '}')
    {
        SetError("Syntax Error, missing curly close", name_of(ParseObject), bPtr->cursor);
        return false;
    }
    else return true;
}


/*->A value in this context is anything that has a name (as well as list items)
Returns a pointer to the created nodes "next" pointer - basically the socket at which we attatch the next value*/
TreeNode** ParseValue(JsonBuffer* bPtr, TreeNode** socket, bool shouldReadName)
{
    *socket = malloc(sizeof(TreeNode));
    if (*socket == NULL)
    {
        SetError("Memory Allocation failure", name_of(ParseValue), bPtr->cursor);
        return NULL;
    }

    if (shouldReadName)
    {
        if (!ReadValueName(bPtr, *socket))
        {
            AddErrorCallStack(name_of(ParseValue));
            return NULL;
        }
        //printf("buffer_at_cursor: %c\n", buffer_at_cursor(bPtr));
        if (char_is_whitespace(buffer_at_cursor(bPtr)))
        {
            SkipWhiteSpace(bPtr,true);
        }

        if ((buffer_at_cursor(bPtr)) != ':') {
            //printf("buffer_at_cursor: %c\n", buffer_at_cursor(bPtr));
            SetError("Syntax Error, missing colon", name_of(ParseValue), bPtr->cursor);
            return NULL;
        }
        else
        {
            //buffer_advance(bPtr);
            //printf("buff at cursor: %c\n", buffer_at_cursor(bPtr));
        }
    }
    SkipWhiteSpace(bPtr, false);
    //printf("at_cursor: %c\n", buffer_at_cursor(bPtr));

    char currChar = buffer_at_offset(bPtr,1);
    switch (currChar)
    {
    case '{':
        buffer_advance(bPtr);
        if (!ParseObject(bPtr, *socket))
        {
            AddErrorCallStack(name_of(ParseValue));
            return NULL;
        }
        break;
    case '[':
        if (!ParseList(bPtr, *socket))
        {
            AddErrorCallStack(name_of(ParseValue));
            return NULL;
        }
        break;
    case '"':
        if (!ParseString(bPtr, *socket))
        {
            AddErrorCallStack(name_of(ParseValue));
            return NULL;
        }
        break;
    default:
        if (!ParseNonString(bPtr, *socket))
        {
            AddErrorCallStack(name_of(ParseValue));
            return NULL;
        }
    }
   
    
    return  &((*socket)->next);
}


/*Arrays are implemented as linked lists - the node pointer of each node points to the next item in the list*/
bool ParseList(JsonBuffer* bPtr, TreeNode* relRoot) //Where relRoot is the object created when we called parseValue
{
    relRoot->nodeType = LIST;
    //In the first instance I pass in a pointer to the roots objects "child" pointer
    TreeNode** nextNodePtr = &(relRoot->child);
    do
    {
        //Each subsequent call uses a pointer to the previous values "next" pointer
        nextNodePtr = ParseValue(bPtr, nextNodePtr, false);
        if (nextNodePtr == NULL) {
            return false;
        }
        SkipWhiteSpace(bPtr,true);
    } while (buffer_at_cursor(bPtr) == ','); //Because after parsing each value we should have a comma

    SkipWhiteSpace(bPtr,true);
    if (!buffer_at_cursor(bPtr) == ']')
    {
        SetError("Syntax Error, missing square close", name_of(ParseList), bPtr->cursor);
        return false;
    }
    else return true;
}


/*This is basically a work allocation method, decides which strategy to use to parse the value*/
bool ParseNonString(JsonBuffer* bPtr, TreeNode* valueNode)
{
    bool isSuccess = false;
    char* content = ReadContent(bPtr, false);
    if (content == NULL)
    {
        AddErrorCallStack(name_of(ParseNonString));
        return false;
    }
    //printf("%s\n", content);
    if (strcmp(content,"true")==0)
    {
        valueNode->boolVal = true;
        valueNode->nodeType = BOOL;
        isSuccess = true;
    }
    else if (strcmp(content, "false") == 0)
    {
        valueNode->boolVal = false;
        valueNode->nodeType = BOOL;
        isSuccess = true;
    }
    else if (char_is_numeric(content[strlen(content) - 1])) //Check the last char in the string, which will always be numeric for a number
    {
        if (strstr(content, "."))
        {
            valueNode->nodeType = FLOAT;
            isSuccess = ParseFloat(content, &(valueNode->floatVal));
        }
        else
        {
            valueNode->nodeType = INT;
            isSuccess = ParseInt(content, &(valueNode->intVal));
        }
    }
    else
    {
        AddErrorCallStack(name_of(ParseNonString));
        isSuccess = false;
    }
    /*Content is a temporary string read from the JSON - once we parse it we don't need the string anymore*/
    free(content);
    return isSuccess;
}

/*This is essentially a decorator over the ReadString() method. It handles the actual assignment - allows us to read string elsewhere*/
bool ParseString(JsonBuffer* bPtr, TreeNode* valueNode)
{
    char* string = ReadContent(bPtr,true);
    if (string != NULL)
    {
        valueNode->stringVal = string;
        valueNode->nodeType = STRING;
        return true;
    }
    else
    {
        AddErrorCallStack(name_of(ParseString));
        return false;
    }
}


/*This is essentially a decorator over the ParseFloat() method - converts the result to int*/
bool ParseInt(char* input, int* intValPtr)
{
    float floatVal;

    if (!ParseFloat(input, &floatVal))
    {
        AddErrorCallStack(name_of(ParseInt));
        return false;
    }
        
    *intValPtr = (int)floatVal;

    return true;;
}


/*This is the core number parsing logic - we always parse as a float, and if need an int, we cast it*/
bool ParseFloat(char* input, float* floatValPtr)
{
    float result = 0;
    int multiplier = 1;
    int reductionFactor = 0;
    bool hasDecPoint = false;
    for (int i = strlen(input) - 1; i >= 0; i--)
    {
        if (char_is_numeric(input[i]))
        {
            result = result + ((input[i] - 48) * multiplier);
            multiplier = multiplier * 10;
            //Kind of need a size limit on this - There is a maximum size of int 
        }
        else if (input[i] == '-' && i == 0)
        {
            result = result * -1;
        }
        else if (input[i] == '.' && hasDecPoint == false)
        {
            reductionFactor = (strlen(input) - 1) - i;
            hasDecPoint = true;
        }
        else
        {
            SetError("Syntax Error, Invalid character in content", name_of(ParseFloat), -1);
            return false;
        }

    }
    for (int f = 0; f < reductionFactor; f++)
        result = result / 10;

    *floatValPtr = result;
    return true;
}

//"":, - for a non string, we immediately return - fine, for a non string...?
/*Method to read text - works for strings and non strings depending on checking method passed in*/
char* ReadContent(JsonBuffer* bPtr, bool isString)
{
    char* string = malloc(bPtr->length - bPtr->cursor);
    if (string == NULL)
    {
        SetError("Memory Allocation failure", name_of(ReadContent), bPtr->cursor);
        return NULL;
    }
    int index = 0;
    Byte byte;
    byte.flags = 0;
    void (*CheckChar)(JsonBuffer*, char*, int*, Byte*);
    
    if (isString && buffer_can_advance(bPtr))
    {
        CheckChar = &CheckCharString;
        if ((buffer_at_offset(bPtr,1)) != ('\"'))
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
        string = realloc(string,(index+1) * sizeof(char));
        printf("%s\n", string);
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


/*Reads and assigns name to a node - if its an object*/
bool ReadValueName(JsonBuffer* bPtr, TreeNode* nodeToName)
{
    bool isSuccess = false;
    SkipWhiteSpace(bPtr,false);
    if (buffer_can_advance(bPtr))
    {
        if ((buffer_at_offset(bPtr, 1)) != ('\"'))
        {
            //printf("buffatoffset: %c\n", buffer_at_offset(bPtr, 1));
            SetError("Syntax Error, missing open quote", name_of(ReadValueName), bPtr->cursor);
            return false;
        }
    }
    nodeToName->name = ReadContent(bPtr, true);
    if (nodeToName->name != NULL)
    {
        isSuccess = true;
    }
    else
    {
        AddErrorCallStack(name_of(ReadValueName));
    }

    return isSuccess;
}

/*Frees a node - uses recursive calls to free children/next nodes first*/
void FreeNode(TreeNode* node)
{
    if (node != NULL)
    {
        if (node->child != NULL)
        {
            FreeNode(node->child);
        }
        if (node->next != NULL)
        {
            FreeNode(node->next);
        }

        free(node->name);
        free(node->stringVal);
        free(node);
    }
    return;
}


void SkipWhiteSpace(JsonBuffer* bPtr, bool doAdvanceRead)
{
    while (buffer_can_advance(bPtr))
    {
        //char currChar = buffer_at_cursor(bPtr);
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
    if (doAdvanceRead)
    {
        if (char_is_whitespace((buffer_at_cursor(bPtr))) && (buffer_can_advance(bPtr)))
        {
            buffer_advance(bPtr);
        }
    }
    
}

void PrintError()
{
    printf("\n!!!->Error: %s || Character: %d\n   ->Call Stack: %s\n", gl_error.errorMessage , gl_error.charPos,gl_error.methodName);
}


void SetError(char* errorMessage,char* methodName ,int charPos)
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





















/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

//Old validation code approach - Will come back to this
/*

//Operates at one level of a JSON object. 
int IsJsonObjectValid(char* jsonString, int cursorPos)
{
    cursorPos += 1;
    short countQuote = 0;
    short countColon = 0;

    for (int i = cursorPos; i < strlen(jsonString); i++)
    {
        switch (jsonString[i])
        {
        case '{':
            if (IsCurlyOpValid(countQuote, countColon) || cursorPos == 1)
            {
                //printf("\n<NewLevel>\n");
                cursorPos = IsJsonObjectValid(jsonString, i);
                if (cursorPos != -1)
                {
                    i = cursorPos;
                }
                else
                {
                    return -1;
                }
            }
            else return -1;
            break;
        case '}':
            if (IsCurlyClValid(countQuote, countColon))
            {
                //printf("\n<EndLevel>");
                return i;
            }
            else return -1;
            break;
        case '"':
            //printf("%c", '"');
            if (IsQuoteValid(countQuote, countColon))
            {
                countQuote++;
            }
            else return -1;
            break;
        case ':':
            //printf("%c", ':');
            if (IsColonValid(countQuote, countColon))
            {
                countColon++;
            }
            else return -1;
            break;
        case ',':
            //printf("%c", ',');
            if (IsCommaValid(countQuote, countColon))
            {
                countColon = 0;
                countQuote = 0;
            }
            else return -1;
            break;
        case '[':

            break;
            //Nede to check in case ts invalid - need to checl any key characters
        case ']':

            break;
        default:
            //add to curr value bucket
            continue;
        }
    }
    return -1;
}

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

bool IsCurlyOpValid(short countQuote, short countColon)
{
    if (countColon == 1 && countQuote == 2) return true;
    else return false;
}

bool IsCurlyClValid(short countQuote, short countColon)
{
    if (countColon == 1 && countQuote == 2) return true;
    else if (countColon == 1 && countQuote == 4) return true;
    else return false;
}



int IsJsonListValid(char* jsonString, int cursorStart)
{
    cursorStart += 1;
    //Actualy, check for symetry first
    switch (jsonString[cursorStart])
    {
    case '{':
        ValidateObjectList(cursorStart);
        break;
    case '"':
        ValidateStringList();
        break;
    case '[':
        ValidateListList();
        break;
    case ',':
        //error
        break;
    case ':':
        //error 
        break;
    case '}':
        //error
        break;
    case ']':
        //error
        break;
    default:
        ValidateValueList();
    }


    return -1;
}

int ValidateObjectList(char* jsonString, int cursorStart)
{
    char* template;
    //Would need to validate the object
    //But only the first one
    int start = cursorStart;
    int end = IsJsonObjectValid(jsonString, cursorStart);
    if (end = -1)
    {
        return end;
    }
    int cursor = start;
    template = 

    char currChar;
    for (int i = start; i < end; i++)
    {

    }
    do
    {
        currChar = jsonString[cursor];
        if (currChar != ']')
        {

        }
    } while (currChar != ']' && cursor < strlen(jsonString));


}
int ValidateStringList()
{

}

int ValidateValueList()
{

}

int ValidateListList()
{

}
//Possible also to use strategies here? 
//i.e have a strategy for object lists
//A strategy for quote lists
//A strategy for none
//I thnk thisis acceptable, because the first value in the list must decide what kind of list its going tobe.
//One cannot change ones mind later on


bool JsonValid(char* jsonString)
{
    if (IsJsonObjectValid(jsonString, 0) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

*/