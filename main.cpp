#include <stdio.h>

#include "structsAndConsts.h"
#include "structAccessFunctions.h"
#include "treeFunctions.h"
#include "differentiatorFunctions.h"

int main (void) {
    tree_t expressionTree = {};
    tree_t diffTree = {};
    diffTree.size = 1000;
    struct dump dumpInfo = {};

    dumpInfo.nameOfGraphFile = "treeGraph.txt";
    dumpInfo.nameOfDumpFile = "dump.html";

    readFileAndCreateTree(&expressionTree, &dumpInfo, "mathExpression.txt");
    solveMathExpressionTree (&expressionTree, &dumpInfo);

    *treeRoot(&diffTree) = differentiateNode (*treeRoot(&expressionTree), &dumpInfo, "x");
    treeDump(&diffTree, &dumpInfo, "diffTree:");

}
