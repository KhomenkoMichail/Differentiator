#ifndef STRUCTS_AND_CONSTS_H
#define STRUCTS_AND_CONSTS_H

enum nodeType_t {
    typeOperator = 1,
    typeVariable = 2,
    typeNumber = 3,
};

enum operatorCode_t {
    opADD = 1,
    opSUB = 2,
    opMUL = 3,
    opDIV = 4,
    opPOW = 5,
    opSIN = 6,
    opCOS = 7,
    opTG = 8,
    opCTG = 9,
    opARCSIN = 10,
    opARCCOS = 11,
    opARCTG = 12,
    opARCCTG = 13,
    opSH = 14,
    opCH = 15,
    opTH = 16,
    opCTH = 17,
    opLN = 18,
    opLOG = 19,
    opEXP = 20,
};

union nodeValue_t {
    operatorCode_t opCode;
    double constValue;
    unsigned long long varHash;
};

struct node_t {
    nodeType_t type;
    nodeValue_t value;

    node_t* parent;

    node_t* left;
    node_t* right;
};

const size_t STR_SIZE = 64;

struct variableInfo {
    char varName[STR_SIZE];
    double varValue;
    unsigned long long varHash;
};

const size_t NUM_OF_VARS = 16;

struct tree_t {
    node_t* rootNode;
    size_t size;

    variableInfo* variableArr;
    size_t variableArrSize;
    size_t numOfVariables;

    int errorCode;
};

struct dump {
    const char* nameOfFile;
    const char* nameOfFunc;
    unsigned int numOfLine;

    const char* nameOfDumpFile;
    const char* nameOfGraphFile;
    int dumpFileWasOpened;

    const char* nameOfLatexFile;
};

enum treeErr_t {
    noErrors = 0x00,
    badLeft = 0x02,
    badRight = 0x04,
    tooManyRecursiveCalls = 0x08,
};

struct operatorInfo {
    const char* opName;
    operatorCode_t opCode;
    const char* dumpColor;
};

const size_t MAX_OP_NAME_LEN = 5;

const size_t NODE_DESCRIPTION_SIZE = 64;

const size_t MAX_TREE_SIZE = 500;

const size_t COMMAND_LENGTH = 512;

const int NUM_OF_SCREENSHOTS = 25;

const double NOT_CONVOLUTED = 0.131313;

#endif
