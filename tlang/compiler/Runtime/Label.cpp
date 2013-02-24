//
//  Label.cpp
//  A toyable language compiler (like a simple c++)
//

#include "compiler/Common.h"
#include "compiler/Label.h"

int Label::m_index = 0;

Label Label::newLabel()
{
    m_index++;
    string labelName = "label";
    labelName += m_index;
    return Label(labelName.c_str());
}

const string& Label::getName()
{
    return m_name;
}