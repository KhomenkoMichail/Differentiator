#ifndef DIFFERENTIATOR_FUNCTIONS_H
#define DIFFERENTIATOR_FUNCTIONS_H

#define DUMP_MESSAGE(file, string, bufPos)\
    fprintf(file, string);\
    fprintf(file, "\n");\
    fprintf(file, "bufPos == %s\n", bufPos);\
    fprintf(file, "\n");\

    char* copyFileContent (const char* nameOfFile);

    int readFileAndCreateTree (tree_t* tree, dump* dumpInfo, const char* nameOfFile);

    node_t* nodeCtorByReadBuffer(char** bufPos, tree_t* tree, dump* dumpInfo, FILE* dumpFile);

    void skipSpaces(char** bufPos);

    nodeType_t getNodeType(char** bufPos);

    int processNodeType (tree_t* tree, node_t* node, char** bufPos);

    unsigned int getSizeOfFile (int fileDescriptor);

    double solveNode (tree_t* tree, node_t* node);

    int bsearchHashComparator(const void* firstParam, const void* secondParam);

    unsigned long long getStringHash(const char* string);

    int structVariableComparator(const void* firstStruct, const void* secondStruct);
#endif
