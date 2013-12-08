//
//  Error.h
//  A toyable language compiler (like a simple c++)


#ifndef __TL_ERROR_H__
#define __TL_ERROR_H__

#include <cstdarg>

namespace tlang { 
    class Location;
    class AST;
    namespace Error {
        void complain(const char *fmt, ...);
        void complain(const Location &location, const char *fmt, ...);
        void complain(const AST &node, const char *fmt, ...);

    } // namespace error
} // namespace tlang 
#endif // __TL_ERROR_H__
