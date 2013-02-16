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
void IREmiter::emitBinOP(int inst, Local *left, Local *right, Local *result)
{
    
}
void IREmiter::emitException()
{
    
}
void IREmiter::emitLoad(Local *dst, Local *src)
{
    
}
void IREmiter::emitStore(Local *dst, Local *src)
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
void IREmiter::emitIfEqual(Local *val1, Local *val2, Label &falseLabel)
{
    
}



