//
//  Value.cpp
//  A toyable language compiler (like a simple c++)
//

#include "compiler/Value.h"
#include "compiler/Type.h"

Value::Value()
{
    m_addr = 0;
    m_size = 0;
}

Value::Value(int addr, int size)
{
    m_addr = addr;
    m_size = size;
}

Value::~Value()
{}

