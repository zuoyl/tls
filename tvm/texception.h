/// tvm
/// virtual machine for toyable language

#ifndef TVM_EXCEPTION_H
#define TVM_EXCEPTION_H

#include <exception.h>
#include "tvm.h"
#include "instrut.h"

class InvalidInstruct : public std::exception {
public: 
    InvlaidInstruct(Instruct &inst):m_inst(inst){}
    ~InvalidInstruct(){}
    Instruct &getInstruct() { 
        return m_instruct; 
    }
private:
    Instruct m_inst;
};

class InvalidMemAccess : public std::exception {
public:
    InvalidMemAccess(u32 address):m_address(address) {}
    ~InvalidMemAccess(){}
    u32 getAddress() {
        return m_address;
    }
private:
    u32 m_address;
};


#endif 