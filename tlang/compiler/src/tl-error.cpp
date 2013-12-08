//
//  Error.cpp
//  A toyable language compiler (like a simple c++)

#include "tl-error.h"
#include "tl-compile.h"
#include "tl-location.h"
#include "tl-ast.h"

using namespace tlang;

#ifdef DEBUG
void dbg(const char* fmt, ...)
{
    char buf[256] = {0};
    va_list list;
    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << buf;
    va_end(list);
}
#else
void dbg(const char* fmt, ...)
{}
#endif 

void Error::complain(const char* fmt, ...)
{
    char buf[255] = { 0 };
    va_list list;
 
    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << buf << std::endl;
    va_end(list);
}

void Error::complain(const Location& location, const char* fmt, ...)
{
    char buf[255] = { 0 };
    va_list list;
    // get compile source file path and name
    LocationMgr *locationMgr = Compiler::getLocationMgr();
    std::string sourcePath = locationMgr->getCompileSourcePath();
    std::string sourceFile = locationMgr->getCompileSourceFile();
    int lineno = location.getLineno();

    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << sourcePath << "/" << sourceFile << ": " <<  lineno <<  ":" <<  buf << "." << std::endl;
    va_end(list);
}


void Error::complain(const AST& node, const char* fmt, ...)
{
    char buf[255] = { 0 };
    va_list list;
    // get compile source file path and name
    LocationMgr *locationMgr = Compiler::getLocationMgr();
    std::string sourcePath = locationMgr->getCompileSourcePath();
    std::string sourceFile = locationMgr->getCompileSourceFile();
    int lineno = node.getLocation().getLineno();

    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << sourcePath << "/" << sourceFile << ":" << lineno << ": "  <<  buf << "." << std::endl;
    va_end(list);
}
