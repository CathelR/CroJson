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

/*Denotes the type of node we're working with*/
enum NodeType
{
    OBJECT,
    LIST,
    BOOL,
    INT,
    FLOAT,
    STRING
};


/*TreeNode is the building blocks of our tree
-> There is some reduncdancy where we stored different object types - could have been solved by using void pointers and allocating on the heap but it's extra complexity for little gain*/
typedef struct TreeNode
{
    char* name;
    enum NodeType nodeType;
    bool boolVal;
    int intVal;
    float floatVal;
    char* stringVal;
    TreeNode* child;
    TreeNode* arrNext;

}TreeNode;


typedef struct Error
{
    char errorMessage[256];
    int charPos;
}Error;
static Error gl_error;

/*This stores the JsonString we're working with, and relevant processing information*/
typedef struct JsonBuffer
{
    char* jsonString;
    int cursor;
    int length;
    short callDepth;
}JsonBuffer;
/*==================================================================================================================================================*/


/*Macro definition*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
#define buffer_can_advance(buffer) (buffer->cursor+1<buffer->length)  
#define buffer_at_cursor(buffer) (buffer->jsonString+buffer->cursor)
#define buffer_advance(buffer) (buffer->cursor++) 
#define char_is_numeric(inChar) (inChar >= 48 && inChar <= 57)  
/*==================================================================================================================================================*/


/*Prototype declaration*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
bool ParseFloat(char*, float*);
bool ParseInt(char*, int*);
void PrintError();
TreeNode* CreateNode(JsonBuffer*);
bool AttatchNodeToRoot(TreeNode*, TreeNode*);
void SkipWhiteSpace(JsonBuffer*);
void FreeNode(TreeNode*);

/*==================================================================================================================================================*/


/*Json Methods*/
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/


/*
conventions :
---------------------------------
->Do we handle cursor advance at the called method or before it?  - The convention should be that each method leaves the cursor ready
 "so if this is the string"
                          ^ <-when returning, the cursor should be here
 
 ->Each method is responsible for skipping it's own whitespace if its possible there will be any
*/

/*
TO DO :
---------------------------------
Make better use of error printing 
Rebuild tests -> Unit for certain ones (like numbers) as well as Integration tests
*/



//Top level - Interface to recursive methods
TreeNode* GetJsonTree(char* jsonString)
{   
    TreeNode* rootNode = NULL;
    JsonBuffer buffer = { jsonString, 0,strlen(jsonString),0 };
    JsonBuffer* bPtr = &buffer;
    if (buffer_at_cursor(bPtr) == '{')
    {
        rootNode = {};
        if (rootNode != NULL)
        {
            bPtr->cursor += 1; //Increase cursor to the next point
            ParseObject(bPtr, rootNode);
        }
        
    }
    //Handle freeing up here? If fail? Doesn't workif we return null?
}


bool ParseObject(JsonBuffer* bPtr, TreeNode* relRoot) //Where relRoot is the object created when we called parseValue
{
    TreeNode* foci = relRoot;
    do
    {
        if (ParseValue(bPtr, foci)) {
            foci=?? //So the thing I pass in here needs to be the point at which I want to attatch the new node. &TreeNode*. Then *TreeNode* = whatever malloc returns
        }
            
        return false;

    } while (buffer_at_cursor(bPtr) == ','); //Because after parsing each value we should have a comma

    SkipWhiteSpace(bPtr);
    if (!buffer_at_cursor(bPtr) == '}') return false;
    else return true;
}

/*Arrays are implemented at linked lists - the node pointer of each node points to the next item in the list*/
TreeNode* ParseList(JsonBuffer* bPtr, TreeNode* relRoot) //Where relRoot is the object created when we called parseValue
{
    //do while cursor = ,
    //Add new thing to linked list
}

TreeNode* ParseValue(JsonBuffer* bPtr, TreeNode* root) //If I pass in the pointer to the pointer I don't have to worry about what type it is.
{
    SkipWhiteSpace(bPtr);

    if (buffer_at_cursor(bPtr) == '"')
    {
        root = CreateNode(bPtr); //Simply put I want root to be the correct pointer for where I'm actually attatching it
        if (root == NULL)
            return false;
    }
    else return false;

    SkipWhiteSpace(bPtr);
    if (!buffer_at_cursor(bPtr) != ':')
        return false;

    SkipWhiteSpace(bPtr);

    char currChar = buffer_at_cursor(bPtr);
    switch (currChar)
    {
    case '{':
        if (!ParseObject(bPtr, root))
            return false;
        break;
    case '"':
        if (!ParseString(bPtr, root))
            return false;
        break;
    default:
        if (!ParseNonString(bPtr, root))
            return false;
    }




        return true;
    else return false;
}


