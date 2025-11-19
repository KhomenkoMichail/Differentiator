#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>

#include "structsAndConsts.h"
#include "structAccessFunctions.h"
#include "treeFunctions.h"
#include "differentiatorFunctions.h"

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


int readFileAndCreateTree (tree_t* tree, dump* dumpInfo, const char* nameOfFile) {
    assert(tree);
    assert(dumpInfo);
    assert(nameOfFile);

    FILE* dumpFile = 0;
    if(dumpInfo->dumpFileWasOpened)
        dumpFile = fopen(dumpInfo->nameOfDumpFile, "a");
    else {
        dumpFile = fopen(dumpInfo->nameOfDumpFile, "w");
        dumpInfo->dumpFileWasOpened = 1;
    }

    if (dumpFile == NULL) {
        fprintf(stderr, "Error of opening file \"%s\"", dumpInfo->nameOfDumpFile);
        perror("");
        return 1;
    }

    tree->variableArr = (variableInfo*)calloc(NUM_OF_VARS, sizeof(variableInfo));
    tree->numOfVariables = NUM_OF_VARS;

    char* bufferStart = copyFileContent(nameOfFile);
    if (bufferStart == NULL) {
        printf("Error of copying tree from file\n");
        return 1;
    }
    else
        *treeRoot(tree) = nodeCtorByReadBuffer(&bufferStart, tree, dumpInfo, dumpFile);

    if (fclose(dumpFile) != 0) {
        fprintf(stderr, "Error of closing file \"%s\"", dumpInfo->nameOfDumpFile);
        perror("");
        return 1;
    }

    treeDump(tree, dumpInfo, "Вот созданное финальное дерево:");

    return 0;
}

node_t* nodeCtorByReadBuffer(char** bufPos, tree_t* tree, dump* dumpInfo, FILE* dumpFile) {
    assert(bufPos);
    assert(dumpInfo);
    assert(tree);
    assert(dumpFile);

    DUMP_MESSAGE(dumpFile, "Зашла в функцию создания узла\n", *bufPos);
    skipSpaces(bufPos);
    if(**bufPos == '(') {
        *treeSize(tree) += 1;
        (*bufPos)++;
        skipSpaces(bufPos);

        DUMP_MESSAGE(dumpFile, "Прочитала (\n", *bufPos);

        node_t* newNode = (node_t*)calloc(1, sizeof(node_t));

        *nodeType(newNode) = getNodeType(bufPos); //FIXME variable!!
        processNodeType(tree, newNode, bufPos); //trree -> error bad op

        DUMP_MESSAGE(dumpFile, "Прочитала имя узла.\n", *bufPos);

        DUMP_MESSAGE(dumpFile, "<h3>Сейчас зайду в левое поддерево.\n</font></h3>", *bufPos);
        *nodeLeft(newNode) = nodeCtorByReadBuffer(bufPos, tree, dumpInfo, dumpFile);

        if(*nodeLeft(newNode) != NULL) {
            fclose(dumpFile);
            SUBTREE_DUMP((*nodeLeft(newNode)), dumpInfo, "Вот созданное ЛЕВОЕ поддерево.\n");
            dumpFile = fopen(dumpInfo->nameOfDumpFile, "a");
        }

        DUMP_MESSAGE(dumpFile, "<h3>Сейчас зайду в правое поддерево.\n</font></h3>", *bufPos);
        *nodeRight(newNode) = nodeCtorByReadBuffer(bufPos, tree, dumpInfo, dumpFile);

        if(*nodeRight(newNode) != NULL) {
            fclose(dumpFile);
            SUBTREE_DUMP((*nodeRight(newNode)), dumpInfo, "Вот созданное ПРАВОЕ поддерево.\n");
            dumpFile = fopen(dumpInfo->nameOfDumpFile, "a");
        }

        skipSpaces(bufPos);
        (*bufPos)++;

        DUMP_MESSAGE(dumpFile, "Прочитала ).\n", *bufPos);

        return newNode;
    }

    if (strncmp(*bufPos, "nil", 3) == 0) {
        DUMP_MESSAGE(dumpFile, "Нашла nil.\n", *bufPos);
        (*bufPos) += 4;
        DUMP_MESSAGE(dumpFile, "Прочитала nil.\n", *bufPos);
        return NULL;
    }

    return NULL;
}

void skipSpaces(char** bufPos) {
    assert(bufPos);
    assert(*bufPos);

    while(**bufPos == ' ')
        (*bufPos)++;
}

