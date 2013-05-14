//
//  IREmiter.h - IR instruction emiiter
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_IREMITER_H
#define TCC_IREMITER_H

#include "Common.h"
#include "IRNodeBlock.h"

class Label;
class Value;


enum {
    IR_INVALID = -1,
    IR_LABEL, 
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
    static void setIRBlockList(IRBlockList *list){}
    static void emit(int inst){}
    static void emit(int inst, const string &target){}
    static void emit(int inst, const string &target, const string &source){}
    
    static void emitLabel(Label &label){}
    static void emitBinOP(int inst, Value *left, Value *right, Value *result){}
    static void emitException(){}
    static void emitLoad(Value *dst, Value *src){}
    static void emitStore(Value *dst, Value *src){}
    static void emitIfEqual(Value *val1, Value *val2, Label &falseLabel){}
	static void emitCMP(Value *val1, Value *val2, Label &trueLabel, Label &falseLabel){}
	static void emitCMP(Value *val1, int val2, Label &trueLabel, Label &falseLabel){}
	static void emitCMP(Value *val1, string &val2, Label &falseLabel){}
    static void emitJump(Label &lable){}
private:
    IREmiter(){}
    ~IREmiter(){}
    static IRBlockList *m_blocks;
};


#endif // endif TCC_IREMITER_H
