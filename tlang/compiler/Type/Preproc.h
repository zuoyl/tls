//
//  Preproc.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_PREPROC_H
#define TCC_PREPROC_H

#include "Type.h"

// Preproc - class to preprocess the .th file to get class information
class Preproc  
{
public:
    Preproc(const string& path, const string& file);
    ~Preproc();
    // build - parse the .th file and get class information
    // @para typeDomain type information declared in .th file 
    void build(TypeDomain &typeDomain);
    
private:
    string m_file;
    string m_path;
};

#endif // TCC_PREPROC_H
