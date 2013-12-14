//
//  IREmiter.cpp
//  A toyable language compiler (like a simple c++)
//
#include "tl-compile.h"
#include "tl-compile-option.h"
#include "tl-ir-emiter.h"
#include "tl-label.h"
#include "tl-value.h"

using namespace tlang;

IREmiter::IREmiter()
{
    m_isOutputAssembleFile = CompileOption::getInstance().isOutputAssembleFile();
    m_blocks = NULL;
    m_curBlock = NULL;
}

IREmiter::~IREmiter()
{
    m_asmFile.close();
}

void IREmiter::prepare()
{
    // open the assemble file
    string fullAssembleFile = m_path;
    fullAssembleFile += "/";
    fullAssembleFile += m_file;
    m_asmFile.open(fullAssembleFile.c_str(), ostream::out | ostream::app);
    if (!m_asmFile) 
        throw "file can not be created for asm file";
    // print section in asm file
    putasm("assemble file for %s", fullAssembleFile.c_str());
    putasm("===================================");
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
    m_asmFile << buf << endl;
    va_end(list);
}
// create tlang object file for current class 
void IREmiter::createTof(const string& name)
{
    // the name is class name, file extension should be added
    string tofFile = m_path;
    tofFile += "/";
    tofFile += name;
    tofFile += ".tof"; 
    m_tofFile.open(tofFile.c_str(), ofstream::out | ofstream::app); 
    if (!m_tofFile)
        throw "file can not be created";
    putasm("==================================");
    putasm("assemble code for class %s", name.c_str());
}

// close tlang object file for current class
void IREmiter::closeTof()
{
    m_tofFile.close();
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

// instruction without operand 
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
// instruction to single operator
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
// instruction with two operands
void IREmiter::emit(int inst, Value& op1, Value& op2)
{
    switch (inst) {
        case IR_LOAD:
            putasm("load %s, %s", op1.getName().c_str(), op2.getName().c_str()); 
            break;
        case IR_STORE:
            putasm("store %s, %s", op1.getName().c_str(), op2.getName().c_str()); 
            break;
        default:
            break;
    }
}

// instruction with three operands 
void IREmiter::emit(int inst, Value& left, Value& right, Value& result)
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
void IREmiter::emitException()
{}

void IREmiter::emitMethodCall(Value& val)
{
    putasm("call %s", val.getName().c_str());
}




