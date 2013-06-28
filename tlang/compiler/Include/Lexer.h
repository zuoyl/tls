//
//  LexerBuilder.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_LEXER_H
#define TCC_LEXER_H

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

/// SimpleLexer - build by hand
class SimpleLexer {
public:
    SimpleLexer(const string& path, const string& file);
    ~SimpleLexer();
    bool parse(TokenStream* tokenStream);
    
private:
    SimpleLexer();
    char getChar();
    void putChar(char ch);
    void pushToken(Token* token);
    void getAtomString(char ch, std::string& atom);
    Token* parseDigitLiteral(char ch);
    Token* parseKeyWord(std::string& name);
private:
    std::ifstream m_ifs;
    string m_file;
    string m_path;
    string m_fullFileName;
    Grammar* m_grammar;
};

/// Lexer - build by NFA/DFA table driven(TODO)
class Lexer
{
public:
    Lexer(const string& path, const string& file);
    ~Lexer();
    Token* getToken();
private:
    string m_file;
    string m_path;
};


#endif // TCC_LEXER_H
