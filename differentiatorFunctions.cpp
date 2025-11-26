#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <math.h>

#include "structsAndConsts.h"
#include "structAccessFunctions.h"
#include "treeFunctions.h"
#include "differentiatorFunctions.h"
#include "helpingFunctions.h"
#include "texReportFunctions.h"
#include "DSL.h"

int readFileAndCreateTree (tree_t* tree, dump* dumpInfo, const char* nameOfFile) {
    assert(tree);
    assert(dumpInfo);
    assert(nameOfFile);

    FILE* dumpFile = 0;
    if(dumpInfo->dumpFileWasOpened)
        dumpFile = fopen(dumpInfo->nameOfDumpFile, "a");
    else {
        dumpFile = fopen(dumpInfo->nameOfDumpFile, "w");
        dumpInfo->dumpFileWasOpened = 1;
    }

    if (dumpFile == NULL) {
        fprintf(stderr, "Error of opening file \"%s\"", dumpInfo->nameOfDumpFile);
        perror("");
        return 1;
    }

    tree->variableArr = (variableInfo*)calloc(NUM_OF_VARS, sizeof(variableInfo));
    tree->variableArrSize = NUM_OF_VARS;

    char* bufferStart = copyFileContent(nameOfFile);
    if (bufferStart == NULL) {
        printf("Error of copying tree from file\n");
        return 1;
    }
    else
        *treeRoot(tree) = nodeCtorByReadBuffer(&bufferStart, tree, NULL, dumpInfo, dumpFile);

    if (fclose(dumpFile) != 0) {
        fprintf(stderr, "Error of closing file \"%s\"", dumpInfo->nameOfDumpFile);
        perror("");
        return 1;
    }

    treeDump(tree, dumpInfo, "Вот созданное финальное дерево:");

    return 0;
}

node_t* nodeCtorByReadBuffer(char** bufPos, tree_t* tree, node_t* parentNode, dump* dumpInfo, FILE* dumpFile) {
    assert(bufPos);
    assert(dumpInfo);
    assert(tree);
    assert(dumpFile);

    DUMP_MESSAGE(dumpFile, "Зашла в функцию создания узла\n", *bufPos);
    skipSpaces(bufPos);
    if(**bufPos == '(') {
        *treeSize(tree) += 1;
        (*bufPos)++;
        skipSpaces(bufPos);

        DUMP_MESSAGE(dumpFile, "Прочитала (\n", *bufPos);

        node_t* newNode = (node_t*)calloc(1, sizeof(node_t));

        *nodeType(newNode) = getNodeType(bufPos);
        processNodeType(tree, newNode, bufPos);

        *(nodeParent(newNode)) = parentNode;

        DUMP_MESSAGE(dumpFile, "Прочитала имя узла.\n", *bufPos);

        DUMP_MESSAGE(dumpFile, "<h3>Сейчас зайду в левое поддерево.\n</font></h3>", *bufPos);
        *nodeLeft(newNode) = nodeCtorByReadBuffer(bufPos, tree, newNode, dumpInfo, dumpFile);

        if(*nodeLeft(newNode) != NULL) {
            fclose(dumpFile);
            SUBTREE_DUMP((*nodeLeft(newNode)), dumpInfo, "Вот созданное ЛЕВОЕ поддерево.\n");
            dumpFile = fopen(dumpInfo->nameOfDumpFile, "a");
        }

        DUMP_MESSAGE(dumpFile, "<h3>Сейчас зайду в правое поддерево.\n</font></h3>", *bufPos);
        *nodeRight(newNode) = nodeCtorByReadBuffer(bufPos, tree, newNode, dumpInfo, dumpFile);

        if(*nodeRight(newNode) != NULL) {
            fclose(dumpFile);
            SUBTREE_DUMP((*nodeRight(newNode)), dumpInfo, "Вот созданное ПРАВОЕ поддерево.\n");
            dumpFile = fopen(dumpInfo->nameOfDumpFile, "a");
        }

        skipSpaces(bufPos);
        (*bufPos)++;

        DUMP_MESSAGE(dumpFile, "Прочитала ).\n", *bufPos);

        return newNode;
    }

    if (strncmp(*bufPos, "nil", 3) == 0) {
        DUMP_MESSAGE(dumpFile, "Нашла nil.\n", *bufPos);
        (*bufPos) += 4;
        DUMP_MESSAGE(dumpFile, "Прочитала nil.\n", *bufPos);
        return NULL;
    }

    return NULL;
}

