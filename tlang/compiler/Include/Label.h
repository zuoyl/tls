//
//  Label.h
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_LABEL_H
#define TCC_LABEL_H

#include "Common.h"

class Label 
{
public:
    Label(const char* name = NULL);
    Label(const string &name):m_name(name){}
    ~Label(){}
    static Label newLabel();
    const string& getName();
    
private:
    string m_name;
    static int m_index;
    
};


#endif
