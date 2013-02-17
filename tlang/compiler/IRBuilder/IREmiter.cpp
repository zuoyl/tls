//
//  IREmiter.cpp
//  A toyable language compiler (like a simple c++)
//

#include "IREmiter.h"


IREmiter* IREmiter::getInstance()
{
    static IREmiter emiter;
    return &emiter;
}

void IREmiter::setIRBlockList(IRBlockList *list)
{
    if (!m_blocks)
        m_blocks = list;
}

void IREmiter::emitLabel(Label &label)
{
    
}
void IREmiter::emitBinOP(int inst, Value *left, Value *right, Value *result)
{
    
}
void IREmiter::emitException()
{
    
}
void IREmiter::emitLoad(Value *dst, Value *src)
{
    
}
void IREmiter::emitStore(Value *dst, Value *src)
{
    
}
void IREmiter::emit(int inst)
{
    
}
void IREmiter::emit(int inst, const string &name)
{
    
}

void IREmiter::emit(int inst, const string &target, const string& src)
{
    
}
void IREmiter::emitIfEqual(Value *val1, Value *val2, Label &falseLabel)
{
    
}



