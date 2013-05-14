////
/// Compile.cpp 
///  A toyable language compiler (like a simple c++)

#include "Compile.h"
#include "TokenStream.h"
#include "Lexer.h"
#include "Parser.h"
#include "Scope.h"
#include "ASTBuilder.h"
#include "TypeBuilder.h"
#include "IRBuilder.h"


/// CompileUnit Implementation

/// CompilerUnit constructor
CompileUnit::CompileUnit(const string &path, const string &file)
    :m_sourcePath(path), m_sourceFile(file)
{
    m_locationMgr = new LocationMgr();
    m_locationMgr->setCompileSourceFile(file);
    m_locationMgr->setCompileSourcePath(path);
    m_tokenStream = new TokenStream();
    m_lexer = new Lexer(path, file);
    m_parser = new Parser;
    m_astBuilder = new ASTBuilder;
    m_typeBuilder = new TypeBuilder;
    m_irBuilder = new IRBuilder;
}


/// CompileUnit::destructor
CompileUnit::~CompileUnit()
{
    delete m_irBuilder;
    delete m_typeBuilder;
    delete m_astBuilder;
    delete m_parser;
    delete m_lexer;
    delete m_tokenStream;
    delete m_locationMgr;
}   

/// build the source file specified by compile thread
bool CompileUnit::build()
{
    Grammar::getInstance()->build("grammar.txt");
    m_lexer->parse(m_tokenStream);
    
    // create the parse tree
    Node * parseTree = m_parser->parse(m_tokenStream);
    
    // create the AST
    AST *ast = m_astBuilder->build(parseTree);
    
    // build the type and scope
    m_typeBuilder->build(ast);
    if (m_typeBuilder->isBuildComplete()) {
        // build the ir code
        IRBlockList blocks;
        m_irBuilder->build(ast, &blocks);
        return true;
    }
    else {
        return false;
    }
}


/// CompileThread Implementation

/// CompileThread constructor
CompileThread::CompileThread(const string &path, const string &file)
{
    m_compileUnit = new CompileUnit(path, file);
    m_threadID = -1;
}

/// CompileThread destructor
CompileThread::~CompileThread()
{
    // stop the thead
    
    delete m_compileUnit;
    m_compileUnit = NULL;
    m_threadID = -1;
}
/// start the compile thread
void CompileThread::start()
{
   // start the thread 
}


/// Compiler Implementation
static map<int, CompileThread *> m_threads;

/// Singleton
Compiler& Compiler::getInstance()
{
    static Compiler compiler;
    return compiler;
}

/// parse the compile options
void Compiler::parseOptions(const char *options)
{
    CompileOption &compileOption = CompileOption::getInstance();
    // parse the options and set it
}

/// compile the source files
void Compiler::compile(vector<string> &sourceFile)
{
    vector<string>::iterator ite = sourceFile.begin();
    for (; ite != sourceFile.end(); ite++) {
        string &sourceFile = *ite;
        string sourcePath ="";
        CompileThread *compileThread = new CompileThread(sourcePath, sourceFile);
        // insert the compile thread into m_thread
        m_threads[compileThread->getThreadID()] = compileThread;
        compileThread->start();
    }
}

CompileThread* Compiler::getCurrentThread()
{
#if 0
    int threaid = pthread_get_thread_id();
    if (m_threads.find(threadid) != m_threads.end())
        return m_threads[threadid];
    else
        return NULL;
#else
        return NULL; // temp
#endif
}


