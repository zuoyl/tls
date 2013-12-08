//
//  Lexer.h
//  A toyable language compiler (like a simple c++)


#ifndef __TL_LEXER_H__
#define __TL_LEXER_H__

#include "tl-common.h"
#include "tl-token.h"
#include "tl-grammar.h"
#include <string>

namespace tlang {
    enum TokenType {
        TOKEN_TYPE_UNKNOWN = 0, 
        TOKEN_TYPE_KEYWORD,
        TOKEN_TYPE_OP,
        TOKEN_TYPE_ID,
        TOKEN_TYPE_INT,
        TOKEN_TYPE_FLOAT,
        TOKEN_TYPE_STRING
    };

    /// SimpleLexer - build by hand
    class SimpleLexer {
    public:
        SimpleLexer(const string &path, const string &file);
        ~SimpleLexer();
        bool parse(TokenStream *tokenStream);
        
    private:
        SimpleLexer();
        char getChar();
        void putChar(char ch);
        void pushToken(Token *token);
        void getAtomString(char ch, string &atom);
        Token* parseDigitLiteral(char ch);
        Token* parseKeyWord(string &name);
    private:
        ifstream m_ifs;
        string m_file;
        string m_path;
        string m_fullFileName;
        Grammar *m_grammar;
    };

    /// Lexer - build by NFA/DFA table driven(TODO)
    class Lexer {
    public:
        Lexer(const string &path, const string &file);
        ~Lexer();
        Token* getToken();
    private:
        string m_file;
        string m_path;
    };

} // namespace tlang 
#endif // __TL_LEXER_H__
