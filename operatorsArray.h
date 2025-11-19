struct operatorInfo operatorsArray[] ={ {"+", opADD},
                                    {"-", opSUB},
                                    {"*", opMUL},
                                    {"\\", opDIV},
                                    {"^", opPOW},
                                    {"sin", opSIN},
                                    {"cos", opCOS} };

const size_t NUM_OF_OPERATORS = sizeof(operatorsArray) / sizeof(operatorInfo);
