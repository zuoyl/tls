//
//  Exception.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_EXCEPTION_H
#define TCC_EXCEPTION_H

#include "Common.h"
#include <stdexcept>
namespace Exception {
class TccException : public std::exception {
public:
    virtual const char* what() throw() = 0;
    virtual void print(iostream &ios) throw() = 0;
};
    
class NoMatchedTokenException : public TccException {
public:
    NoMatchedTokenException(){}
    NoMatchedTokenException(const string &token):m_token(token){}
    ~NoMatchedTokenException() throw(){}
    void print(iostream &ios) throw(){}
    const char* what()  throw() { return NULL; }
protected:
    string m_token;
};


class InvalidExpr : public TccException {
public:
    InvalidExpr(){}
    InvalidExpr(const string &expr):m_item(expr){}
    ~InvalidExpr() throw() {}
    void print(iostream &ios) throw(){}
    const char* what() throw() { return m_item.c_str(); }
private:
    string m_item;

};

class InvalidStatement : public TccException {
public:  
    InvalidStatement(){}
    InvalidStatement(const string &stmt):m_item(stmt){}
    ~InvalidStatement() throw() {}
    void print(iostream &ios) throw(){}
    const char* what()throw() { return m_item.c_str(); }
private:
    string m_item;
};

class InvalidSyntax : public TccException {
public:
    InvalidSyntax(){}
    InvalidSyntax(const string &expr):m_item(expr){}
    ~InvalidSyntax() throw() {}
    void print(iostream &ios) throw(){}
    const char* what() throw() { return m_item.c_str(); }
private:
    string m_item;

};

class InvalidScope : public TccException {
public:
    InvalidScope(){}
    InvalidScope(const string &expr):m_item(expr){}
    ~InvalidScope() throw() {}
    void print(iostream &ios) throw(){}
    const char* what() throw() { return m_item.c_str(); }
private:
    string m_item;

};

} // namesapce


#endif // TCC_EXCEPTION_H
