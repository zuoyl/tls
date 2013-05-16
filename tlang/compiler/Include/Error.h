//
//  Error.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_ERROR_H
#define TCC_ERROR_H

#include <cstdarg>
#include "Location.h"

namespace Error {
    void complain(const char *fmt, ...);
    void complain(Location &location, const char *fmt, ...);
};

#endif // TCC_ERROR_H
