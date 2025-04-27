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

//Would be handier for the run through if each one pointed to the next
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
    int tokenCount;

    StringPool stringPool;
}TokenPool;


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

typedef struct Byte
{
    unsigned int flags : 8;
}Byte;

/*
bool ParseFloat(char*, float*);
bool ParseInt(char*, int*);
void PrintError();
void SetError(char*, char*, int);
void AddErrorContext(char* );
TreeNode* CreateNamedNode(JsonBuffer*);
bool AttatchNodeToRoot(TreeNode*, TreeNode*);
void SkipWhiteSpace(JsonBuffer*, bool);
void FreeNode(TreeNode*);
TreeNode* GetJsonTree(char*);
bool ParseObject(JsonBuffer*, TreeNode*);
TreeNode* ParseValue(JsonBuffer*, TreeNode**, bool);
bool ParseList(JsonBuffer*, TreeNode*);
bool ParseNonString(JsonBuffer*, TreeNode*);
bool ParseString(JsonBuffer*, TreeNode*);
char* ReadContent(JsonBuffer*, bool);
void CheckCharString(JsonBuffer*, char*, int*, Byte*);
void CheckCharNonString(JsonBuffer*, char*, int*, Byte*);
bool ReadValueName(JsonBuffer*, TreeNode*);
void AddCharToContent(char, char*, int*);
TreeNode* SearchTree(char*, TreeNode*);
TreeNode* GetListItem(int, TreeNode*);
void PrintToCursor(JsonBuffer*);

*/

static TokenPool* TokenizeJson(JsonBuffer* bPtr);
static bool AddContentToken(TokenPool* tokens);
static bool AddSyntaxToken(char* tokenVal, TokenPool* tokens);
bool IsQuoteValid(short, short);
bool IsCommaValid(short, short);
bool IsColonValid(short, short);
bool IsCurlyOpenValid(short, short);
bool IsCurlyCloseValid(short, short);
bool IsSquareOpenValid(short, short);
bool IsSquareCloseValid(short, short);
char* ReadContent(JsonBuffer*, bool, char*);
void CheckCharString(JsonBuffer*, char*, int*, Byte*);
void CheckCharNonString(JsonBuffer*, char*, int*, Byte*);
void AddCharToContent(char, char*, int*);
void PrintError();
void SetError(char*, char*, int);
void AddErrorContext(char*);
void SkipWhiteSpace(JsonBuffer*)
//int IsJsonListValid(char*, int);
/*
int ValidateObjectList(char*, int);
int ValidateStringList(char*, int);
int ValidateValueList(char*, int);
int ValidateListList(char*, int);
*/

#endif
