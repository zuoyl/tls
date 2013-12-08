//
//  IRNodeBlock.h
//  A toyable language compiler (like a simple c++)
//


#ifndef __TL_IR_NODEBLOCK_H__
#define __TL_IR_NODEBLOCK_H__

#include "tl-common.h"

namespace tlang {
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

} // namespace tlang 
#endif // __TL_IR_NODEBLOCK_H__
