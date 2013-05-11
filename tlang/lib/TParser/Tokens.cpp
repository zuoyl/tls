//
//  Tokens.h
//  A toyable language compiler (like a simple c++)


#include "Tokens.h"
#include <iostream>

TokenStream::TokenStream() 
{
    m_mark = 0;
    m_index = 0;
    
}
TokenStream::~TokenStream() 
{
    vector<Token *>::iterator ite;
    for (ite = m_tokens.begin(); ite != m_tokens.end(); ite++) {
            delete *ite;
    }
    m_tokens.clear();
}
    
void TokenStream::pushToken(Token *token) 
{
    m_tokens.push_back(token);
}

bool TokenStream::matchToken(int type, Token **token) 
{
    Token * result = NULL;
    if (m_index < (int)m_tokens.size()) {
        result = m_tokens[m_index++];
        if (result && result->type == type) {
            if (token )
                *token = result;
            return true;
        }   
    }
    return false;
}
bool TokenStream::matchToken(int type, const char *name) 
{
    if (m_index < (int)m_tokens.size()) {
        Token *token = m_tokens[m_index++];
        if (token && token->type == type) {
            if (name && token->assic != name)
                return false;
            return true;
        }
    }
    return false;
    
}
Token* TokenStream::getToken() 
{
    if (m_index < (int)m_tokens.size())
        return m_tokens[m_index];
    return NULL;
    
}
void TokenStream::advanceToken(Token ** token) 
{
    if (m_index < (int)m_tokens.size()) {
        if (token)
            *token = m_tokens[m_index];
        m_index++;
    }
}
void TokenStream::dumpAllTokens() 
{
    int index = 0;
    vector<Token *>::iterator ite;

    std::cout << "####----------------TokenStream dump --------------####" << std::endl;
    std::cout << "\t" << index << "\t" << "assic" << "\t\t\t\t" << "type" << "\t\t" << "lineno" <<  std::endl;

    for (ite = m_tokens.begin(); ite != m_tokens.end(); ite++) {
        Token *token = *ite;
        std::cout << "\t" << index << "\t";
        std::cout << token->assic << "\t\t\t\t";
        std::cout << token->type << "\t\t";
        std::cout << token->lineno << std::endl;
        index++;
    }
    
}
