//
//  TokenStream.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_TOKEN_STREAM_H
#define TCC_TOKEN_STREAM_H

#include "Common.h"
#include "Exception.h"
#include "Location.h"

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
   
    ~Token(){}
};


class TokenStream {
public:
    TokenStream() 
    { m_mark = 0; m_index = 0; }

    ~TokenStream()
    {
        vector<Token *>::iterator ite = m_tokens.begin();
        for (; ite != m_tokens.end(); ite++)
            delete(*ite);
    }

    
    void pushToken(Token *token)
    { 
        if (token)
            m_tokens.push_back(token);
    }
    // check wether the next token is matched withe the specified type,
    // if it is matched, get the matched token if the token is not null
    bool matchToken(int type, Token **token)
    {
        if (m_index < (int)m_tokens.size()) {
            if (m_tokens[m_index]->type == type) {
                if (token)
                    *token = m_tokens[m_index];
                m_index++;
                return true;
            }
        }
        throw Exception::NoMatchedTokenException();
        return false;
    }
    // check wether the next token is matched with the specified type without moving token
    bool matchToken(int type, const char *name)
    {
        if (m_index < (int)m_tokens.size()) {
            if (m_tokens[m_index]->type == type && 
                    m_tokens[m_index]->assic == name)
                return true;
        }
        return false;
    }

    // get the current token
    Token * getToken()
    {
        if (m_index < (int)m_tokens.size())
            return m_tokens[m_index++];
        else
            return NULL;
    }
    
    void advanceToken(Token ** token = NULL)
    {
        if (m_index < (int)m_tokens.size()) {
            if (token)
                *token = m_tokens[m_index];
            m_index++;
        }
    }

    void setMark(int mark) { m_mark = mark; }
    void clearMark() {m_mark = 0;}
    void reset() { m_index = 0; }
    void dumpAllTokens()
    {
        // dump all tokens
        for (int i = 0; i < (int)m_tokens.size(); i++) {
            Token * token = m_tokens[i];
            std::cout << index << "\t" << token->assic;
            std::cout << "\t\t" << token->location.getLineno() << std::endl;
        }
    }
private:
    std::vector<Token *> m_tokens;
    int m_mark;
    int m_index;
};


#endif // TCC_TOKEN_STREAM_H
