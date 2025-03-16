#ifndef MODELS_H
#define MODELS_H

#include <stdbool.h>

struct StatusObject
{
	bool isSuccessful;
	char errorMessage[128];
};
//Will need a struct/structs to represent the JSON

//ENUM denotes the type of node we're working with
enum NodeType
{
	OBJECT,
	LIST,
	VALUE
};

//These are the basic building blocks of out JSON tree
struct TreeNode
{
	char name[64];
	//val is the actual content,  this could be extremely long (images etc) so we allocate on the heap
	char *val;
	int branchCount;
	enum NodeType nodeType;
	//There are likely to be more than one here
	struct TreeNode* branches;
};

//Void Pointers


#endif