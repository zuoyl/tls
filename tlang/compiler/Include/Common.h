//
//  Common.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_COMMON_H
#define TCC_COMMON_H

#include <string>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <iostream>
#include <assert.h>
#include <fstream>

using namespace std;

#define ASSERT assert


#include "Error.h"


void dbgprint(const char* fmt, ...);

#ifdef TDEBUG
#define dbg dbgprint
#else
#define dbg
#endif

#endif // TCC_COMMON_H
