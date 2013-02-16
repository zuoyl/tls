//
//  Exception.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_EXCEPTION_H
#define TCC_EXCEPTION_H

#include <Common.h>
#include <stdexcept>

namespace Exception {
    class MccException : public std::exception {
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
} // namespace 


#endif // TCC_EXCEPTION_H