nodeType_t getNodeType(char** bufPos) {
    assert(bufPos);
    assert(*bufPos);

    if (isdigit(**bufPos))
        return typeNumber;

    if (**bufPos == '-') {
        if (isdigit(*(*bufPos + 1)))
            return typeNumber;
        else
            return typeOperator;
    }

    char operatorName[MAX_OP_NAME_LEN] = {};
    sscanf(*bufPos, "%64s", operatorName);

    #include "operatorsArray.h"
    for (size_t numOfOp = 0; numOfOp < NUM_OF_OPERATORS; numOfOp++)
        if (strncmp((operatorsArray[numOfOp]).opName, operatorName, MAX_OP_NAME_LEN) == 0)
             return typeOperator;

    return typeVariable;
}

int processNodeType (tree_t* tree, node_t* node, char** bufPos) {
    assert(tree);
    assert(bufPos);
    assert(*bufPos);
    assert(node);

    int lenOfValue = 0;
    char valueString[STR_SIZE] = {};

    switch (*nodeType(node)) {
        case typeNumber:
            sscanf(*bufPos, "%64s%n", valueString, &lenOfValue);
            (*bufPos) += lenOfValue;

            (nodeValue(node))->constValue = atof(valueString);
            break;

        case typeVariable: {
            sscanf(*bufPos, "%s%n", valueString, &lenOfValue);
            (*bufPos) += lenOfValue;

            unsigned long long variableHash = getStringHash(valueString);
            struct variableInfo* searchedVariable = (struct variableInfo*)bsearch(&variableHash,
            tree->variableArr, tree->variableArrSize, sizeof(struct variableInfo), bsearchHashComparator);

            if((searchedVariable != NULL) && (strcmp(searchedVariable->varName, valueString) == 0)) {
                (nodeValue(node))->varHash = searchedVariable->varHash;
            }
            else {
                strncpy(((tree->variableArr)[0]).varName, valueString, lenOfValue);
                ((tree->variableArr)[0]).varHash = variableHash;
                qsort(tree->variableArr, tree->variableArrSize, sizeof(struct variableInfo), structVariableComparator);
                (nodeValue(node))->varHash = variableHash;
            }

            break;
        }
        case typeOperator: {
            sscanf(*bufPos, "%64s%n", valueString, &lenOfValue);
            (*bufPos) += lenOfValue;

            #include "operatorsArray.h"
            int curOpCode = 0;

            for (size_t numOfOp = 0; numOfOp < NUM_OF_OPERATORS; numOfOp++)
                if (strncmp((operatorsArray[numOfOp]).opName, valueString, MAX_OP_NAME_LEN) == 0) {
                    curOpCode = (operatorsArray[numOfOp]).opCode;
                    break;
                }

            if (curOpCode == 0) {
                printf("ERROR! Bad operator name \"%s\"\n", valueString);
                return 1;
            }

            (nodeValue(node))->opCode = (operatorCode_t)curOpCode;
            break;
        }
        default:
            break;
    }

    return 0;
}

