////
/// Compile.h 
/// compiler unit to manger lexer, parser and other components
/// to improve the compile performance, maybe use the mulitthread to compile,
/// in future, each thread will contain a compile unit
///
///  A toyable language compiler (like a simple c++)

#ifndef TCC_COMPILE_H
#define TCC_COMPILE_H

#include "Common.h"

class TokenStream;
class LocationMgr;
class SimpleLexer;
class Parser;
class ASTBuilder;
class TypeBuilder;
class IRBuilder;
class TypeDomain;

/// 'class CompileOption
/// for each compilation pass ,the compliation option is single only
class  CompileOption
{
public:
    void setCompileLevel(int v) { m_compileLevel = v; }
    int  getCompileLevel() { return m_compileLevel; }
    void setDebug(bool v) { m_isDebug = v; }
    bool getDebug() { return m_isDebug; }
    bool isOutputAssembleFile() { return m_isOutputAssembleFile; }
    void setOutputAssembleFile(bool v) { m_isOutputAssembleFile = v; }
    bool isOutputParseTree() { return m_isOutputParseTree; }
    void setOutputParseTree(bool v) { m_isOutputParseTree = v; }
    bool isOutputAST() { return m_isOutputAST; }
    void setOutputAST(bool v) { m_isOutputAST = v; }
    
    static CompileOption& getInstance(); 
private:
    CompileOption();
    ~CompileOption();

private:
    int m_compileLevel;
    bool m_isDebug;
    bool m_isOutputAssembleFile;
    bool m_isOutputAST;
    bool m_isOutputParseTree;

};

/// 'class CompileUnit
/// each compile unit include a source file to be compile 
/// for multithread to improve performance
class CompileUnit
{
public:
    CompileUnit(const string &path, const string &file);
    ~CompileUnit();
    bool build();
    const LocationMgr* getLocationMgr() { return m_locationMgr;} 
private:
    LocationMgr *m_locationMgr;
    TokenStream *m_tokenStream;
    SimpleLexer *m_lexer;
    Parser      *m_parser;
    ASTBuilder  *m_astBuilder;
    TypeBuilder *m_typeBuilder;
    IRBuilder   *m_irBuilder;
    TypeDomain  *m_typeDomain;
    string m_sourcePath;
    string m_sourceFile;
};

class CompileThread
{
public:
    CompileThread(const string &path, const string &file);
    ~CompileThread();
    void start();
    int getThreadID() { return m_threadID; }
    CompileUnit* getCompileUnit() { return m_compileUnit; }
    static void ComileThreaProc(CompileUnit *compileUnit);

private:
    CompileUnit *m_compileUnit;
    int m_threadID;
};


class Compiler 
{
public:
     enum { CompileMaxThreads = 10 };
public:
    void compile(vector<string> &sourFiles);
    static CompileThread* getCurrentThread();
    static Compiler& getInstance();
private:
    Compiler(){}
    ~Compiler(){}
private:
    static map<int, CompileThread *>  m_theads;
};

LocationMgr* getLocationMgr();


#endif  // TCC_COMPILE_H


