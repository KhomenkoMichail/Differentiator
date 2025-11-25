#ifndef DIFFERENTIATOR_FUNCTIONS_H
#define DIFFERENTIATOR_FUNCTIONS_H

#define DUMP_MESSAGE(file, string, bufPos)\
    fprintf(file, string);\
    fprintf(file, "\n");\
    fprintf(file, "bufPos == %s\n", bufPos);\
    fprintf(file, "\n");\

    int readFileAndCreateTree (tree_t* tree, dump* dumpInfo, const char* nameOfFile);

    node_t* nodeCtorByReadBuffer (char** bufPos, tree_t* tree, node_t* parentNode, dump* dumpInfo, FILE* dumpFile);

    nodeType_t getNodeType (char** bufPos);

    int processNodeType (tree_t* tree, node_t* node, char** bufPos);

    double solveNode (tree_t* tree, node_t* node);

    double solveMathExpressionTree (tree_t* tree, dump* dumpInfo);

    double getDoubleVarValue (void);

    node_t* newNodeCtor (tree_t* tree, nodeType_t nodeType, nodeValue_t nodeValue, node_t* left, node_t* right);

    node_t* copyNode (tree_t* tree, node_t* node);

    node_t* differentiateNode (tree_t* tree, node_t* node, dump* dumpInfo, const char* diffVarName, FILE* latexFile);

    int findDiffVariable (node_t* node, unsigned long long diffVarHash);

    double constConvolution (tree_t* tree, node_t* node, FILE* latexFile);

    void makeZeroNode (tree_t* tree, node_t* node, FILE* latexFile);

    void deleteRightNode (tree_t* tree, node_t* node, FILE* latexFile);

    void deleteLeftNode (tree_t* tree, node_t* node, FILE* latexFile);

    int deleteNeutralNode (tree_t* tree, node_t* node, FILE* latexFile);

    void simplifyTree (tree_t* tree, dump* dumpInfo, FILE* latexFile);

#endif
