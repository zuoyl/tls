//
//  Error.cpp
//  A toyable language compiler (like a simple c++)

#include "Error.h"
#include "Compile.h"
#include "Location.h"

void Error::complain(const char *fmt, ...)
{
    char buf[255] = { 0 };
    va_list list;
 
    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << buf << std::endl;
    va_end(list);

}

void Error::complain(Location &location, const char *fmt, ...)
{
    char buf[255] = { 0 };
    va_list list;
    // get compile source file path and name
    LocationMgr *locationMgr = getLocationMgr();
    string sourcePath = locationMgr->getCompileSourcePath();
    string sourceFile = locationMgr->getCompileSourceFile();

    va_start(list, fmt);
    vsprintf(buf, fmt, list);
    std::cout << sourcePath << "/" << sourceFile << ":" <<  buf << std::endl;
    va_end(list);
}
