////
/// tl-compile.h 
/// compiler unit to manger lexer, parser and other components
/// to improve the compile performance, maybe use the mulitthread to compile,
/// in future, each thread will contain a compile unit
///
///  A toyable language compiler (like a simple c++)
/// @author:jenson.zuo@gmail.com
/// 

#ifndef __TL_COMPILE_H__
#define __TL_COMPILE_H_

#include "tl-common.h"

namespace tlang { 
    class TokenStream;
    class LocationMgr;
    class SimpleLexer;
    class Parser;
    class ASTBuilder;
    class TypeBuilder;
    class IRBuilder;
    class TypeDomain;

    /// 'class CompileUnit
    /// each compile unit include a source file to be compile 
    /// for multithread to improve performance
    class CompileUnit {
        public:
            CompileUnit(const string &path, const string& file);
            ~CompileUnit();
            bool build();
            LocationMgr* getLocationMgr() { return m_locationMgr;} 
        private:
            LocationMgr *m_locationMgr;
            TokenStream *m_tokenStream;
            SimpleLexer *m_lexer;
            Parser      *m_parser;
            ASTBuilder  *m_astBuilder;
            TypeBuilder *m_typeBuilder;
            IRBuilder   *m_irBuilder;
            TypeDomain  *m_typeDomain;
            string m_sourcePath;
            string m_sourceFile;
    };

    class CompileThread {
        public:
            CompileThread(const string &path, const string &file);
            ~CompileThread();
            void start();
            int getThreadID() { return m_threadID; }
            CompileUnit* getCompileUnit() { return m_compileUnit; }
            static void CompileThreaProc(CompileUnit *compileUnit);

        private:
            CompileUnit* m_compileUnit;
            int m_threadID;
    };


    class Compiler {
        public:
             enum { CompileMaxThreads = 10 };
        public:
            void compile(vector<string> &sourFiles);
            static CompileThread* getCurrentThread();
            static Compiler& getInstance();
            static LocationMgr* getLocationMgr();
        private:
            Compiler(){}
            ~Compiler(){}
        private:
            static map<int, CompileThread* >  m_theads;
        };

} // namespace tlang 
#endif  // __TL_COMPILE_H


