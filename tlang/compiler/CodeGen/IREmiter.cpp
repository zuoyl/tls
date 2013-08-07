//
//  IREmiter.cpp
//  A toyable language compiler (like a simple c++)
//
#include "Compile.h"
#include "IREmiter.h"

IREmiter::IREmiter()
{
    m_isOutputAssembleFile = CompileOption::getInstance().isOutputAssembleFile();
    m_blocks = NULL;
    m_curBlock = NULL;
}

IREmiter::~IREmiter()
{
}

void IREmiter::put(unsigned long w)
{
    m_tofFile << w;
}

// create tlang object file for current class 
void IREmiter::createTof(const string& name)
{
    // the name is class name, file extension should be added
    m_curTofFile = name;
    m_curTofFile += ".tof"; 
    m_tofFile.open(m_curTofFile.c_str(), std::ofstream::out|std::ofstream::app); 
    if (!m_tofFile)
        throw "file can not be created";
}

// close tlang object file for current class
void IREmiter::closeTof()
{
    m_tofFile.close();
    m_curTofFile = "";
}

// create a new code block and insert it into blocklist
void IREmiter::emitBlock()
{
   IRBlock* block = new IRBlock();
   m_curBlock = block;
   m_blocks->push_back(block);
}

// instruction emiter
void IREmiter::emit(int inst)
{

}

void IREmiter::emit(int inst, const string& target)
{}
void IREmiter::emit(int inst, const string& target, const string& source)
{}
void IREmiter::emit(int inst, Value& val)
{}
void IREmiter::emit(int inst, Value& val1, Value& val2)
{}
void IREmiter::emit(int inst, Value& val1, Value& val2, Value& val3)
{} 
void IREmiter::emitLabel(Label& label)
{}
void IREmiter::emitBinOP(int inst, Value& left, Value& right, Value& result)
{}
void IREmiter::emitException()
{}
void IREmiter::emitLoad(Value& dst, Value& src)
{}
void IREmiter::emitStore(Value& dst, Value& src)
{}
void IREmiter::emitIfEqual(Value& val1, Value& val2, Label& falseLabel)
{}
void IREmiter::emitCMP(Value& val1, Value& val2, Label& trueLabel, Label& falseLabel){}
void IREmiter::emitCMP(Value& val1, int val2, Label& trueLabel, Label& falseLabel)
{}
void IREmiter::emitCMP(Value& val1, string& val2, Label& falseLabel)
{}
void IREmiter::emitJump(Label& lable)
{}
void IREmiter::emitPush(Value& val)
{}
void IREmiter::emitPop()
{}
void IREmiter::emitMethodCall(Value& val)
{}