double solveNode (tree_t* tree, node_t* node) {
    assert(tree);
    assert(node);

    switch (*nodeType(node)) {
        case typeNumber:
            return (nodeValue(node))->constValue;
            break;
        case typeVariable: {
            struct variableInfo* searchedVariable = (struct variableInfo*)bsearch(&((nodeValue(node))->varHash), tree->variableArr, tree->variableArrSize, sizeof(struct variableInfo), bsearchHashComparator);

            if (!searchedVariable) {
                printf("Error of searching variable!\n");
                return NAN;
            }

            return (searchedVariable->varValue);
        }

        case typeOperator: {
            double leftValue = NAN;
            double rightValue = NAN;

            if(*nodeLeft(node))
                leftValue = solveNode(tree, *nodeLeft(node));

            if(*nodeRight(node))
                rightValue = solveNode(tree, *nodeRight(node));

            switch ((nodeValue(node))->opCode) {
                case opADD:
                    return (leftValue + rightValue);
                case opSUB:
                    return (leftValue - rightValue);
                case opMUL:
                    return (leftValue * rightValue);
                case opDIV:
                    return (leftValue / rightValue);
                case opPOW:
                    return (pow(leftValue, rightValue));
                case opSIN:
                    return (sin(rightValue));
                case opCOS:
                    return (cos(rightValue));
                case opTG:
                    return (tan(rightValue));
                case opCTG:
                    return (1 / (tan(rightValue)));
                case opARCSIN:
                    return (asin(rightValue));
                case opARCCOS:
                    return (acos(rightValue));
                case opARCTG:
                    return (atan(rightValue));
                case opARCCTG:
                    return atan2(1.0, rightValue);
                case opSH:
                    return (sinh(rightValue));
                case opCH:
                    return (cosh(rightValue));
                case opTH:
                    return (tanh(rightValue));
                case opCTH:
                    return (1 / (tanh(rightValue)));
                case opLN:
                    return (log(rightValue));
                case opLOG:
                    return log(rightValue) / log(leftValue);
                case opEXP:
                    return (exp(rightValue));
                default:
                    return NAN;
            }
        }
        default:
            break;
    }

    return NAN;
}

double solveMathExpressionTree (tree_t* tree, dump* dumpInfo) {
    assert(tree);
    assert(dumpInfo);

    for(size_t numOfVar = 0; numOfVar < tree->variableArrSize; numOfVar++)
        if (((tree->variableArr)[numOfVar].varName)[0] != '\0') {
            printf("Enter the value of variable \"%s\": ", (tree->variableArr)[numOfVar].varName);
            (tree->variableArr)[numOfVar].varValue = getDoubleVarValue();
        }

    double result = solveNode(tree, *treeRoot(tree));

    printf("the result of solving Math Expression is : %g\n", result);

    return result;
}

double getDoubleVarValue (void) {
    int ch = 0;
    double number = NAN;
    char ch1 = '\0';

    while ((scanf ("%lf%c", &number, &ch1) != 2) || (ch1 != '\n')) {

        putchar(ch1);
        while ((ch = getchar()) != '\n')
            putchar (ch);

        printf (" is not a number.\nEnter a number like 3, 52.52, -8\n");
    }
    return number;
}

