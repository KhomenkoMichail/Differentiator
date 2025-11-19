#ifndef TREE_FUNCTIONS_H
#define TREE_FUNCTIONS_H

#define SUBTREE_DUMP(subtreeRoot, dumpInfo, message) ({\
    tree_t subtree = {};\
    *treeRoot(&subtree) = subtreeRoot;\
    *treeSize(&subtree) = MAX_TREE_SIZE;\
    treeDump (&subtree, dumpInfo, message);\
})

//node_t* treeNodeCtor (const char* newObjectDescription); //FIXME

int fprintfTreeGraphDump (tree_t* tree, const char* textGraphFileName);

int fprintfNodeGraph (node_t* node, int rank, FILE* graphFile, size_t* nodesPassed, size_t treeSize);

int fprintfNodeLinksForGraph (node_t* node, FILE* graphFile, size_t* nodesPassed, size_t treeSize);

void treeDump (struct tree_t* tree, struct dump* dumpInfo, const char* message);

void createGraphImageForDump (struct tree_t* tree, FILE* dumpFile, const char* nameOfTextGraphFile);

//tree_t* akinatorTreeCtor(tree_t* tree, dump* dumpInfo);//FIXME

int nodeVerifier (node_t* node, int* errorCode, size_t* nodesPassed, size_t treeSize);

void fprintfTreeErrorsForDump (struct tree_t* tree, FILE* dumpFile, struct dump* dumpInfo);

int treeVerifier (tree_t* tree);

int deleteTree (tree_t* tree);

int deleteNode(node_t* node , size_t* nodesPassed, size_t treeSize);

#endif
