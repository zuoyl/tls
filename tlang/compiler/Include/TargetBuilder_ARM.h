//
//  CodeGenerator_ARM.h
//  A toyable language compiler (like a simple c++)
//
//  Created by apple on 12-4-17.
//  Copyright 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef CODEGENERATOR_ARM_H
#define CODEGENERATOR_ARM_H

#include "TargetBuilder.h"

class CodeGenerator_ARM : public CodeGenerator {
public:
    CodeGenerator_ARM():CodeGenerator("arm"){}
    ~CodeGenerator_ARM(){}
    
};
#endif // CODEGENERATOR_ARM_H