node_t* differentiateNode (tree_t* tree, node_t* node, dump* dumpInfo, const char* diffVarName, FILE* latexFile) {
    assert(node);
    assert(dumpInfo);

    unsigned long long diffVarHash = getStringHash(diffVarName);

    switch (*nodeType(node)) {
        case typeNumber:
            return $(NUM(0.0));
        case typeVariable:
            if ((nodeValue(node))->varHash == diffVarHash)
                return $(NUM(1.0));
            else
                return $(NUM(0.0));
        case typeOperator:
            switch (nodeValue(node)->opCode) {
                case opADD:
                    return $(ADD_(dL, dR));
                case opSUB:
                    return $(SUB_(dL, dR));
                case opMUL:
                    return $(ADD_(MUL_(dL, cR), MUL_(cL, dR)));
                case opDIV:
                    return $(DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), MUL_(cR, cR)));
                case opPOW: {
                    int baseHasDiffVar = findDiffVariable(*nodeLeft(node), diffVarHash);
                    int powHasDiffVar = findDiffVariable(*nodeRight(node), diffVarHash);

                    if(baseHasDiffVar && powHasDiffVar)
                        return $(ADD_(MUL_(MUL_(cR, POW_(cL, SUB_(cR, NUM(1.0)))), dL), MUL_(MUL_(POW_(cL, cR), LN_(cL)), dR)));
                    else if(baseHasDiffVar)
                        return $(MUL_(MUL_(cR, POW_(cL, SUB_(cR, NUM(1.0)))), dL));
                    else if(powHasDiffVar)
                        return $(MUL_(MUL_(POW_(cL, cR), LN_(cL)), dR));
                    else return $(NUM(0.0));
                }
                case opSIN:
                    return $(COMPOUND_FUNC(COS_(cR)));
                case opCOS:
                    return $(COMPOUND_FUNC(MUL_(NUM(-1.0), SIN_(cR))));
                case opTG:
                    return $(COMPOUND_FUNC(DIV_(NUM(1.0), POW_(COS_(cR), NUM(2.0)))));
                case opCTG:
                    return $(COMPOUND_FUNC(DIV_(NUM(-1.0), POW_(SIN_(cR), NUM(2.0)))));
                case opARCSIN:
                    return $(COMPOUND_FUNC(DIV_(NUM(1.0), POW_(SUB_(NUM(1.0), POW_(cR, NUM(2.0))), NUM(0.5)))));
                case opARCCOS:
                    return $(COMPOUND_FUNC(DIV_(NUM(-1.0), POW_(SUB_(NUM(1.0), POW_(cR, NUM(2.0))), NUM(0.5)))));
                case opARCTG:
                    return $(COMPOUND_FUNC(DIV_(NUM(1.0), ADD_(NUM(1.0), POW_(cR, NUM(2.0))))));
                case opARCCTG:
                    return $(COMPOUND_FUNC(DIV_(NUM(-1.0), ADD_(NUM(1.0), POW_(cR, NUM(2.0))))));
                case opSH:
                    return $(COMPOUND_FUNC(CH_(cR)));
                case opCH:
                    return $(COMPOUND_FUNC(SH_(cR)));
                case opTH:
                    return $(COMPOUND_FUNC(DIV_(NUM(1.0), POW_(CH_(cR), NUM(2.0)))));
                case opCTH:
                    return $(COMPOUND_FUNC(DIV_(NUM(-1.0), POW_(SH_(cR), NUM(2.0)))));
                case opLN:
                    return $(COMPOUND_FUNC(DIV_(NUM(1.0), cR)));
                case opLOG:
                    return $(COMPOUND_FUNC(DIV_(NUM(1.0), MUL_(cR, LN_(cL)))));
                case opEXP:
                    return $(COMPOUND_FUNC(EXP_(cR)));
                default:
                    return NULL;
            }

        default:
            return NULL;
    }
    return NULL;
}

node_t* newNodeCtor (tree_t* tree, nodeType_t curType, nodeValue_t curValue, node_t* left, node_t* right) {

    node_t* newNode = (node_t*)calloc(1, sizeof(node_t));

    *nodeType(newNode) = curType;
    *nodeValue(newNode) = curValue;

    *nodeLeft(newNode) = left;
    *nodeRight(newNode) = right;

    if (left)
        *nodeParent(left) = newNode;

    if (right)
        *nodeParent(right) = newNode;

    *treeSize(tree) += 1;
    return newNode;
}

node_t* copyNode (tree_t* tree, node_t* node) {

    node_t* newNode = (node_t*)calloc(1, sizeof(node_t));

    *nodeType(newNode) = *nodeType(node);
    *nodeValue(newNode) = *nodeValue(node);

    if(*nodeLeft(node)) {
        *nodeLeft(newNode) = copyNode(tree, *nodeLeft(node));
        *nodeParent(*nodeLeft(newNode)) = newNode;
    }

    if(*nodeRight(node)) {
        *nodeRight(newNode) = copyNode(tree, *nodeRight(node));
        *nodeParent(*nodeRight(newNode)) = newNode;
    }

    *treeSize(tree) += 1;
    return newNode;
}

