#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "structsAndConsts.h"
#include "structAccessFunctions.h"
#include "treeFunctions.h"
#include "helpingFunctions.h"
#include "differentiatorFunctions.h"
#include "texReportFunctions.h"

static int graphsCounter = 0;

node_t* fprintfNodeToLatex (tree_t* tree, node_t* node, FILE* latexFile) {
    assert(node);
    assert(latexFile);

    switch(*nodeType(node)) {
        case typeNumber:
            if ((nodeValue(node))->constValue >= 0)
                fprintf(latexFile, "%g", (nodeValue(node))->constValue);
            else
                fprintf(latexFile, "(%g)", (nodeValue(node))->constValue);
            break;
        case typeVariable: {
            struct variableInfo* searchedVariable = (struct variableInfo*)bsearch(&(nodeValue(node)->varHash),
            tree->variableArr, tree->variableArrSize, sizeof(struct variableInfo), bsearchHashComparator);

            fprintf(latexFile, "%s", searchedVariable->varName);
            break;
        }
        case typeOperator:
            if (needBrackets(node))
                fprintf(latexFile, "(");
            switch((nodeValue(node))->opCode) {
                case opADD:
                    fprintfNodeToLatex(tree, *nodeLeft(node), latexFile);
                    fprintf(latexFile, " + ");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    break;

                case opSUB:
                    fprintfNodeToLatex(tree, *nodeLeft(node), latexFile);
                    fprintf(latexFile, " - ");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    break;

                case opMUL:
                    fprintfNodeToLatex(tree, *nodeLeft(node), latexFile);
                    fprintf(latexFile, " \\cdot ");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    break;

                case opDIV:
                    fprintf(latexFile, "\\frac{");
                    fprintfNodeToLatex(tree, *nodeLeft(node), latexFile);
                    fprintf(latexFile, "}{");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, "}");
                    break;

                case opPOW:
                    fprintfNodeToLatex(tree, *nodeLeft(node), latexFile);
                    fprintf(latexFile, "^{");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, "}");
                    break;

                case opSIN:
                    fprintf(latexFile, "\\sin(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opCOS:
                    fprintf(latexFile, "\\cos(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opTG:
                    fprintf(latexFile, "\\tan(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opCTG:
                    fprintf(latexFile, "\\cot(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opARCSIN:
                    fprintf(latexFile, "\\arcsin(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opARCCOS:
                    fprintf(latexFile, "\\arccos(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opARCTG:
                    fprintf(latexFile, "\\arctan(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opARCCTG:
                    fprintf(latexFile, "\\arccot(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opSH:
                    fprintf(latexFile, "\\sinh(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opCH:
                    fprintf(latexFile, "\\cosh(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opTH:
                    fprintf(latexFile, "\\tanh(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opCTH:
                    fprintf(latexFile, "\\coth(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opLN:
                    fprintf(latexFile, "\\ln(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opLOG:
                    fprintf(latexFile, "\\log_{");
                    fprintfNodeToLatex(tree, *nodeLeft(node), latexFile);
                    fprintf(latexFile, "}(");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, ")");
                    break;

                case opEXP:
                    fprintf(latexFile, "e^{");
                    fprintfNodeToLatex(tree, *nodeRight(node), latexFile);
                    fprintf(latexFile, "}");
                    break;

                default:
                    break;
            }
            if (needBrackets(node))
                fprintf(latexFile, ")");

        default:
            break;
    }

    return node;
}

int needBrackets(node_t* node) {
    assert(node);

    int currentPriority = getOperatorPriority((nodeValue(node))->opCode);
    int parentPriority = 0;

    if(*nodeParent(node))
        parentPriority = getOperatorPriority((nodeValue(*nodeParent(node)))->opCode);

    return (currentPriority < parentPriority);
}

int getOperatorPriority(operatorCode_t opCode) {
    switch (opCode) {
        case opADD:
        case opSUB:
            return 1;
        case opMUL:
        case opDIV:
            return 2;
        case opPOW:
            return 3;
        case opSIN:
        case opCOS:
        case opTG:
        case opCTG:
        case opARCSIN:
        case opARCCOS:
        case opARCTG:
        case opARCCTG:
        case opSH:
        case opCH:
        case opTH:
        case opCTH:
        case opLN:
        case opLOG:
        case opEXP:
        default:
            return 4;
    }
    return 0;
}

void printfLatexReport(tree_t* expressionTree, dump* dumpInfo) {
    assert(expressionTree);
    assert(dumpInfo);

    FILE* latexFile = fopen(dumpInfo->nameOfLatexFile, "w");

    if (!latexFile) {
        fprintf(stderr, "Error of opening file \"%s\"", dumpInfo->nameOfDumpFile);
        perror("");
        return;
    }

    fprintfTexReportIntro(latexFile);
    fprintfNodeToLatex(expressionTree, *treeRoot(expressionTree), latexFile);
    fprintf(latexFile, " \\]\n\n Ее график имеет вид:\n\n");
    createFunctionGraph(expressionTree, "график исходной функции", latexFile, dumpInfo);


    //tree_t diffTree = {};
    //diffTree.variableArrSize = expressionTree->variableArrSize;
    //diffTree.variableArr = expressionTree->variableArr;

    /*diffTree.rootNode = differentiateNode(&diffTree, *treeRoot(expressionTree), dumpInfo, "x", latexFile);

    simplifyTree(&diffTree, dumpInfo, latexFile);
    createFunctionGraph(&diffTree, "график производной", latexFile, dumpInfo);

    findTheTangentAtPoint(expressionTree, &diffTree, "x", dumpInfo, latexFile);


    deleteTree(&diffTree);*/
    firstDiffReport(expressionTree, dumpInfo, latexFile);

    fprintf(latexFile, "\\end{document}\n");

    if (fclose(latexFile) != 0) {
        fprintf(stderr, "Error of closing file \"%s\"", dumpInfo->nameOfDumpFile);
        perror("");
    }
}

const char* getRandomPhrase (void) {
    const char* phrases[] = { "Нетрудно заметить, что:\n",
                              "Очевидно что:\n",
                              "Из леммы 6.66, следует, что:\n",
                              "Согласно школьной программе:\n",
                              "Внимательный читатель заметит, что\n",
                              "Не умаляя общности:\n",
                              "Произведя некоторые подстановки:\n",
                              "Ввиду нехитрых преобразований:\n",
                              "Все доказано:\n",
                              "Для любых из 6 СПС, верно, что:\n"
                              "(Желаю всем, кто пишет \"СПС\" вместо \"спасибо\"\n"
                              "продуктивно потратить сэкономленное время)\n"};


    size_t numOfPhrases = sizeof(phrases) / sizeof(phrases[0]);

    size_t curPhraseNum = (size_t)rand() % numOfPhrases;

    return phrases[curPhraseNum];
}


node_t* fprintfNodeToGnuplot(tree_t* tree, node_t* node, FILE* gnuplotFile) {
    assert(node);
    assert(gnuplotFile);

    switch(*nodeType(node)) {
        case typeNumber:
            if ((nodeValue(node))->constValue >= 0)
                fprintf(gnuplotFile, "%g", (nodeValue(node))->constValue);
            else
                fprintf(gnuplotFile, "(%g)", (nodeValue(node))->constValue);
            break;
        case typeVariable: {
            struct variableInfo* searchedVariable = (struct variableInfo*)bsearch(&(nodeValue(node)->varHash),
            tree->variableArr, tree->variableArrSize, sizeof(struct variableInfo), bsearchHashComparator);
            fprintf(gnuplotFile, "%s", searchedVariable->varName);
            break;
        }
        case typeOperator:
            if (needBrackets(node))
                fprintf(gnuplotFile, "(");
            switch((nodeValue(node))->opCode) {
                case opADD:
                    fprintfNodeToGnuplot(tree, *nodeLeft(node), gnuplotFile);
                    fprintf(gnuplotFile, " + ");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    break;

                case opSUB:
                    fprintfNodeToGnuplot(tree, *nodeLeft(node), gnuplotFile);
                    fprintf(gnuplotFile, " - ");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    break;

                case opMUL:
                    fprintfNodeToGnuplot(tree, *nodeLeft(node), gnuplotFile);
                    fprintf(gnuplotFile, " * ");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    break;

                case opDIV:
                    fprintfNodeToGnuplot(tree, *nodeLeft(node), gnuplotFile);
                    fprintf(gnuplotFile, " / ");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    break;

                case opPOW:
                    fprintfNodeToGnuplot(tree, *nodeLeft(node), gnuplotFile);
                    fprintf(gnuplotFile, " ** ");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    break;

                case opSIN:
                    fprintf(gnuplotFile, "sin(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opCOS:
                    fprintf(gnuplotFile, "cos(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opTG:
                    fprintf(gnuplotFile, "tan(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opCTG:
                    fprintf(gnuplotFile, "(1/tan(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, "))");
                    break;

                case opARCSIN:
                    fprintf(gnuplotFile, "asin(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opARCCOS:
                    fprintf(gnuplotFile, "acos(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opARCTG:
                    fprintf(gnuplotFile, "atan(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opARCCTG:
                    fprintf(gnuplotFile, "(pi/2 - atan(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, "))");
                    break;

                case opSH:
                    fprintf(gnuplotFile, "sinh(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opCH:
                    fprintf(gnuplotFile, "cosh(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opTH:
                    fprintf(gnuplotFile, "tanh(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opCTH:
                    fprintf(gnuplotFile, "(1/tanh(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, "))");
                    break;

                case opLN:
                    fprintf(gnuplotFile, "log(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                case opLOG:
                    fprintf(gnuplotFile, "(log(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ") / log(");
                    fprintfNodeToGnuplot(tree, *nodeLeft(node), gnuplotFile);
                    fprintf(gnuplotFile, "))");
                    break;

                case opEXP:
                    fprintf(gnuplotFile, "exp(");
                    fprintfNodeToGnuplot(tree, *nodeRight(node), gnuplotFile);
                    fprintf(gnuplotFile, ")");
                    break;

                default:
                    break;
            }
            if (needBrackets(node))
                fprintf(gnuplotFile, ")");
            break;

        default:
            break;
    }

    return node;
}

void createFunctionGraph(tree_t* tree, const char* graphName, FILE* latexFile, dump* dumpInfo) {
    assert(tree);
    assert(graphName);
    assert(latexFile);
    assert(dumpInfo);

    graphsCounter++;

    FILE* gnuplotFile = fopen(dumpInfo->nameOfPlotFile, "w");
    if (!gnuplotFile) {
        fprintf(stderr, "Error opening file \"%s\"", dumpInfo->nameOfPlotFile);
        perror("");
        return;
    }

    fprintfGnuplotHeaders(gnuplotFile);
    fprintfNodeToGnuplot(tree, *treeRoot(tree), gnuplotFile);
    fprintf(gnuplotFile, "\nplot f(x) with lines title \"%s\"\n", graphName);

    if (fclose(gnuplotFile) != 0) {
        fprintf(stderr, "Error of closing file \"%s\"", dumpInfo->nameOfDumpFile);
        perror("");
    }

    char plotCallCommand[STR_SIZE] = {};
    snprintf(plotCallCommand, sizeof(plotCallCommand),
             "gnuplot \"%s\"", dumpInfo->nameOfPlotFile);

    system(plotCallCommand);

    fprintfGraphAtLatex(latexFile, graphName);
}

int findTheTangentAtPoint (tree_t* funcTree, tree_t* diffTree, const char* diffVarName,
                            dump* dumpInfo, FILE* latexFile) {
    assert(funcTree);
    assert(diffTree);
    assert(diffVarName);
    assert(dumpInfo);
    assert(latexFile);

    printf("Enter the point at which you want to get the tangent: ");
    double point = getDoubleVarValue ();

    unsigned long long diffVarHash = getStringHash(diffVarName);

    variableInfo* funcVariable = (struct variableInfo*)bsearch(&diffVarHash,
        funcTree->variableArr, funcTree->variableArrSize, sizeof(variableInfo),
        bsearchHashComparator);

    variableInfo* diffVariable = (struct variableInfo*)bsearch(&diffVarHash,
        diffTree->variableArr, diffTree->variableArrSize, sizeof(variableInfo),
        bsearchHashComparator);

    if ((!funcVariable) || (!diffVariable)) {
        printf("Error in function %s: cannot find variable \"%s\".\n", __func__, diffVarName);
        return 1;
    }

    funcVariable->varValue = point;
    diffVariable->varValue = point;

    double pointValue = solveNode(funcTree, *treeRoot(funcTree));
    double derivativeValue = solveNode(diffTree, *treeRoot(diffTree));

    fprintf(latexFile, "Уравнение касательной функции:\n");

    fprintf(latexFile, "\\[ f = ");
    fprintfNodeToLatex(funcTree, *treeRoot(funcTree), latexFile);
    fprintf(latexFile, " \\]\n\n в точке %s = %g:\n", diffVarName, point);

    fprintf(latexFile, "\\[ f = %g + (%g)*(%s - %g)\\]\n\n", pointValue, derivativeValue,
                                                             diffVarName, point);

    FILE* gnuplotFile = fopen(dumpInfo->nameOfPlotFile, "w");
    if (!gnuplotFile) {
        fprintf(stderr, "Error opening file \"%s\"", dumpInfo->nameOfPlotFile);
        perror("");
        return 1;
    }

    graphsCounter++;

    fprintfGnuplotHeaders(gnuplotFile);
    fprintfNodeToGnuplot(funcTree, *treeRoot(funcTree), gnuplotFile);
    fprintf(gnuplotFile, "\ntangent(x) = %g + (%g)*(%s - %g)\n", pointValue, derivativeValue,
                                                             diffVarName, point);

    fprintf(gnuplotFile, "plot f(x) with lines title \"Исходная функция\", \\\n");
    fprintf(gnuplotFile, "\ttangent(x) with lines title \"Касательная в точке %g\"\n", point);

    if (fclose(gnuplotFile) != 0) {
        fprintf(stderr, "Error of closing file \"%s\"", dumpInfo->nameOfDumpFile);
        perror("");
    }

    char plotCallCommand[STR_SIZE] = {};
    snprintf(plotCallCommand, sizeof(plotCallCommand),
             "gnuplot \"%s\"", dumpInfo->nameOfPlotFile);

    system(plotCallCommand);

    fprintfGraphAtLatex(latexFile, "График касательной функции в точке");

    return 0;
}


void fprintfTexReportIntro (FILE* latexFile) {
    assert(latexFile);

    fprintf(latexFile, "\\documentclass{article}\n");
    fprintf(latexFile, "\\usepackage[T2A]{fontenc}\n");
    fprintf(latexFile, "\\usepackage[utf8]{inputenc}\n");
    fprintf(latexFile, "\\usepackage[russian,english]{babel}\n");
    fprintf(latexFile, "\\usepackage{amsmath}\n");
    fprintf(latexFile, "\\usepackage{amssymb}\n");
    fprintf(latexFile, "\\usepackage{graphicx}\n");
    fprintf(latexFile, "\\usepackage{geometry}\n");
    fprintf(latexFile, "\\geometry{a4paper, margin=0.5cm}\n");
    fprintf(latexFile, "\\begin{document}\n\n");

    fprintf(latexFile, "\\begin{center}\n");
    fprintf(latexFile, "\\vspace*{3cm}\n\n");
    fprintf(latexFile, "{\\Huge \\textbf{Курс С раздолб до отл 10 на семестрой по матану за одну ночь.}}\\\\[1cm]\n");
    fprintf(latexFile, "{\\Large Выполнил: Хоменко М.М.}\\\\[0.5cm]\n");
    fprintf(latexFile, "\\vfill\n");
    fprintf(latexFile, "{\\Large Долгопрудный, \\the\\year}\n");
    fprintf(latexFile, "\\end{center}\n");
    fprintf(latexFile, "\\newpage\n\n");

    fprintf(latexFile, "\\section{Введение}\n");
    fprintf(latexFile, "Семестровая по матану лишь одно из жизненных испытаний, ");
    fprintf(latexFile, "которое вам предстоит пройти. Будьте уверены: каждому, кто учился в школе, ");
    fprintf(latexFile, "по силам сдать семестровую. Все задания составлены на основе школьной программы. ");
    fprintf(latexFile, "Поэтому каждый из вас может успешно сдать семестровую.\n\n");
    fprintf(latexFile, "Автор данного пособия не учился в школе и, дабы помочь себе ");
    fprintf(latexFile, "подобным сдать экзамен, собрал небольшой курс упражнений для подготовки всего за одну ночь. ");
    fprintf(latexFile, "Желаю вам приятного времяпрепровождения.\n\n");

    fprintf(latexFile, "\\section{Упражнение первое: взятие производной простейшей функции}\n");
    fprintf(latexFile, "Имеем функцию:\n");
    fprintf(latexFile, "\\[ f = ");
}

int firstDiffReport (tree_t* expressionTree, dump* dumpInfo, FILE* latexFile) {
    assert(expressionTree);
    assert(dumpInfo);
    assert(latexFile);

    tree_t diffTree = {};
    diffTree.variableArrSize = expressionTree->variableArrSize;
    diffTree.variableArr = expressionTree->variableArr;

    const char* diffVarName = getDiffVarName(expressionTree);

    fprintf(latexFile, "{\\Large Вычислим производную данной функции:}\n\n");
    diffTree.rootNode = differentiateNode(&diffTree, *treeRoot(expressionTree), dumpInfo, diffVarName, latexFile);

    fprintf(latexFile, "{\\Large Теперь упростим полученную производную:}\n\n");
    simplifyTree(&diffTree, dumpInfo, latexFile);

    fprintf(latexFile, "{\\Large Итого получаем:}\n\n");
    fprintf(latexFile, "\\[ \\frac{d}{d%s}(", diffVarName);
    fprintfNodeToLatex(expressionTree, *treeRoot(expressionTree), latexFile);
    fprintf(latexFile, ") = ");
    fprintfNodeToLatex(&diffTree, *treeRoot(&diffTree), latexFile);
    fprintf(latexFile, "\\]\n");

    fprintf(latexFile, "{\\Large График полученной производной:}\n\n");
    createFunctionGraph(&diffTree, "график производной", latexFile, dumpInfo);

    fprintf(latexFile, "\\section{Упражнение второе: вычисление касательной функции в точке}\n");
    findTheTangentAtPoint(expressionTree, &diffTree, diffVarName, dumpInfo, latexFile);

    deleteTree(&diffTree);
    return 0;
}

const char* getDiffVarName (tree_t* tree) {
    assert(tree);

    char diffVarName[STR_SIZE] = {};
    struct variableInfo* searchedVariable = 0;

    printf("Enter the name of the variable by which differentiation is performed: ");

    while(1) {
        for (size_t i = 0; i < STR_SIZE; i++)
            diffVarName[i] = '\0';

        scanf("%s", diffVarName);

        unsigned long long variableHash = getStringHash(diffVarName);
        searchedVariable = (struct variableInfo*)bsearch(&variableHash,
        tree->variableArr, tree->variableArrSize, sizeof(struct variableInfo), bsearchHashComparator);

        if((searchedVariable != NULL) && (strcmp(searchedVariable->varName, diffVarName) == 0))
            break;

        printf("The function does not have variable \"%s\"\n", diffVarName);
        printf("Enter another name of variable: ");
    }

    return searchedVariable->varName;
}

void fprintfGnuplotHeaders(FILE* gnuplotFile) {
    assert(gnuplotFile);

    fprintf(gnuplotFile, "set terminal png size 800,600\n");
    fprintf(gnuplotFile, "set output 'TEX_DUMP/FUNC_GRAPHS/funcGraph%d.png'\n", graphsCounter);
    fprintf(gnuplotFile, "set xlabel 'x'\n");
    fprintf(gnuplotFile, "set ylabel 'y'\n");
    fprintf(gnuplotFile, "set grid\n");
    fprintf(gnuplotFile, "f(x) = ");
}

void fprintfGraphAtLatex(FILE* latexFile, const char* graphName) {
    assert(latexFile);
    assert(graphName);

    fprintf(latexFile, "\\begin{figure}[h]\n\t\\centering\n");
    fprintf(latexFile, "\t\\includegraphics[width=0.5\\textwidth]{FUNC_GRAPHS/funcGraph%d.png}\n", graphsCounter);
    fprintf(latexFile, "\t\\caption{%s}\n", graphName);
    fprintf(latexFile, "\\end{figure}\n");
}