nodeType_t getNodeType(char** bufPos) {
    assert(bufPos);
    assert(*bufPos);

    if (isdigit(**bufPos))
        return typeNumber;

    if (**bufPos == '-') {
        if (isdigit(*(*bufPos + 1)))
            return typeNumber;
        else
            return typeOperator;
    }

    char operatorName[MAX_OP_NAME_LEN] = {};
    sscanf(*bufPos, "%64s", operatorName);

    #include "operatorsArray.h"
    for (size_t numOfOp = 0; numOfOp < NUM_OF_OPERATORS; numOfOp++)
        if (strncmp((operatorsArray[numOfOp]).opName, operatorName, MAX_OP_NAME_LEN) == 0)
             return typeOperator;

    return typeVariable;;
}

int processNodeType (tree_t* tree, node_t* node, char** bufPos) {
    assert(tree);
    assert(bufPos);
    assert(*bufPos);
    assert(node);

    int lenOfValue = 0;
    char valueString[STR_SIZE] = {};

    switch (*nodeType(node)) {
        case typeNumber:
            sscanf(*bufPos, "%64s%n", valueString, &lenOfValue);
            (*bufPos) += lenOfValue;

            (nodeValue(node))->constValue = atof(valueString);
            break;

        case typeVariable: {//FIXME
            sscanf(*bufPos, "%s%n", valueString, &lenOfValue);
            (*bufPos) += lenOfValue;

            unsigned long long variableHash = getStringHash(valueString);
            struct variableInfo* searchedVariable = (struct variableInfo*)bsearch(&variableHash, tree->variableArr, tree->numOfVariables, sizeof(struct variableInfo), bsearchHashComparator);

            if((searchedVariable != NULL) && (strcmp(searchedVariable->varName, valueString) == 0)) {
                (nodeValue(node))->varHash = searchedVariable->varHash;
            }
            else {
                ((tree->variableArr)[0]).varName = strdup(valueString);
                ((tree->variableArr)[0]).varHash = variableHash;
                qsort(tree->variableArr, tree->numOfVariables, sizeof(struct variableInfo), structVariableComparator);
                (nodeValue(node))->varHash = variableHash;
            }

            break;
        }
        case typeOperator: {
            sscanf(*bufPos, "%64s%n", valueString, &lenOfValue);
            (*bufPos) += lenOfValue;

            #include "operatorsArray.h"
            int curOpCode = 0;

            for (size_t numOfOp = 0; numOfOp < NUM_OF_OPERATORS; numOfOp++)
                if (strncmp((operatorsArray[numOfOp]).opName, valueString, MAX_OP_NAME_LEN) == 0) {
                    curOpCode = (operatorsArray[numOfOp]).opCode;
                    break;
                }

            if (curOpCode == 0) {
                printf("ERROR! Bad operator name \"%s\"\n", valueString);
                return 1;
            }

            (nodeValue(node))->opCode = (operatorCode_t)curOpCode;
            break;
        }
        default:
            break;
    }

    return 0;
}

double solveNode (tree_t* tree, node_t* node) {
    assert(tree);
    assert (node);

    switch (*nodeType(node)) {
        case typeNumber:
            return (nodeValue(node))->constValue;
            break;
        case typeVariable: {
            struct variableInfo* searchedVariable = (struct variableInfo*)bsearch(&((nodeValue(node))->varHash), tree->variableArr, tree->numOfVariables, sizeof(struct variableInfo), bsearchHashComparator);

            if (!searchedVariable) {
                printf("Error of searching variable!\n");
                return NAN;
            }

            return (searchedVariable->varValue);
        }

        case typeOperator: {
            double leftValue = NAN;
            double rightValue = NAN;

            if(*nodeLeft(node))
                leftValue = solveNode(tree, *nodeLeft(node));

            if(*nodeRight(node))
                rightValue = solveNode(tree, *nodeRight(node));

            switch ((nodeValue(node))->opCode) {
                case opADD:
                    return (leftValue + rightValue);
                case opSUB:
                    return (leftValue - rightValue);
                case opMUL:
                    return (leftValue * rightValue);
                case opDIV:
                    return (leftValue / rightValue);
                case opPOW:
                    return (pow(leftValue, rightValue));
                case opSIN:
                    return (sin(rightValue));
                case opCOS:
                    return (cos(rightValue));
                default:
                    return NAN;
            }
        }
        default:
            break;
    }

    return NAN;
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
