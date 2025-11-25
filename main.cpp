#include <stdio.h>
#include <stdlib.h>

#include "structsAndConsts.h"
#include "structAccessFunctions.h"
#include "treeFunctions.h"
#include "differentiatorFunctions.h"
#include "texReportFunctions.h"

int main (void) {
    tree_t expressionTree = {};
    struct dump dumpInfo = {};

    dumpInfo.nameOfGraphFile = "DUMPS/treeGraph.txt";
    dumpInfo.nameOfDumpFile = "DUMPS/dump.html";
    dumpInfo.nameOfLatexFile = "TEX_DUMP/latexReport.tex";
    dumpInfo.nameOfPlotFile = "TEX_DUMP/FUNC_GRAPHS/funcGraph.plt";

    readFileAndCreateTree(&expressionTree, &dumpInfo, "mathExpression.txt");


    printfLatexReport(&expressionTree, &dumpInfo);

    free(expressionTree.variableArr);
    deleteTree(&expressionTree);
}
