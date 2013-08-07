//
//  Value.cpp
//  A toyable language compiler (like a simple c++)
//
#include "Value.h"

const string& Value::getName()
{
    char buf[32] = {'0'}; 
    // wether the value is in register
    if (m_inreg) {
        if (m_regidx == IR_INVALID_REG) { // the register is not alocated
            // how to allocate register 
        }
        else {
            sprintf(buf, "R%d", m_regidx);
        }
    }
    // the value is not in register 
    else {
        switch (m_vtype) {
            case IntV:
                sprintf(buf, "%d", m_intValue);
                break;
            case FloatV:
                sprintf(buf, "%f", m_floatValue); 
                break;
            case StringV:
                sprintf(buf, "%s", m_stringValue.c_str()); 
                break;
            case RefV:
                break;
            default:
                break;
        }
    }
    m_name = buf; 
    return m_name;
}
