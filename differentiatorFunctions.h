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

    double getDoubleVarValue (const char* varName);

    node_t* newNodeCtor (nodeType_t nodeType, nodeValue_t nodeValue, node_t* left, node_t* right);

    node_t* copyNode (node_t* node);

    node_t* differentiateNode (tree_t* tree, node_t* node, dump* dumpInfo, const char* diffVarName, FILE* latexFile);

    int findDiffVariable (node_t* node, unsigned long long diffVarHash);

    node_t* fprintfNodeToLatex (tree_t* tree, node_t* node, node_t* parentNode, FILE* latexFile);

    int needBrackets (node_t* node);

    int getOperatorPriority (operatorCode_t opCode);

    void printfLatexReport (tree_t* expressionTree, dump* dumpInfo);

    const char* getRandomPhrase (void);

#endif
