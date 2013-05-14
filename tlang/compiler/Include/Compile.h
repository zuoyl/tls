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
class Lexer;
class Parser;
class ASTBuilder;
class TypeBuilder;
class IRBuilder;


/// 'class CompileOption
/// for each compilation pass ,the compliation option is single only
class  CompileOption
{
public:
    void setCompileLevel(int level) { m_compileLevel = level; }
    int  getCompileLevel() { return m_compileLevel; }
    void setOutputFile(const string &output) { m_outputFile = output; }
    void setDebug(bool isDebug) { m_isDebug = isDebug; }
    bool getDebug() { return m_isDebug; }
    static CompileOption& getInstance() 
    {
        static CompileOption option;
        return option;
    }
private:
    CompileOption(){}
    ~CompileOption(){}

private:
    int m_compileLevel;
    string m_outputFile;
    bool m_isDebug;
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
    Lexer       *m_lexer;
    Parser      *m_parser;
    ASTBuilder  *m_astBuilder;
    TypeBuilder *m_typeBuilder;
    IRBuilder   *m_irBuilder;
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
    void parseOptions(const char *options);
    void compile(vector<string> &sourFiles);
    static CompileThread* getCurrentThread();
    static Compiler& getInstance();
private:
    Compiler(){}
    ~Compiler(){}
private:
    static map<int, CompileThread *>  m_theads;
};

LocationMgr* getCurrentLocationMgr();





#endif  // TCC_COMPILE_H


