///
/// Location.h - location for compilation information, such as line
//  A toyable language compiler (like a simple c++

#ifndef __TL_LOCATION_H__
#define __TL_LOCATION_H__

#include "tl-common.h"

namespace tlang { 
    class Location {
        public:
            Location(){ m_lineno = -1; }
            Location(int lineno):m_lineno(lineno){}
            ~Location(){}
            int getLineno() const {  return m_lineno;  }
            void setLineno(int lineno) {  m_lineno = lineno; }
            Location& operator = (Location &location) {
                m_lineno = location.m_lineno;
                return *this;
            }
            void operator = (int v) { m_lineno = v; }
        private:
            int m_lineno;
    };

    class LocationMgr {
        public:
            LocationMgr(){}
            ~LocationMgr(){}
            void setCompileSourceFile(const string &file) { m_sourceFile = file; }
            void setCompileSourcePath(const string &path) { m_sourcePath = path; }
            const string& getCompileSourceFile() { return m_sourceFile; }
            const string& getCompileSourcePath() { return m_sourcePath; }
        private:
            string m_sourceFile;
            string m_sourcePath;
    };

} // namespace tlang 
#endif // __TL_LOCATION_H__
