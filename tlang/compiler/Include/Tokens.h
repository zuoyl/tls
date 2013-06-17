//
//  TokenStream.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_TOKEN_STREAM_H
#define TCC_TOKEN_STREAM_H

#include <string>
#include <vector>
#include "Common.h"
#include "Exception.h"
#include "Location.h"

using namespace std;

struct Token 
{
public:
    std::string assic;
    int type;
    Location location;
public:
    Token():location(-1), type(-1){}
    
    Token(const char *name, int type, int lineno)
        :location(lineno)
    { this->assic = name; this->type = type; }

    Token(char ch, int type, int lineno)
        :location(lineno)
    { this->assic = ch;  this->type = type; }
   
};


class TokenStream 
{
public:
    TokenStream();
    ~TokenStream();
    
    void pushToken(Token *token);
    // check wether the next token is matched withe the specified type,
    // if it is matched, get the matched token if the token is not null
    bool matchToken(int type, Token **token);
    // check wether the next token is matched with the specified type without moving token
    bool matchToken(int type, const char *name);
    Token* getToken();
    Token* lookNextToken(); 
    void advanceToken(Token ** = NULL);
    void setMark(int mark) { m_mark = mark; }
    void clearMark() {m_mark = 0;}
    void reset() { m_index = 0; } 
    void clear();
    void dumpAllTokens();
private:
    vector<Token *> m_tokens;
    size_t m_mark;
    size_t m_index;
};


#endif // TCC_TOKEN_STREAM_H