int findDiffVariable (node_t* node, unsigned long long diffVarHash) {
    assert(node);

    if (*nodeType(node) == typeVariable)
        if (nodeValue(node)->varHash == diffVarHash)
            return 1;

    if (*nodeLeft(node)) {
        if (findDiffVariable(*nodeLeft(node), diffVarHash))
            return 1;
    }

    if (*nodeRight(node)) {
        if (findDiffVariable(*nodeRight(node), diffVarHash))
            return 1;
    }

    return 0;
}

double constConvolution (tree_t* tree, node_t* node, FILE* latexFile) {
    assert(tree);
    assert(node);
    assert(latexFile);

    double leftConvolution = NOT_CONVOLUTED;
    double rightConvolution = NOT_CONVOLUTED;

    switch (*nodeType(node)) {
        case typeNumber:
            return (nodeValue(node))->constValue;
        case typeVariable:
            return NAN;
        case typeOperator:
            if(*nodeLeft(node))
                leftConvolution = constConvolution (tree, *nodeLeft(node), latexFile);
            if (*nodeRight(node))
                rightConvolution = constConvolution (tree, *nodeRight(node), latexFile);

            if (!(isnan(leftConvolution)) && !(isnan(rightConvolution))) {
                fprintf(latexFile, "%s", getRandomPhrase());
                fprintf(latexFile, "\\[ ");
                fprintfNodeToLatex(tree, node, latexFile);
                fprintf(latexFile, " = ");

                nodeValue(node)->constValue = solveNode(tree, node);
                *nodeType(node) = typeNumber;

                if(*nodeLeft(node)) {
                    free(*nodeLeft(node));
                    *treeSize(tree) -= 1;
                }

                if(*nodeRight(node)) {
                    free(*nodeRight(node));
                    *treeSize(tree) -= 1;
                }

                *nodeLeft(node) = NULL;
                *nodeRight(node) = NULL;

                fprintfNodeToLatex(tree, node, latexFile);
                fprintf(latexFile, "\\]\n");
                return nodeValue(node)->constValue;
            }
            return NAN;
        default: return NAN;
    }
    return NAN;
}

int deleteNeutralNode (tree_t* tree, node_t* node, FILE* latexFile) {
    assert(tree);
    assert(node);
    assert(latexFile);

    if (*nodeLeft(node))
        deleteNeutralNode(tree, *nodeLeft(node), latexFile);

    if (*nodeRight(node))
        deleteNeutralNode(tree, *nodeRight(node), latexFile);

    if (*nodeType(node) == typeOperator) {
        if (nodeValue(node)->opCode == opMUL) {

            if (((*nodeType(*nodeLeft(node)) == typeNumber) && (compareDouble(nodeValue(*nodeLeft(node))->constValue, 0.0))) ||
               ((*nodeType(*nodeRight(node)) == typeNumber) && (compareDouble(nodeValue(*nodeRight(node))->constValue, 0.0)))) {

                makeZeroNode(tree, node, latexFile);
                return 1;
            }

            if ((*nodeType(*nodeLeft(node)) == typeNumber) &&
                (compareDouble(nodeValue(*nodeLeft(node))->constValue, 1.0))) {

                deleteLeftNode(tree, node, latexFile);
                return 1;
            }

            if ((*nodeType(*nodeRight(node)) == typeNumber) &&
                (compareDouble(nodeValue(*nodeRight(node))->constValue, 1.0))) {

                deleteRightNode(tree, node, latexFile);
                return 1;
            }
        }
        if (nodeValue(node)->opCode == opDIV) {

            if ((*nodeType(*nodeLeft(node)) == typeNumber) &&
                (compareDouble(nodeValue(*nodeLeft(node))->constValue, 0.0))) {

                makeZeroNode(tree, node, latexFile);
                return 1;
            }
            if ((*nodeType(*nodeRight(node)) == typeNumber) &&
                (compareDouble(nodeValue(*nodeRight(node))->constValue, 1.0))) {

                deleteRightNode(tree, node, latexFile);
            }
        }
        if (nodeValue(node)->opCode == opADD) {
            if ((*nodeType(*nodeLeft(node)) == typeNumber) &&
                (compareDouble(nodeValue(*nodeLeft(node))->constValue, 0.0))) {

                deleteLeftNode(tree, node, latexFile);
                return 1;
            }
            if ((*nodeType(*nodeRight(node)) == typeNumber) &&
                (compareDouble(nodeValue(*nodeRight(node))->constValue, 0.0))) {

                deleteRightNode(tree, node, latexFile);
                return 1;
            }
        }
        if (nodeValue(node)->opCode == opSUB) {
            if ((*nodeType(*nodeRight(node)) == typeNumber) &&
                (compareDouble(nodeValue(*nodeRight(node))->constValue, 0.0))) {

                deleteRightNode(tree, node, latexFile);
                return 1;
            }
        }
    }

    return 0;
}

