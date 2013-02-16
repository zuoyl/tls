//
//  OSWraper.h
//  A toyable language compiler (like a simple c++)
//


#ifndef TCC_OS_WRAPER_H
#define TCC_OS_WRAPER_H 

#include <Common.h>

using namespace  std;

namespace OS {
    bool isFilesExist(const string &fullPathFile);
    bool isFolderExist(const string &folder);
    void getWorkPath(string &path);
};

#endif //  TCC_OS_WRAPER_H
