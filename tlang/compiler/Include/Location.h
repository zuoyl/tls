///
/// Location.h - location for compilation information, such as line
//  A toyable language compiler (like a simple c++

#ifndef TCC_LOCATION_H
#define TCC_LOCATION_H

#include "Common.h"

class Location
{
public:
    Location(){ m_lineno = -1; }
    Location(int lineno):m_lineno(lineno){}
    ~Location(){}
    int getLineno() const {  return m_lineno;  }
    void setLineno(int lineno) {  m_lineno = lineno; }
    Location &operator = (Location &location) {
        this->m_lineno = location.m_lineno;
    }

private:
    int m_lineno;
};

class LocationMgr
{
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

#endif // TCC_LOCATION_H
