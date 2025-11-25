#ifndef DSL_H
#define DSL_H

#define NUM(value) newNodeCtor(tree, typeNumber, {.constValue = value}, NULL, NULL)

#define dL differentiateNode(tree, *nodeLeft(node), dumpInfo, diffVarName, latexFile)
#define dR differentiateNode(tree, *nodeRight(node), dumpInfo, diffVarName,  latexFile)
#define cL copyNode(tree, *nodeLeft(node))
#define cR copyNode(tree, *nodeRight(node))

#define ADD_(left, right) newNodeCtor(tree, typeOperator, {.opCode = opADD}, left, right)
#define SUB_(left, right) newNodeCtor(tree, typeOperator, {.opCode = opSUB}, left, right)
#define MUL_(left, right) newNodeCtor(tree, typeOperator, {.opCode = opMUL}, left, right)
#define DIV_(left, right) newNodeCtor(tree, typeOperator, {.opCode = opDIV}, left, right)
#define POW_(left, right) newNodeCtor(tree, typeOperator, {.opCode = opPOW}, left, right)

#define COMPOUND_FUNC(func) MUL_(func, dR)

#define SIN_(right) newNodeCtor(tree, typeOperator, {.opCode = opSIN}, NULL, right)
#define COS_(right) newNodeCtor(tree, typeOperator, {.opCode = opCOS}, NULL, right)
#define TG_(right) newNodeCtor(tree, typeOperator, {.opCode = opTG}, NULL, right)
#define CTG_(right) newNodeCtor(tree, typeOperator, {.opCode = opCTG}, NULL, right)

#define ARCSIN_(right) newNodeCtor(tree, typeOperator, {.opCode = opARCSIN}, NULL, right)
#define ARCCOS_(right) newNodeCtor(tree, typeOperator, {.opCode = opARCCOS}, NULL, right)
#define ARCTG_(right) newNodeCtor(tree, typeOperator, {.opCode = opARCTG}, NULL, right)
#define ARCCTG_(right) newNodeCtor(tree, typeOperator, {.opCode = opARCCTG}, NULL, right)

#define SH_(right) newNodeCtor(tree, typeOperator, {.opCode = opSH}, NULL, right)
#define CH_(right) newNodeCtor(tree, typeOperator, {.opCode = opCH}, NULL, right)
#define TH_(right) newNodeCtor(tree, typeOperator, {.opCode = opTH}, NULL, right)
#define CTH_(right) newNodeCtor(tree, typeOperator, {.opCode = opCTH}, NULL, right)

#define LN_(right) newNodeCtor(tree, typeOperator, {.opCode = opLN}, NULL, right)
#define LOG_(left, right) newNodeCtor(tree, typeOperator, {.opCode = opLOG}, left, right)
#define EXP_(right) newNodeCtor(tree, typeOperator, {.opCode = opEXP}, NULL, right)

#define $(diffNode) ({\
    node_t* diffResult = (diffNode);\
    fprintf(latexFile, "%s", getRandomPhrase());\
    fprintf(latexFile, "\\[ \\frac{d}{d%s}(", diffVarName);\
    fprintfNodeToLatex(tree, node, latexFile);\
    fprintf(latexFile, ") = ");\
    fprintfNodeToLatex(tree, diffResult, latexFile);\
    fprintf(latexFile, "\\]\n");\
    diffResult;\
})

//Taylor DSL: &taylorTree
#define ADD_T_(left, right) newNodeCtor(&taylorTree, typeOperator, {.opCode = opADD}, left, right)
#define MUL_T_(left, right) newNodeCtor(&taylorTree, typeOperator, {.opCode = opMUL}, left, right)
#define DIV_T_(left, right) newNodeCtor(&taylorTree, typeOperator, {.opCode = opDIV}, left, right)
#define POW_T_(left, right) newNodeCtor(&taylorTree, typeOperator, {.opCode = opPOW}, left, right)

#define VAR_T_(hash) newNodeCtor(&taylorTree, typeVariable, {.varHash = hash}, NULL, NULL)
#define NUM_T_(value) newNodeCtor(&taylorTree, typeNumber, {.constValue = value}, NULL, NULL)

#endif
