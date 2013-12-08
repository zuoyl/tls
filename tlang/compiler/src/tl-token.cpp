//
//  Tokens.h
//  A toyable language compiler (like a simple c++)


#include <iostream>
#include "tl-token.h"
#include "tl-common.h"

using namespace tlang;

TokenStream::TokenStream() 
{
    m_mark = 0;
    m_index = 0;
}

TokenStream::~TokenStream() 
{
    vector<Token*>::iterator ite;
    for (ite = m_tokens.begin(); ite != m_tokens.end(); ite++) 
        delete *ite;
    m_tokens.clear();
}

void TokenStream::clear()
{
    vector<Token*>::iterator ite;
    for (ite = m_tokens.begin(); ite != m_tokens.end(); ite++) 
        delete *ite;
    m_tokens.clear();
    m_index = 0;
    m_mark = 0;
}

void TokenStream::pushToken(Token *token) 
{
    if (token)
        m_tokens.push_back(token);
}

bool TokenStream::matchToken(int type, Token **token) 
{
    Token* result = NULL;
    if (m_index < m_tokens.size()) {
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
    if (m_index < m_tokens.size()) {
        Token *token = m_tokens[m_index++];
        if (token && token->type == type) {
            if (name && token->assic != name)
                return false;
            return true;
        }
    }
    return false;
}

Token* TokenStream::lookNextToken()
{
    if ((m_index + 1) < m_tokens.size()) 
        return m_tokens[m_index + 1];
    else 
        return NULL;
}

Token* TokenStream::getToken() 
{
    if (m_index < m_tokens.size())
        return m_tokens[m_index];
    return NULL;
    
}
void TokenStream::advanceToken(Token **token) 
{
    if (m_index < m_tokens.size()) {
        if (token)
           *token = m_tokens[m_index];
        m_index++;
    }
}

void TokenStream::dumpAllTokens() 
{
    int index = 0;
    cout << "####----------------TokenStream dump --------------####" << std::endl;
    cout << "\t" << index << "\t" << "assic" << "\t\t\t\t" << "type" << "\t\t" << "lineno" <<  std::endl;

    vector<Token*>::iterator ite;
    for (ite = m_tokens.begin(); ite != m_tokens.end(); ite++) {
        Token* token =* ite;
        cout << "\t" << index << "\t";
        cout << token->assic << "\t\t\t\t";
        cout << token->type << "\t\t";
        cout << token->location.getLineno() << std::endl;
        index++;
    }
}
