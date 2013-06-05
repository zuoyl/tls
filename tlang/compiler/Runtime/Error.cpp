//
//  Error.cpp
//  A toyable language compiler (like a simple c++)

#include "Error.h"
#include "Compile.h"
#include "Location.h"
#include "AST.h"

void dbgprint(const char *fmt, ...)
{
    char buf[256] = {0};
    va_list list;
    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << buf;
    va_end(list);
}


void Error::complain(const char *fmt, ...)
{
    char buf[255] = { 0 };
    va_list list;
 
    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << buf << std::endl;
    va_end(list);

}

void Error::complain(const Location &location, const char *fmt, ...)
{
    char buf[255] = { 0 };
    va_list list;
    // get compile source file path and name
    LocationMgr *locationMgr = getLocationMgr();
    string sourcePath = locationMgr->getCompileSourcePath();
    string sourceFile = locationMgr->getCompileSourceFile();
    int lineno = location.getLineno();

    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << sourcePath << "/" << sourceFile << ":" <<  lineno <<  ":" <<  buf << std::endl;
    va_end(list);
}


void Error::complain(const AST &node, const char *fmt, ...)
{
    char buf[255] = { 0 };
    va_list list;
    // get compile source file path and name
    LocationMgr *locationMgr = getLocationMgr();
    string sourcePath = locationMgr->getCompileSourcePath();
    string sourceFile = locationMgr->getCompileSourceFile();
    int lineno = node.getLocation().getLineno();

    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << sourcePath << "/" << sourceFile << ":" << lineno << ":"  <<  buf << std::endl;
    va_end(list);
}
