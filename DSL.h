#ifndef DSL_H
#define DSL_H

#define dL differentiateNode(*nodeLeft(node), dumpInfo, diffVarName)
#define dR differentiateNode(*nodeRight(node), dumpInfo, diffVarName)
#define cL copyNode(*nodeLeft(node))
#define cR copyNode(*nodeRight(node))

#define ADD_(left, right) newNodeCtor(typeOperator, {.opCode = opADD}, left, right)
#define SUB_(left, right) newNodeCtor(typeOperator, {.opCode = opSUB}, left, right)
#define MUL_(left, right) newNodeCtor(typeOperator, {.opCode = opMUL}, left, right)
#define DIV_(left, right) newNodeCtor(typeOperator, {.opCode = opDIV}, left, right)


#endif
