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

    readFileAndCreateTree(&expressionTree, &dumpInfo, "mathExpression.txt");
}
