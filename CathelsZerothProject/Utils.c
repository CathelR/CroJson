#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void PrintErr(char error[])
{
    printf(" !->error: ");
    printf("%s\n", error);
}



char* StrInsertChar(char* ext, char toAdd)
{
    int newSize = strlen(ext) + 2;
    //How DOE WE DEAL WITH NULL ASSIGNMENT HERE?
    ext = realloc(ext, newSize);
    for (int i = 0; i < newSize - 1; i++)
    {
        ext[newSize - 1 - i] = ext[newSize - 2 - i];
    }
    ext[0] = toAdd;

    return ext;
}


char* StrAddChar(char* string, char toAdd)
{
    int nullPos = strlen(string);
    string = realloc(string, nullPos + 2);
    string[nullPos] = toAdd;
    string[nullPos + 1] = '\0';
    return string;
}


