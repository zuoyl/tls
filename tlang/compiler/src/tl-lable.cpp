//
//  Label.cpp
//  A toyable language compiler (like a simple c++)
//

#include "tl-common.h"
#include "tl-label.h"

using namespace tl;

int Label::m_index = 0;

Label::Label(const char* name)
{
    if (name)
        m_name = name;
}


Label Label::newLabel()
{
    m_index++;
    string labelName = "label_";
    labelName += m_index;
    return Label(labelName);
}

const string& 
Label::getName()
{
    return m_name;
}