void makeZeroNode(tree_t* tree, node_t* node, FILE* latexFile) {
    assert(tree);
    assert(node);
    assert(latexFile);

    fprintf(latexFile, "%s", getRandomPhrase());
    fprintf(latexFile, "\\[");
    fprintfNodeToLatex(tree, node, latexFile);

    deleteNode(tree, *nodeLeft(node));
    deleteNode(tree, *nodeRight(node));

    *nodeLeft(node) = NULL;
    *nodeRight(node) = NULL;

    *nodeType(node) = typeNumber;
    nodeValue(node)->constValue = 0.0;

    fprintf(latexFile, " = ");
    fprintfNodeToLatex(tree, node, latexFile);
    fprintf(latexFile, " \\]\n\n");
}

void deleteRightNode(tree_t* tree, node_t* node, FILE* latexFile) {
    assert(tree);
    assert(node);
    assert(latexFile);

    fprintf(latexFile, "%s", getRandomPhrase());
    fprintf(latexFile, "\\[");
    fprintfNodeToLatex(tree, node, latexFile);

    free(*nodeRight(node));

    node_t* parNode = *nodeParent(node);
    if (parNode) {
        if (*nodeLeft(parNode) == node)
            *nodeLeft(parNode) = *nodeLeft(node);
        else
            *nodeRight(parNode) = *nodeLeft(node);
    }
    else
        *treeRoot(tree) = *nodeLeft(node);

    *nodeParent(*nodeLeft(node)) = parNode;

    fprintf(latexFile, " = ");
    fprintfNodeToLatex(tree, *nodeLeft(node), latexFile);
    fprintf(latexFile, " \\]\n\n");

    free(node);
    *treeSize(tree) -= 2;
}

void deleteLeftNode(tree_t* tree, node_t* node, FILE* latexFile) {
    assert(tree);
    assert(node);
    assert(latexFile);

    fprintf(latexFile, "%s", getRandomPhrase());
    fprintf(latexFile, "\\[");
    fprintfNodeToLatex(tree, node, latexFile);

    free(*nodeLeft(node));

    node_t* parNode = *nodeParent(node);
    if (parNode) {
        if (*nodeLeft(parNode) == node)
            *nodeLeft(parNode) = *nodeRight(node);
        else
            *nodeRight(parNode) = *nodeRight(node);
    }
    else
        *treeRoot(tree) = *nodeRight(node);

    *nodeParent(*nodeRight(node)) = parNode;

    fprintf(latexFile, " = ");
    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
    fprintf(latexFile, " \\]\n\n");

    free(node);
    *treeSize(tree) -= 2;
}

