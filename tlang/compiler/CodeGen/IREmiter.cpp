//
//  IREmiter.cpp
//  A toyable language compiler (like a simple c++)
//
#include "Compile.h"
#include "IREmiter.h"

IREmiter::IREmiter()
{
    m_isOutputAssembleFile = CompileOption::getInstance().isOutputAssembleFile();
}

IREmiter::~IREmiter()
{
}
