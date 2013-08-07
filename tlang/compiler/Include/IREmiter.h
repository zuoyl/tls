//
//  IREmiter.h - IR instruction emiter
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_IREMITER_H
#define TCC_IREMITER_H

#include "Common.h"
#include "IRNodeBlock.h"

class Label;
class Value;

enum IRRegister
{
    IR_INVALID_REG = -1,
    IR_R0,
    IR_R1,
    IR_R2,
    IR_R3,
    IR_R4,
    IR_R5,
    IR_R6,
    IR_R7,
    IR_R8,
    IR_R9,
    IR_R10,
    IR_R11,
    IR_PC,
    IR_SP,
    IR_BP,
    IR_LR,
    IR_MAX_REG
};


enum IRInstructType
{
    IR_INVALID = -1,
    IR_LABEL, 
    IR_INC,
    IR_DEC,
    IR_LOAD,
    IR_STORE,
    IR_MOV,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_MODULO,
    IR_LSHIFT,
    IR_RSHIFT,
    IR_CALL,
    IR_CMP,
    IR_JMP,
    IR_PUSH,
    IR_POP,
    IR_RET,
    IR_BR,
    IR_IFEQ,
    IR_IFNEQ,
    IR_JUMP,
    IR_LOGIC_OR,
    IR_LOGIC_AND,
    IR_OR,
    IR_AND,
    IR_BIT_OR,
    IR_BIT_XOR,
    IR_BIT_AND,
    IR_EQ,
    IR_NEQ,
    IR_GTEQ,
    IR_LTEQ,
    IR_GT,
    IR_LT,
    IR_RAISE_EXCEPT,
    IR_MAX
};

class IREmiter 
{
public:
    IREmiter();
    ~IREmiter();
    void createTof(const string& name); 
    void closeTof();
    void setAssembleFile(const string& file){ m_file = file; } 
    void setIRBlockList(IRBlockList* blockList){ m_blocks = blockList; }
    void emitBlock(); 
    void emit(int inst);
    void emit(int inst, const string& target);
    void emit(int inst, const string& target, const string& source);
    void emit(int inst, Value& val) ;
    void emit(int inst, Value& val1, Value& val2);
    void emit(int inst, Value& val1, Value& val2, Value& val3); 
    void emitLabel(Label& label);
    void emitBinOP(int inst, Value& left, Value& right, Value& result);
    void emitException();
    void emitLoad(Value& dst, Value& src);
    void emitStore(Value& dst, Value& src);
    void emitIfEqual(Value& val1, Value& val2, Label& falseLabel);
    void emitCMP(Value& val1, Value& val2, Label& trueLabel, Label& falseLabel);
    void emitCMP(Value& val1, int val2, Label& trueLabel, Label& falseLabel);
    void emitCMP(Value& val1, string& val2, Label& falseLabel);
    void emitJump(Label& lable);
    void emitPush(Value& val);
    void emitPop();
    void emitMethodCall(Value& val);
private:
    void put(unsigned long w);
private:
    bool m_isOutputAssembleFile;
    string m_file; 
    IRBlockList* m_blocks;
    string m_curTofFile;
    ofstream m_tofFile;
    IRBlock* m_curBlock;
};


#endif // endif TCC_IREMITER_H
