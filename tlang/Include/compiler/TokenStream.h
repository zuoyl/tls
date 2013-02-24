//
//  TokenStream.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_TOKEN_STREAM_H
#define TCC_TOKEN_STREAM_H

#include "compiler/Common.h"

struct Token {
public:
    std::string assic;
    int type;
    int lineno;
    int column;
public:
    Token();
    Token(const char *name, int type, int lineno);
    Token(char ch, int tp, int line);
    ~Token();
};


class TokenStream {
public:
    TokenStream();
    ~TokenStream();
    
    void pushToken(Token *token);
    bool matchToken(int type, Token **token);
    bool matchToken(int type, const char *name);
    Token * getToken();
    void advanceToken(Token ** = NULL);
    void setMark(int mark) { m_mark = mark; }
    void clearMark() {m_mark = 0;}
    void reset() { m_index = 0; }
    void dumpAllTokens();
private:
    std::vector<Token *> m_tokens;
    int m_mark;
    int m_index;
};


#endif // TCC_TOKEN_STREAM_H
