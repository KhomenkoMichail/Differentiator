#ifndef MATH_EXPRESSION_READER_H
#define MATH_EXPRESSION_READER_H

void syntaxError (char** bufPos);

node_t* getNumberNode (tree_t* tree, char** bufPos);

node_t* getVariableNode (tree_t* tree, char** bufPos);

node_t* getUnaryOperatorNode(tree_t* tree, char** bufPos);

node_t* getBracketExpressionNode(tree_t* tree, char** bufPos);

node_t* getMULandDIVnodes (tree_t* tree, char** bufPos);

node_t* getPowNodes (tree_t* tree, char** bufPos);

node_t* getADDandSUBnodes (tree_t* tree, char** bufPos);

node_t* getMathExpressionTree (tree_t* tree, char** bufPos);

#endif