/*This is basically a work allocation method, decides which strategy to use to parse the value*/
bool ParseNonString(JsonBuffer* bPtr, TreeNode* valueNode)
{
    bool isSuccess = false;
    char* content = ReadNonString(bPtr);//Still want that to be
    if (content == NULL) return false;

    if (content == "true")
    {
        valueNode->boolVal = true;
        isSuccess = true;
    }
    else if (content == "false")
    {
        valueNode->boolVal = false;
        isSuccess = true;
    }
    else if (char_is_numeric(content[strlent(content) - 1]))
    {
        if (strstr(content, "."))
        {
            isSuccess = ParseFloat(content, &valueNode->floatVal);
        }
        else
        {
            isSuccess = ParseInt(content, &valueNode->intVal);
        }
    }
    else isSuccess = false;

    free(content);
    return isSuccess;
}

/*This is essentially a decorator over the ReadString method. It handles the actual assignment*/
bool ParseString(JsonBuffer* bPtr, TreeNode* valueNode)
{
    char* string = ReadString(bPtr);
    if (string != NULL)
    {
        valueNode->stringVal = string;
        return true;
    }
    else return false;
}




/*This is essentially a decorator over the ParseFloat() method - converts the result to int*/
bool ParseInt(char* input, int* intValPtr)
{
    float floatVal;

    if (!ParseFloat(input, &floatVal))
        return false;
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
            //TO DO: instead of doing this, just change the bit
        }
        else if (input[i] == '.' && hasDecPoint == false)
        {
            reductionFactor = (strlen(input) - 1) - i;
            hasDecPoint = true;
        }
        else return false;

    }
    for (int f = 0; f < reductionFactor; f++)
        result = result / 10;

    *floatValPtr = result;
    return true;
}

/*Method to parse a string. Can be used for names as well as values*/
char* ReadText(JsonBuffer* bPtr, char boundaryChar)
{
    SkipWhiteSpace(bPtr);
    bPtr->cursor++; //Advance past first //But SKip WhiteSpace lands us on the next char - so we need a convention...
    char* string = malloc(bPtr->length - bPtr->cursor);
    int index = 0;
    bool isSuccess;
    while (buffer_can_advance(bPtr))
    {
        buffer_advance(bPtr);
        if (buffer_at_cursor(bPtr) == boundaryChar) //Or COmma - so theres the difficulty - it can be comma only in non string
        {
            isSuccess = true;
            break;
        }
        else
        {
            *(string + index) = buffer_at_cursor(bPtr);
            index += 1;
        }
    }

    if (isSuccess)
    {
        *(string + index) = '\0';
        string = realloc((strlen(string) + 1), sizeof(char));
        return string;
    }
    else
    {
        free(string);
        return NULL;
    }
}


char* ReadNonString(JsonBuffer* bPtr)
{
    char* content = malloc((bPtr->length - bPtr->cursor) * sizeof(char));
    bool isSuccess;
    int index = 0;
    SkipWhiteSpace(bPtr);
    while (buffer_can_advance(bPtr))
    {
        buffer_advance(bPtr);
        if (buffer_at_cursor(bPtr) == ' ')
        {
            isSuccess = true;
            break;
        }
        else
        {
            *(content + index) = buffer_at_cursor(bPtr);
            index += 1;
        }
    }
    if (isSuccess)
    {
        /*Doesn't strictly need to be a string, but it's handy to have access to strlen*/
        *(content + index) = '\0';
        content = realloc((strlen(content) + 1), sizeof(char));
        return content;
    }
    else
    {
        free(content);
        return NULL;
    }
}

/*Method creates the new node (whether it be object,scalar etc.. AND Reads the name.*/
TreeNode* CreateNode(JsonBuffer* bPtr)
{
    TreeNode* node = NULL;
    node = malloc(sizeof(TreeNode));
    if (node == NULL)
        return NULL;

    node->name = ReadString(bPtr);
    if (node->name == NULL)
    {
        FreeNode(node);
        return NULL;
    }
    return node;
}


/*Attatches a child that we have just created into the array of a parent node*/
//I'm thinking here we are wasting a lot of cycles - if we used pointer to an array of pointers we could create the nodes and just leave them where they are
bool AttatchNodeToRoot(TreeNode* root, TreeNode* branch)
{
    TreeNode* newSpace = malloc((root->branchCount + 1) * sizeof(TreeNode));
    if (newSpace == NULL)
        return false;

    memcpy(newSpace, root->branches, root->branchCount * sizeof(TreeNode)); //Do I need to error check this??? Is that maybe why he was using function hooks???
    free(root->branches);
    root->branches = newSpace;

    *(root->branches + root->branchCount) = *branch; 

    /*Would like more error checking here*/
    return true;
}



void FreeNode(TreeNode* node)
{
    if (node != NULL)
    {
        if (node->name != NULL) 
            free(node->name);
        if (node->stringVal != NULL) 
            free(node->stringVal);

        for (int i = 0; i < node->branchCount; i++)
        {
            FreeNode(node->branches + i);
        }

        free(node);
    }
}


void SkipWhiteSpace(JsonBuffer* bPtr)
{
    while (buffer_can_advance(bPtr) && buffer_at_cursor(bPtr) <= 32)
    {
        bPtr->cursor++;
    }
}

void PrintError()
{
    printf("\n!!!->error: %s || position: %d", gl_error.errorMessage, gl_error.charPos);
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