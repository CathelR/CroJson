#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Utils.h"


bool GetFilePath(char* filePath)
{
    bool isValid = true;
    printf("Enter the filepath the Json to be read:\n");
    gets(filePath, 128, stdin);
    int filePathLen = strlen(filePath);

    //Make better, get first 3 chars
    char pathRoot[3];
    pathRoot[0] = filePath[0];
    pathRoot[1] = filePath[1];
    pathRoot[2] = filePath[2];

    if (strcmp(pathRoot, "C:\\") != 0)
    {
        PrintErr("Filepath does not Point to the C drive");
        isValid = false;
    }

    char* ext = malloc(sizeof(char));
    if (ext == NULL)
    {
        PrintErr("Error allocating memory during file extension checking");
        return false;
    }
    *ext = '\0';

    //Arguable the loop value should come from the strn length
    for (int i = 1; i < 8; ++i)
    {
        char currChar = filePath[filePathLen - i];

        if (currChar != '.')
        {
            ext = StrInsertChar(ext, currChar);
        }
        else if (currChar == '.')
        {
            ext = StrInsertChar(ext, currChar);
            break;
        }
    }

    if (strcmp(ext, ".txt") != 0)
    {
        PrintErr("File Extension Error. File extension should be .txt");
        isValid = false;
    }

    free(ext);
    //Always null your pointers
    ext = NULL;

    if (isValid)
    {
        return true;
    }
    else
    {
        return false;
    }

}

char* GetFileContents( char filePath[])
{
    FILE* jsonFile = fopen(filePath, "r");
    if (jsonFile == NULL) {
        PrintErr("Not able to open the file.");
        //return false;
    }

    long fileSize;
    if (fseek(jsonFile, 0, SEEK_END) == -1)
    {
        PrintErr("Couldn't Get File Size");
        //return false;
    }

    fileSize = ftell(jsonFile);
    fseek(jsonFile, 0, SEEK_SET);
    //CHunk is the line we've just read. Must be the size of the whole file in case there are no newlines
    char* jsonChunk = malloc(fileSize);
    jsonChunk[fileSize - 1] = '\0';
    char* jsonString = malloc(1);
    jsonString[0] = '\0';

    if (jsonString == NULL || jsonChunk == NULL)
    {
        PrintErr("Error allocating memory during file contents retrieval");
        return NULL;
    }

    //Most likely I will realloc and build hte string
    //COUld actually reduce the size of chunk by the amount of each thing we read in, so the TOTAL size is never more than the file
    while (fgets(jsonChunk, fileSize, jsonFile)) {
        int lineLen = strlen(jsonChunk);
        jsonString = realloc(jsonString, lineLen + strlen(jsonString) + 1);
        strcat(jsonString, jsonChunk);
        //realloc down by the same amount
    }

    fclose(jsonFile);
    free(jsonChunk);
    jsonChunk = NULL;

    return jsonString;
}


