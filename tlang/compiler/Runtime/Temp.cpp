//
//  Local.cpp
//  A toyable language compiler (like a simple c++)
//

#include "Local.h"
#include "Type.h"

Local::Local()
{
    m_addr = 0;
    m_size = 0;
}

Local::Local(int addr, int size)
{
    m_addr = addr;
    m_size = size;
}

Local::~Local()
{}

