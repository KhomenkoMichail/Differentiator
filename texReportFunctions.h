#ifndef TEX_REPORT_FUNCTIONS_H
#define TEX_REPORT_FUNCTIONS_H

    node_t* fprintfNodeToLatex (tree_t* tree, node_t* node, FILE* latexFile);

    int needBrackets (node_t* node);

    int getOperatorPriority (operatorCode_t opCode);

    void printfLatexReport (tree_t* expressionTree, dump* dumpInfo);

    const char* getRandomPhrase (void);

    node_t* fprintfNodeToGnuplot(tree_t* tree, node_t* node, FILE* gnuplotFile);

    void createFunctionGraph (tree_t* tree, const char* graphName, FILE* latexFile,
                              dump* dumpInfo);

    int findTheTangentAtPoint (tree_t* funcTree, tree_t* diffTree, const char* diffVarName,
                            dump* dumpInfo, FILE* latexFile);

    void fprintfTexReportIntro (FILE* latexFile);

#endif
