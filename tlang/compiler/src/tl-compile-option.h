////
/// tl-compile-option.h 
/// compiler unit to manger lexer, parser and other components
/// to improve the compile performance, maybe use the mulitthread to compile,
/// in future, each thread will contain a compile unit
///
///  A toyable language compiler (like a simple c++)

#ifndef __TL_COMPILE_OPTION_H__
#define __TL_COMPILE_OPTION_H_

#include "tl-common.h"

namespace tlang { 
    /// 'class CompileOption
    /// for each compilation pass ,the compliation option is single only
    class CompileOption {
        public:
            static CompileOption& getInstance(); 
            void getSourceFiles(vector<string> &files);
            void setCompileLevel(int v) { m_compileLevel = v; }
            int  getCompileLevel() { return m_compileLevel; }
            void setDebug(bool v) { m_isDebug = v; }
            bool getDebug() { return m_isDebug; }
            bool isOutputAssembleFile() { return m_isOutputAssembleFile; }
            void setOutputAssembleFile(bool v) { m_isOutputAssembleFile = v; }
            bool isOutputParseTree() { return m_isOutputParseTree; }
            void setOutputParseTree(bool v) { m_isOutputParseTree = v; }
            bool isOutputAST() { return m_isOutputAST; }
            void setOutputAST(bool v) { m_isOutputAST = v; }
            bool parse(const char* argv[], int argc);
            void usage(ostream &ios);
                
        private:
            CompileOption();
            ~CompileOption();
            void dumpAllOptions(ostream &ios);
            bool checkOptionValidity(const string &key, string &val);
            bool getOneOption(const char* argv[], int &leftArgc, string &key, string &val);
            void parseAllOptions(map<string, string> &options);

        private:
            int m_compileLevel;
            bool m_isDebug;
            bool m_isOutputAssembleFile;
            bool m_isOutputAST;
            bool m_isOutputParseTree;
            vector<string> m_sourceFiles;
    };

} // namespace tlang 
#endif  // __TL_COMPILE_OPTION_H__


