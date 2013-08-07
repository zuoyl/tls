//
//  IREmiter.cpp
//  A toyable language compiler (like a simple c++)
//
#include "Compile.h"
#include "IREmiter.h"
#include "Label.h"
#include "Value.h"

IREmiter::IREmiter()
{
    m_isOutputAssembleFile = CompileOption::getInstance().isOutputAssembleFile();
    m_blocks = NULL;
    m_curBlock = NULL;
}

IREmiter::~IREmiter()
{
}

void IREmiter::putbyte(unsigned long w)
{
    m_tofFile << w;
}

void IREmiter::putasm(const char* fmt, ...)
{
    char buf[255] = { 0 };
    va_list list;
 
    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    m_asmFile << buf << std::endl;
    va_end(list);
}
// create tlang object file for current class 
void IREmiter::createTof(const string& name)
{
    // the name is class name, file extension should be added
    m_curTofFile += m_path;
    m_curTofFile += "/";
    m_curTofFile = name;
    m_curTofFile += ".tof"; 
    m_tofFile.open(m_curTofFile.c_str(), std::ofstream::out | std::ofstream::app); 
    if (!m_tofFile)
        throw "file can not be created";
    
    // create the assemble file for class 
    string asmFile = m_path;
    asmFile += "/";
    asmFile += name;
    asmFile += ".asm";
    m_asmFile.open(asmFile.c_str(), std::ostream::out | std::ostream::app);
    if (!m_asmFile) {
        m_tofFile.close();
        throw "file can not be created for asm file";
    }
    // print section in asm file
    putasm("assemble file for %s", name.c_str());
    putasm("===================================");

}

// close tlang object file for current class
void IREmiter::closeTof()
{
    m_tofFile.close();
    m_curTofFile = "";
    m_asmFile.close();
}

// create a new code block and insert it into blocklist
void IREmiter::emitBlock()
{
   IRBlock* block = new IRBlock();
   m_curBlock = block;
   m_blocks->push_back(block);
}

// emit a label in asm file
void IREmiter::emitLabel(Label& label)
{
    putasm("%s:", label.getName().c_str());
}

// instruction emiter
void IREmiter::emit(int inst)
{
    switch (inst) {
        case IR_RET:
            putasm("ret");
            break;
        case IR_NOP:
            putasm("nop");
            break;
        default:
            break;
    }
}
// instruction to load src into dst
void IREmiter::emitLoad(Value& dst, Value& src)
{
    putasm("load %s, %s", dst.getName().c_str(), src.getName().c_str()); 
}
void IREmiter::emitStore(Value& dst, Value& src)
{
   putasm("store %s, %s", dst.getName().c_str(), src.getName().c_str()); 
}

// instruction for binary operator
void IREmiter::emitBinOP(int inst, Value& left, Value& right, Value& result)
{
    const char* lhs = left.getName().c_str();
    const char* rhs = right.getName().c_str();
    const char* rst = result.getName().c_str();

    switch (inst) {
        case IR_ADD:
            putasm("add %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_SUB:
            putasm("sub %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_MUL:
            putasm("mul %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_DIV:
            putasm("div %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_MODULO:
            putasm("modulo %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_LSHIFT:
            putasm("lshift %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_RSHIFT:
            putasm("rshift %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_BIT_OR:
            putasm("or %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_BIT_XOR:
            putasm("xor %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_BIT_AND:
            putasm("and %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_EQ:
            putasm("eq %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_NEQ:
            putasm("neq %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_LT:
            putasm("lt %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_LTEQ:
            putasm("lteq %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_GT:
            putasm("gt %s, %s, %s", lhs, rhs, rst);
            break;
        case IR_GTEQ:
            putasm("gteq %s, %s, %s", lhs, rhs, rst);
            break;
        
        default:
            break;
    }
}
// instruction to singal operator
void IREmiter::emit(int inst, Value& val)
{
    switch (inst) {
        case IR_CALL:
            putasm("call %s", val.getName().c_str()); 
            break;
        case IR_JMP:
            putasm("jmp %s", val.getName().c_str()); 
            break;
        case IR_PUSH:
            putasm("push %s", val.getName().c_str()); 
            break;
        case IR_INC:
            putasm("inc %s", val.getName().c_str()); 
            break;
        case IR_DEC:
            putasm("dec %s", val.getName().c_str()); 
            break;
        default:
            break;
    }
}
// instruction for jump
void IREmiter::emitJump(Label& label)
{
    putasm("jmp %s", label.getName().c_str());
}
// instruction for cmpare
void IREmiter::emitCMP(Value& val1, Value& val2, Label& trueLabel, Label& falseLabel)
{
    putasm("cmp %s, %s", val1.getName().c_str(), val2.getName().c_str());
    putasm("jmpz %s", trueLabel.getName().c_str());
    putasm("jmp %s", falseLabel.getName().c_str());
}
void IREmiter::emitCMP(Value& val1, int val2, Label& trueLabel, Label& falseLabel)
{
    putasm("cmp %s, %d", val1.getName().c_str(), val2);
    putasm("jmpz %s", trueLabel.getName().c_str());
    putasm("jmp %s", falseLabel.getName().c_str());
}
void IREmiter::emit(int inst, Value& val1, Value& val2)
{}
void IREmiter::emit(int inst, Value& val1, Value& val2, Value& val3)
{} 

void IREmiter::emitException()
{}

void IREmiter::emitMethodCall(Value& val)
{
    putasm("call %s", val.getName().c_str());
}




