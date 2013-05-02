//
//  IRNodeBlock.h
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_IRNODEBLOCK_H
#define TCC_IRNODEBLOCK_H

#include "compiler/Common.h"

class IRNode {
public:
    IRNode(){}
    ~IRNode(){}
};

class IRBlock {
public:
    IRBlock(){}
    ~IRBlock(){}
};

typedef vector<IRBlock *> IRBlockList;


#endif // TCC_IRNODEBLOCK_H
