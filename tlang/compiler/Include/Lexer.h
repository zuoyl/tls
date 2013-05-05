//
//  LexerBuilder.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_LEXERBUILDER_H
#define TCC_LEXERBUILDER_H

#include "Common.h"
#include "../../lib/TParser/TGrammar.h"

class Token;
class TokenStream;

enum TokenType {
    T_KEYWORD,
    T_OP,
    T_ID,
    T_INT,
    T_FLOAT,
    T_STRING
};

class LexerException {
    
};

class Lexer {
public:
    Lexer(const char *file, TGrammar *grammar);
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
    std::string m_file;
    TGrammar *m_grammar;
};

#endif // TCC_LEXERBUILDER_H
