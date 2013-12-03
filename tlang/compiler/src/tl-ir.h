//
//  IR.h - IR definitions 
//  A toyable language compiler (like a simple c++)
//

#ifndef TCC_IR_H
#define TCC_IR_H

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
    IR_R12,
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
    IR_NOP,
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
    IR_NEG, 
    IR_RAISE_EXCEPT,
    IR_MAX
};


#endif

