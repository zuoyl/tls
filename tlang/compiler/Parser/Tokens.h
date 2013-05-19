//
//  TokenStream.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_TOKEN_STREAM_H
#define TCC_TOKEN_STREAM_H

#include <string>
#include <vector>

using namespace std;

struct Token 
{
public:
    std::string assic;
    int type;
    int lineno;
    int column;
public:
    Token()
        {assic = "", type = -1; lineno = -1; column = -1; }
    Token(const char *name, int type, int lineno)
        {assic = name; this->type = type, this->lineno = lineno; }
    Token(char ch, int type, int lineno)
        {assic = ch; this->type = type, this->lineno = lineno; }
    ~Token(){}
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
