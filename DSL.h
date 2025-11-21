#ifndef DSL_H
#define DSL_H

#define NUM(value) newNodeCtor(typeNumber, {.constValue = value}, NULL, NULL)

#define dL differentiateNode(tree, *nodeLeft(node), dumpInfo, diffVarName, latexFile)
#define dR differentiateNode(tree, *nodeRight(node), dumpInfo, diffVarName,  latexFile)
#define cL copyNode(*nodeLeft(node))
#define cR copyNode(*nodeRight(node))

#define ADD_(left, right) newNodeCtor(typeOperator, {.opCode = opADD}, left, right)
#define SUB_(left, right) newNodeCtor(typeOperator, {.opCode = opSUB}, left, right)
#define MUL_(left, right) newNodeCtor(typeOperator, {.opCode = opMUL}, left, right)
#define DIV_(left, right) newNodeCtor(typeOperator, {.opCode = opDIV}, left, right)
#define POW_(left, right) newNodeCtor(typeOperator, {.opCode = opPOW}, left, right)

#define COMPOUND_FUNC(func) MUL_(func, dR)

#define SIN_(right) newNodeCtor(typeOperator, {.opCode = opSIN}, NULL, right)
#define COS_(right) newNodeCtor(typeOperator, {.opCode = opCOS}, NULL, right)
#define TG_(right) newNodeCtor(typeOperator, {.opCode = opTG}, NULL, right)
#define CTG_(right) newNodeCtor(typeOperator, {.opCode = opCTG}, NULL, right)

#define ARCSIN_(right) newNodeCtor(typeOperator, {.opCode = opARCSIN}, NULL, right)
#define ARCCOS_(right) newNodeCtor(typeOperator, {.opCode = opARCCOS}, NULL, right)
#define ARCTG_(right) newNodeCtor(typeOperator, {.opCode = opARCTG}, NULL, right)
#define ARCCTG_(right) newNodeCtor(typeOperator, {.opCode = opARCCTG}, NULL, right)

#define SH_(right) newNodeCtor(typeOperator, {.opCode = opSH}, NULL, right)
#define CH_(right) newNodeCtor(typeOperator, {.opCode = opCH}, NULL, right)
#define TH_(right) newNodeCtor(typeOperator, {.opCode = opTH}, NULL, right)
#define CTH_(right) newNodeCtor(typeOperator, {.opCode = opCTH}, NULL, right)

#define LN_(right) newNodeCtor(typeOperator, {.opCode = opLN}, NULL, right)
#define LOG_(left, right) newNodeCtor(typeOperator, {.opCode = opLOG}, left, right)
#define EXP_(right) newNodeCtor(typeOperator, {.opCode = opEXP}, NULL, right)

#define $(diffNode) ({\
    node_t* diffResult = (diffNode);\
    fprintf(latexFile, "%s", getRandomPhrase());\
    fprintf(latexFile, "\\[ \\frac{d}{d%s}(", diffVarName);\
    fprintfNodeToLatex(tree, node, NULL, latexFile);\
    fprintf(latexFile, ") = ");\
    fprintfNodeToLatex(tree, diffResult, NULL, latexFile);\
    fprintf(latexFile, "\\]\n");\
    diffResult;\
})

#endif
