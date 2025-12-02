#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "structsAndConsts.h"
#include "structAccessFunctions.h"
#include "differentiatorFunctions.h"
#include "mathExpressionReader.h"
#include "helpingFunctions.h"
#include "DSL.h"

void syntaxError (char** bufPos) {
    assert(bufPos);

    printf("Syntax error in position %p: error symbol \"%c\"\n", *bufPos, **bufPos);
    printf("In string \"%s\"\n",  *bufPos);
    assert(0); //FIXME may be
}

node_t* getNumberNode (tree_t* tree, char** bufPos) {
    assert(tree);
    assert(bufPos);

    double value = 0.0;
    double sign = 1.0;

    skipSpaces(bufPos);

    if (**bufPos == '-'){
        sign = -1.0;
        (*bufPos)++;
    }

    char* oldPos = *bufPos;
    while (('0' <= **bufPos) && (**bufPos <= '9')) {
        value = value*10 + (**bufPos -'0');
        (*bufPos)++;
    }

    if (oldPos == *bufPos)
        return getVariableNode(tree, bufPos);

    if (**bufPos == '.') {
        (*bufPos)++;

        char* posAfterPoint = *bufPos;
        for (double divider = 10;('0' <= **bufPos) && (**bufPos <= '9'); divider *= 10) {
            value = value + (**bufPos -'0')/divider;
            (*bufPos)++;
        }
        if (posAfterPoint == *bufPos)
            syntaxError(bufPos);
    }
    skipSpaces(bufPos);

    return NUM_(sign*value);
}

node_t* getVariableNode (tree_t* tree, char** bufPos) {
    assert(tree);
    assert(bufPos);

    skipSpaces(bufPos);

    int lenOfName = 0;

    char objectName[MAX_OP_NAME_LEN] = {};
    sscanf(*bufPos, "%64s%n", objectName, &lenOfName);

    #include "operatorsArray.h"
    for (size_t numOfOp = 0; numOfOp < NUM_OF_OPERATORS; numOfOp++)
        if (strncmp((operatorsArray[numOfOp]).opName, objectName, MAX_OP_NAME_LEN) == 0)
            return getUnaryOperatorNode(tree, bufPos);

    (*bufPos) += lenOfName;

    unsigned long long variableHash = getStringHash(objectName);
    struct variableInfo* searchedVariable = (struct variableInfo*)bsearch(&variableHash,
    tree->variableArr, tree->variableArrSize, sizeof(struct variableInfo), bsearchHashComparator);

    if((searchedVariable == NULL) || (strcmp(searchedVariable->varName, objectName) != 0)) {

        strncpy(((tree->variableArr)[0]).varName, objectName, lenOfName);
        ((tree->variableArr)[0]).varHash = variableHash;
        qsort(tree->variableArr, tree->variableArrSize, sizeof(struct variableInfo), structVariableComparator);
    }
    skipSpaces(bufPos);

    return newNodeCtor(tree, typeVariable, {.varHash = variableHash}, NULL, NULL);
}

node_t* getUnaryOperatorNode (tree_t* tree, char** bufPos) {
    assert(tree);
    assert(bufPos);

    char objectName[MAX_OP_NAME_LEN] = {};
    int lenOfName = 0;

    skipSpaces(bufPos);
    sscanf(*bufPos, "%64s%n", objectName, &lenOfName);

    #include "operatorsArray.h"
    for (size_t numOfOp = 0; numOfOp < NUM_OF_OPERATORS; numOfOp++)
        if (strncmp((operatorsArray[numOfOp]).opName, objectName, MAX_OP_NAME_LEN) == 0) {
            (*bufPos) += lenOfName;
            skipSpaces(bufPos);
            return newNodeCtor(tree, typeOperator, {.opCode = (operatorsArray[numOfOp]).opCode}, NULL, getBracketExpressionNode(tree, bufPos));
        }

    skipSpaces(bufPos);
    return getBracketExpressionNode(tree, bufPos);
}


node_t* getBracketExpressionNode (tree_t* tree, char** bufPos) {
    assert(tree);
    assert(bufPos);

    skipSpaces(bufPos);

    if (**bufPos == '(') {
        (*bufPos)++;
        node_t* newNode = getADDandSUBnodes(tree, bufPos);

        skipSpaces(bufPos);
        if (**bufPos == ')')
            (*bufPos)++;
        else syntaxError(bufPos);
        return newNode;
    }
    else
        return getNumberNode(tree, bufPos);
}

node_t* getMULandDIVnodes (tree_t* tree, char** bufPos) {
    assert(tree);
    assert(bufPos);

    skipSpaces(bufPos);
    node_t* firstOperand = getPowNodes(tree, bufPos);

    while((**bufPos == '*') || (**bufPos == '/')) {
        int op = **bufPos;
        (*bufPos)++;
        node_t* secondOperand = getPowNodes(tree, bufPos);
        if (op == '*')
            firstOperand = MUL_(firstOperand, secondOperand);
        else
            firstOperand = DIV_(firstOperand, secondOperand);
    }
    skipSpaces(bufPos);
    return firstOperand;
}

node_t* getPowNodes (tree_t* tree, char** bufPos) {
    assert(tree);
    assert(bufPos);

    skipSpaces(bufPos);
    node_t* firstOperand = getUnaryOperatorNode(tree, bufPos);

    skipSpaces(bufPos);
    while(**bufPos == '^') {
        (*bufPos)++;

        node_t* secondOperand = getUnaryOperatorNode(tree, bufPos);

        firstOperand = POW_(firstOperand, secondOperand);
        skipSpaces(bufPos);
    }

    skipSpaces(bufPos);
    return firstOperand;
}

node_t* getADDandSUBnodes (tree_t* tree, char** bufPos) {
    assert(tree);
    assert(bufPos);

    skipSpaces(bufPos);
    node_t* firstOperand = getMULandDIVnodes(tree, bufPos);

    while((**bufPos == '+') || (**bufPos == '-')) {
        int op = **bufPos;
        (*bufPos)++;
        node_t* secondOperand = getMULandDIVnodes(tree, bufPos);
        if (op == '+')
            firstOperand = ADD_(firstOperand, secondOperand);
        else
            firstOperand = SUB_(firstOperand, secondOperand);
    }

    skipSpaces(bufPos);
    return firstOperand;
}

node_t* getMathExpressionTree (tree_t* tree, char** bufPos) {
    assert(tree);
    assert(bufPos);

    skipSpaces(bufPos);
    node_t* newNode = getADDandSUBnodes(tree, bufPos);

    skipSpaces(bufPos);
    if (**bufPos != '$')
        syntaxError(bufPos);

    (*bufPos)++;

    return newNode;
}