void simplifyTree (tree_t* tree, dump* dumpInfo, FILE* latexFile) {
    assert(tree);
    assert(latexFile);

    fprintf(latexFile, "Наведем косметики в функции:\n");
    fprintf(latexFile, "\\[ f = ");
    fprintfNodeToLatex(tree, *treeRoot(tree), latexFile);
    fprintf(latexFile, " \\]\n\n");

    size_t oldTreeSize = 0;
    do {
        treeDump(tree, dumpInfo, "начало цикла");
        oldTreeSize = *treeSize(tree);
        constConvolution(tree, (*treeRoot(tree)), latexFile);
        deleteNeutralNode(tree, (*treeRoot(tree)), latexFile);
        treeDump(tree, dumpInfo, "конец цикла");
    } while (oldTreeSize != *treeSize(tree));

    fprintf(latexFile, "Итого:\n\\[ ");
    fprintfNodeToLatex(tree, *treeRoot(tree), latexFile);
    fprintf(latexFile, " \\]\n\n");

    treeDump(tree, dumpInfo, "tree AFTER simplifying:");
}


void expandTheFunctionInTaylor(tree_t* expressionTree, dump* dumpInfo, FILE* latexFile) {
    assert(expressionTree);
    assert(dumpInfo);
    assert(latexFile);

    fprintf(latexFile, "\\section{Упражнение третье - разложение по Тайлеру}");

    printf("Enter the name of variable according to which the decomposition of the Taylor formula will be carried out: ");
    const char* diffVarName = getDiffVarName(expressionTree);
    unsigned long long diffVarHash = getStringHash(diffVarName);

    printf("Enter the degree to which decomposition must be carried out: ");
    size_t degree = getSize_t();

    tree_t diffTree = {};
    diffTree.variableArrSize = expressionTree->variableArrSize;
    diffTree.variableArr = expressionTree->variableArr;
    *treeRoot(&diffTree) = *treeRoot(expressionTree);

    tree_t taylorTree = {};
    taylorTree.variableArrSize = expressionTree->variableArrSize;
    taylorTree.variableArr = expressionTree->variableArr;
    *treeRoot(&taylorTree) = newNodeCtor(&taylorTree, typeNumber, {.constValue = 0.0}, NULL, NULL);


    fprintf(latexFile, "{\\Large Ну что почесались:}\n\n");

    for (size_t curDegree = 0; curDegree <= degree; curDegree++) {

        //diffTree.rootNode = differentiateNode(&diffTree, *treeRoot(&diffTree), dumpInfo, diffVarName, latexFile);

        double diffValue = solveNode(&diffTree, *treeRoot(&diffTree));
        double degreeFactorial = (double)getFactorial(curDegree);

        *treeRoot(&taylorTree) = ADD_T_(*treeRoot(&taylorTree), MUL_T_(DIV_T_(NUM_T_(diffValue), NUM_T_(degreeFactorial)), POW_T_(VAR_T_(diffVarHash), NUM_T_((double)curDegree))));

        diffTree.rootNode = differentiateNode(&diffTree, *treeRoot(&diffTree), dumpInfo, diffVarName, latexFile);
    }

    fprintf(latexFile, "{\\Large В итоге:}\n\n");
    fprintf(latexFile, "\\[\\boxed{ ");
    fprintfNodeToLatex(expressionTree, *treeRoot(expressionTree), latexFile);
    fprintf(latexFile, " = ");
    fprintfNodeToLatex(&taylorTree, *treeRoot(&taylorTree), latexFile);
    fprintf(latexFile, "+ o(%s^%llu)}\\]\n", diffVarName, degree);

    fprintf(latexFile, "{\\Large Теперь упростим полученнoе выражение:}\n\n");
    simplifyTree(&taylorTree, dumpInfo, latexFile);

    fprintf(latexFile, "{\\Large Вcё доказано:}\n\n");
    fprintf(latexFile, "\\[\\boxed{ ");
    fprintfNodeToLatex(expressionTree, *treeRoot(expressionTree), latexFile);
    fprintf(latexFile, " = ");
    fprintfNodeToLatex(&taylorTree, *treeRoot(&taylorTree), latexFile);
    fprintf(latexFile, "+ o(%s^%llu)}\\]\n", diffVarName, degree);

    deleteTree(&diffTree);
    deleteTree(&taylorTree);
}




