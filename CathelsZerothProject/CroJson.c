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
    int branchCount;
    struct TreeNode* branches;
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
        rootNode = {}
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
    do
    {
        if (ParseValue(bPtr, relRoot)) relRoot->branchCount++; //record that we have an extra branch now
    } while (buffer_at_cursor(bPtr) == ','); //Because after parsing each value we should have a comma

    SkipWhiteSpace(bPtr);
    if (!buffer_at_cursor(bPtr) == '}') return false;
    else return true;
}

TreeNode* ParseList(JsonBuffer* bPtr, TreeNode* relRoot) //Where relRoot is the object created when we called parseValue
{

}

bool ParseValue(JsonBuffer* bPtr, TreeNode* relRoot)
{
    SkipWhiteSpace(bPtr);
    TreeNode* node = NULL;
    if (buffer_at_cursor(bPtr) == '"')
    {
        node = CreateNode(bPtr); //So create node reads the name, which means by the time we get back we should be ready to read the colon
        if (node == NULL)
            return false;
    }
    else return false;

    SkipWhiteSpace(bPtr);//Puts us ready to read...
    if (!buffer_at_cursor(bPtr) != ':')
        return false;

    SkipWhiteSpace(bPtr);

    char currChar = buffer_at_cursor(bPtr);
    switch (currChar)
    {
    case '{':
        ParseObject(bPtr, node); //Needs the node we're working withj as Parse Object deals with whats in the  {}
        break;
    case '"':
        //Dont wan to call read string directly - should be an interacemethod wh ere handle tpe assignment as well
        break;

    default:
        ParseNonString();
    }
    bool attatchSuccess = AtatchNodeToRoot(relRoot, node);
    
    free(node);
    if (attatchSuccess)
        return true;
    else return false;
}



/*Method to parse a string. Can be used for names as well as values*/
char* ReadString(JsonBuffer* bPtr)
{
    SkipWhiteSpace(bPtr);
    bPtr->cursor++; //Advance past first //But SKip WhiteSpace lands us on the next char - so we need a convention...
    char* string = malloc(bPtr->length - bPtr->cursor);
    int index = 0;
    bool isSuccess;
    while (buffer_can_advance(bPtr))
    {
        buffer_advance(bPtr);
        if (buffer_at_cursor(bPtr) == '\"')
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

bool ParseString()
{
    //Read string
    //assign string 
    //set node type
}



/*This is basically a work allocation method, decides which strategy to use to parse the value*/
bool ParseNonString(JsonBuffer* bPtr, TreeNode* valueNode)
{
    bool isSuccess = false;
    char* content = ReadNonString(bPtr);
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
        isSuccess = ParseNumber(content, valueNode);
    }
    else isSuccess = false;

    free(content);
    return isSuccess;
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



bool ParseNumber() 
{

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