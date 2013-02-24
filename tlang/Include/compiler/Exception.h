//
//  Exception.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_EXCEPTION_H
#define TCC_EXCEPTION_H

#include "compiler/Common.h"
#include <stdexcept>

class TccException : public std::exception {
public:
    virtual const char* what() const throw() = 0;
    virtual void print(iostream &ios) throw() = 0;
};
    
class NoMatchedToken : public MccException {
public:
    NoMatchedToken(){}
    NoMatchedToken(const string &token):m_token(token){}
    ~NoMatchedToken() throw() {}
    void print(iostream &ios) throw(){}
    char* what()const throw() { return NULL; }
protected:
    string m_token;
};


class InvalidExpression : public TccException {
public:
    InvalidExpression(){}
    InvalidExpression(const string &expr):m_item(expr){}
    ~InvalidExpression() throw() {}
    void print(iostream &ios) throw(){}
    char* what()const throw() { return m_item.c_str(); }
private:
    string m_item;

};

class InvalidStatement : public TccExcetpion {
public:  
    InvalidStatement(){}
    InvalidStatement(const string &stmt):m_item(stmt){}
    ~InvalidStatement() throw() {}
    void print(iostream &ios) throw(){}
    char* what()const throw() { return m_item.c_str(); }
private:
    string m_item;
};

class InvalidSyntax : public TccException {
public:
    InvalidSyntax(){}
    InvalidSyntax(const string &expr):m_item(expr){}
    ~InvalidSyntax() throw() {}
    void print(iostream &ios) throw(){}
    char* what()const throw() { return m_item.c_str(); }
private:
    string m_item;

};




#endif // TCC_EXCEPTION_H
