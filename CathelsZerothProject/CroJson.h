#ifndef CROJSON_H
#define CROJSON_H

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


typedef struct TreeNode
{
    char* name;
    enum NodeType nodeType;
    bool boolVal;
    int intVal;
    float floatVal;
    char* stringVal;
    struct TreeNode* child;
    struct TreeNode* next;
}TreeNode;


typedef struct JsonBuffer
{
    char* jsonString;
    int cursor;
    int length;
    short callDepth;
}JsonBuffer;


bool ParseFloat(char*, float*);
bool ParseInt(char*, int*);
void PrintError();
TreeNode* CreateNamedNode(JsonBuffer*);
bool AttatchNodeToRoot(TreeNode*, TreeNode*);
void SkipWhiteSpace(JsonBuffer*);
void FreeNode(TreeNode*);
TreeNode* GetJsonTree(char*);
bool ParseObject(JsonBuffer*, TreeNode*);
TreeNode** ParseValue(JsonBuffer*, TreeNode**);
bool ParseList(JsonBuffer*, TreeNode*);
bool ParseNonString(JsonBuffer*, TreeNode*);
bool ParseString(JsonBuffer*, TreeNode*);
char* ReadContent(JsonBuffer*, bool (*CheckChar)(JsonBuffer*, char*, int*));
bool CheckCharString(JsonBuffer*, char*, int*);
bool CheckCharNonString(JsonBuffer*, char*, int*);
TreeNode* CreateNamedNode(JsonBuffer*);
void AddCharToContent(char, char*, int*);




/*
* -> Used for the structure validation methoid which is currently on hold.
bool IsQuoteValid(short, short);
bool IsCommaValid(short, short);
bool IsColonValid(short, short);
bool IsCurlyOpValid(short, short);
bool IsCurlyClValid(short, short);
int IsJsonListValid(char*, int);
int ValidateObjectList(char*, int);
int ValidateStringList(char*, int);
int ValidateValueList(char*, int);
int ValidateListList(char*, int);
*/
#endif
