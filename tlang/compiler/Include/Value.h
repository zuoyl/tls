//
//  Value.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_VALUE_H
#define TCC_VALUE_H

#include "Common.h"
#include "Type.h"
#include "BuiltinType.h"
#include "IREmiter.h"

/// 'class Value
/// Value is an object used for locals in frame/stack

enum ValueType {
      VT_INREG,
      VT_INFRAME,
      VT_CINT,
      VT_CSTRING, 
};

class Value {
public:
    Value(){}
    Value(bool inreg){}
    Value(IRRegister reg) {}
    Value(bool inreg, int v){} 
    Value(bool inreg, float v){}
    Value(bool inreg, string &v){}
    ~Value(){}
	
	bool isValid() const { return true; }
    ValueType getVType()const { return m_vtype;}
    Type* getType() const { return m_type; }
	bool isInReg() const { return m_inreg; }
    bool isConst() const { return (m_vtype == VT_CINT || m_vtype == VT_CSTRING); }
	int getSize() const { return m_size; }
  
    void initializeWithType(Type *type){}
  
private:
    int m_offset;
    int m_size;
    Type *m_type;
    ValueType m_vtype;
	bool m_inreg;
	int m_regidx;
};

#endif // TCC_VALUE_H
