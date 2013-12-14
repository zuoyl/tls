//
//  Common.h
//  A toyable language compiler (like a simple c++)


#ifndef __TL_COMMON_H__
#define __TL_COMMON_H__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <stack>
#include <iostream>
#include <assert.h>
#include <fstream>

using namespace std;

void dbg(const char* fmt, ...);

#ifdef DEBUG
#define require(condition) assert(condition)
#define assure(condition)  assert(condition)
#define Assert(conditon) assert(conditon)
#else
#define require(condition) 
#define assure(condition) 
#define assert(condition) 
#endif


#include "tl-error.h"


// xml support
#define LIBXML_TREE_ENABLED
#define LIBXML_OUTPUT_ENABLED
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

#endif // __TL_COMMON_H__
