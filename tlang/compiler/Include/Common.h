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
//#define TDEBUG

void dbgprint(const char* fmt, ...);

#ifdef TDEBUG
#define dbg dbgprint
#else
#define dbg
#endif
// xml support
#define LIBXML_TREE_ENABLED
#define LIBXML_OUTPUT_ENABLED
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#endif // TCC_COMMON_H
