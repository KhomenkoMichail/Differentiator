#include <stdio.h>

#include "structsAndConsts.h"
#include "structAccessFunctions.h"
#include "treeFunctions.h"
#include "differentiatorFunctions.h"

int main (void) {
    tree_t expressionTree = {};
    struct dump dumpInfo = {};

    dumpInfo.nameOfGraphFile = "treeGraph.txt";
    dumpInfo.nameOfDumpFile = "dump.html";
    dumpInfo.nameOfLatexFile = "latexReport.tex";
    dumpInfo.nameOfPlotFile = "funcGraph.plt";

    readFileAndCreateTree(&expressionTree, &dumpInfo, "mathExpression.txt");
    solveMathExpressionTree (&expressionTree, &dumpInfo);

    printfLatexReport(&expressionTree, &dumpInfo);
    deleteTree(&expressionTree);

}
