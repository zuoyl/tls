//
//  tl-exception.h
//  A toyable language compiler (like a simple c++)


#ifndef __TL_EXCEPTION_H__
#define __TL_EXCEPTION_H__

#include "tl-common.h"
#include <stdexcept>

namespace tlang {
    namespace Exception {
        class TLException : public exception {
            public:
                virtual void print(iostream &ios) throw() = 0;
        };
                
        class NoMatchedToken : public TLException {
            public:
                NoMatchedToken(){}
                NoMatchedToken(const string &token):m_token(token){}
                ~NoMatchedToken() throw(){}
                void print(iostream &ios) throw(){}
            protected:
                string m_token;
        };


        class InvalidExpr : public TLException {
            public:
                InvalidExpr(){}
                InvalidExpr(const string &expr):m_item(expr){}
                ~InvalidExpr() throw() {}
                void print(iostream &ios) throw(){}
            private:
                string m_item;

        };

        class InvalidStatement : public TLException {
            public:  
                InvalidStatement(){}
                InvalidStatement(const string &stmt):m_item(stmt){}
                ~InvalidStatement() throw() {}
                void print(iostream &ios) throw(){}
            private:
                string m_item;
        };

        class InvalidSyntax : public TLException {
            public:
                InvalidSyntax(){}
                InvalidSyntax(const string &expr):m_item(expr){}
                ~InvalidSyntax() throw() {}
                void print(iostream &ios) throw(){}
            private:
                string m_item;

        };

        class InvalidScope : public TLException {
            public:
                InvalidScope(){}
                InvalidScope(const string &expr):m_item(expr){}
                ~InvalidScope() throw() {}
                void print(iostream &ios) throw(){}
            private:
                string m_item;

        };
    } // namesapce exception
} // namespace tlang 
#endif // __TL_EXCEPTION_H__
