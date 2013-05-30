//
//  Preproc.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_PREPROC_H
#define TCC_PREPROC_H

#include "AST.h"
#include "ASTVistor.h"
#include "Scope.h"

// Preproc - class to preprocess the .th file to get class information
class Preproc : public ASTVisitor 
{
public:
    Preproc(const string&path, const string& file);
    ~Preproc();
    // build - parse the .th file and get class information
    // @para file the .th source file
    // @para path the .th source file path
    // @para clsmap class information declared in .th file 
    void build(map<string, AST*> &clsmap);
    
private:
    string m_file;
    string m_path;
    map<string, Class*> *m_clsmap;
};

#endif // TCC_PREPROC_H
