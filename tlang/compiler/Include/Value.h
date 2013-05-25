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

class Value {
private:
    enum { UnknowV, IntV, FloatV, StringV, RefV };
    
public:
    Value(){ m_vtype = UnknowV;}
    Value(bool inreg){ 
        m_vtype = IntV;
        m_inreg = inreg;
    }
    Value(IRRegister reg) {
        m_vtype = IntV;
        m_inreg = true;
        m_regidx = reg;
    }
    Value(bool inreg, int v) {
        m_vtype = IntV;
        m_inreg = inreg;
        m_intValue = v;
    } 
    Value(bool inreg, float v){
        m_vtype = FloatV;
        m_floatValue = v;
        m_inreg = inreg;
    }
    Value(bool inreg, string &v){
        m_inreg = inreg;
        m_vtype = StringV;
        m_stringValue = v;
    }
    ~Value(){}

    Value& operator = (Value &rhs) {
        m_vtype = rhs.m_vtype;
        m_inreg = rhs.m_inreg;
        m_regidx = rhs.m_regidx;
        m_intValue = rhs.m_intValue;
        m_floatValue = rhs.m_floatValue;
        m_stringValue = rhs.m_stringValue;
        return *this;
    }
	bool isValid() const { return (m_vtype != UnknowV);  }
	bool isInReg() const { return m_inreg; }
    bool isConst() const { return m_isConst; }
    void setConst(bool isConst) { m_isConst = isConst; }
    void initializeWithType(Type *type){}
  
private:
    bool m_inreg;    /// wether the value is in register(true, or false)
    int  m_vtype;    /// value type, IntV...
    int  m_regidx;   /// if the value is in register, the register index
    bool m_isConst;  /// wether the value is const
    int  m_intValue;
    string m_stringValue;
    float m_floatValue;
friend class IREmiter;

};

#endif // TCC_VALUE_H
