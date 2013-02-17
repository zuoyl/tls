//
//  Value.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_VALUE_H
#define TCC_VALUE_H

#include "Common.h"
#include "Runtime/Type.h"
#include "Runtime/BuiltinType.h"

/// 'class Value
/// Value is an object used for locals in frame/stack

enum ValueTYpe {
      VT_INREG,
      VT_INFRAME,
      VT_CINT,
      VT_CSTRING, 
};

class Value {
public:
    Value(ValueType type, int v, bool inreg = true);
    Value(ValueType type, float v, bool inreg = true);
    Value(ValueType type, string &v, bool inreg = true);
    ~Value();
	
	bool isValid() const { return true; }
    ValueType getType()const { return m_type;}
    Type* getType() const;
	bool isInReg() const { return m_inreg; }
    bool isConst() const { return (m_vtype == VT_CINT || m_vtype == VT_CSTRING); }
	int getSize() const { return m_size; }
  
    void initializeWithType(Type *type);
  
private:
    int m_offset;
    int m_size;
    Type *m_type;
    ValueType m_vtype;
	bool m_inreg;
	int m_regidx;
};

#endif // TCC_VALUE_H
