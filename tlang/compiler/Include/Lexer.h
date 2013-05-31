//
//  LexerBuilder.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_LEXERBUILDER_H
#define TCC_LEXERBUILDER_H

#include "Common.h"

class Token;
class TokenStream;
class Grammar;

enum TokenType {
    T_KEYWORD,
    T_OP,
    T_ID,
    T_INT,
    T_FLOAT,
    T_STRING
};

class Lexer {
public:
    Lexer(const string& path, const string& file);
    ~Lexer();
    bool parse(TokenStream *tokenStream);
    
private:
    Lexer();
    char getChar();
    void putChar(char ch);
    void pushToken(Token *token);
    void getAtomString(char ch, std::string &atom);
    Token *parseDigitLiteral(char ch);
    Token *parseKeyWord(std::string &name);
private:
    std::ifstream m_ifs;
    string m_file;
    string m_path;
    string m_fullFileName;
    Grammar *m_grammar;
};

#endif // TCC_LEXERBUILDER_H
