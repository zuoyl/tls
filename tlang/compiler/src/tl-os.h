//
//  tl-os.h
//  A toyable language compiler (like a simple c++)
//

#ifndef __TL_OS_H__
#define __TL_OS_H__

#include "tl-common.h"
#include <string>

namespace tl {
    namespace os {
        bool isFilesExist(const string& fullPathFile);
        bool isFolderExist(const string& folder);
        void getWorkPath(string& path);
    } // namespace os

} // namespace tl
#endif // __TL_OS_H__ 
