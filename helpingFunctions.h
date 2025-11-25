#ifndef HELPING_FUNCTIONS_H
#define HELPING_FUNCTIONS_H

    char* copyFileContent (const char* nameOfFile);

    unsigned int getSizeOfFile (int fileDescriptor);

    void skipSpaces (char** bufPos);

    int bsearchHashComparator(const void* firstParam, const void* secondParam);

    unsigned long long getStringHash (const char* string);

    int structVariableComparator (const void* firstStruct, const void* secondStruct);

    int compareDouble (double first, double second);

    size_t getSize_t (void);

    unsigned long long getFactorial(size_t num);
#endif
