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

/// CompileOption
CompileOption::CompileOption()
{
    m_compileLevel = 1;
    m_isDebug = true;
    m_isOutputAssembleFile = false;
    m_isOutputAST = false;
    m_isOutputParseTree = false;
}
CompileOption::~CompileOption()
{}

CompileOption& CompileOption::getInstance() 
{
    static CompileOption option;
    return option;
}
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
    m_parser = new Parser(path, file);
    m_astBuilder = new ASTBuilder(path, file);
    m_typeBuilder = new TypeBuilder(path, file);
    m_irBuilder = new IRBuilder(path, file);
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
    if (m_compileUnit)
        m_compileUnit->build();
}


/// Compiler Implementation
static map<int, CompileThread *> m_threads;

/// Singleton
Compiler& Compiler::getInstance()
{
    static Compiler compiler;
    return compiler;
}


/// compile the source files
void Compiler::compile(vector<string> &sourceFile)
{
    vector<string>::iterator ite = sourceFile.begin();
    for (; ite != sourceFile.end(); ite++) {
        string &fullFile = *ite;
        unsigned found = fullFile.find_last_of("/\\");
        string sourcePath = fullFile.substr(0, found);
        string sourceFile = fullFile.substr(found + 1);
        CompileThread *compileThread = new CompileThread(sourcePath, sourceFile);
        // insert the compile thread into m_thread
        m_threads[compileThread->getThreadID()] = compileThread;
        compileThread->start();
    }
}

CompileThread* Compiler::getCurrentThread()
{
#ifdef MTHREAD_COMPILE
    // dummy now
    int threaid = pthread_get_thread_id();
    if (m_threads.find(threadid) != m_threads.end())
        return m_threads[threadid];
    else
        return NULL;
#else
        return NULL; // temp
#endif
}

LocationMgr* getLocationMgr()
{
#ifdef MTHREAD_COMPILE
    CompileThread *compileThread = Compiler::getCurrentThread();
    CompileUnit *compileUnit = compileThread->getCompileUnit();
    LocationMgr *locationMgr = compileUnit->getLocationMgr();
    return locationMgr;
#else
    static LocationMgr locationMgr;
    return &locationMgr;

#endif
}
