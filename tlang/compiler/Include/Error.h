//
//  Error.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_ERROR_H
#define TCC_ERROR_H

#include <cstdarg>

class Location;
class AST;

namespace Error {
    void complain(const char *fmt, ...);
    void complain(const Location &location, const char *fmt, ...);
    void complain(const AST &node, const char *fmt, ...);
};

#endif // TCC_ERROR_H
