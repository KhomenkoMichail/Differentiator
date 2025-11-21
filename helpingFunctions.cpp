#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "structsAndConsts.h"
#include "helpingFunctions.h"

char* copyFileContent (const char* nameOfFile) {
    assert(nameOfFile);

    int fileDescriptor = open(nameOfFile, O_RDONLY, 0);
    if (fileDescriptor == -1) {

        fprintf(stderr, "Error of opening file \"%s\"", nameOfFile);
        perror("");
        return NULL;
    }

    unsigned int sizeOfFile = getSizeOfFile(fileDescriptor);
    if (sizeOfFile == 0) {
        close(fileDescriptor);
        return NULL;
    }

    char* fileCopyBuffer = (char*)calloc(sizeOfFile + 1, sizeof(char));

    size_t numOfReadSymbols = read(fileDescriptor, fileCopyBuffer, sizeOfFile);
    fileCopyBuffer[numOfReadSymbols] = '\0';

    if(close(fileDescriptor) != 0) {
        fprintf(stderr, "Error of closing file \"%s\"", nameOfFile);
        perror("");
        return NULL;
    }

    return fileCopyBuffer;
}

unsigned int getSizeOfFile (int fileDescriptor) {
    struct stat fileInfo = {};

    if (fstat(fileDescriptor, &fileInfo) == 0)
        return fileInfo.st_size;

    perror("Error of getting the size of the file");
    return 0;
}

void skipSpaces(char** bufPos) {
    assert(bufPos);
    assert(*bufPos);

    while(**bufPos == ' ')
        (*bufPos)++;
}

int bsearchHashComparator(const void* firstParam, const void* secondParam) {
    assert(firstParam);
    assert(secondParam);

    const unsigned long long* labelHash = (const unsigned long long*)firstParam;
    const struct variableInfo* searchedVariable = (const struct variableInfo*)secondParam;

    return (int)(*labelHash - searchedVariable->varHash);
}

unsigned long long getStringHash(const char* string) {
    assert(string);
    unsigned long long hash = 5381;

    for(ssize_t numOfElement = 0; string[numOfElement] != '\0'; numOfElement++)
        hash = ((hash << 5) + hash) + (unsigned long long)(string[numOfElement]);

    return hash;
}

int structVariableComparator(const void* firstStruct, const void* secondStruct) {
    assert(firstStruct);
    assert(secondStruct);

    const struct variableInfo* firstVariable = (const struct variableInfo*)firstStruct;
    const struct variableInfo* secondVariable = (const struct variableInfo*)secondStruct;

    return (int)(firstVariable->varHash - secondVariable->varHash);
}
