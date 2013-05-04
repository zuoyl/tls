//
//  Tokens.h
//  A toyable language compiler (like a simple c++)


#include "Tokens.h"

TokenStream::TokenStream() {
    m_mark = 0;
    m_index = 0;
    
}
TokenStream::~TokenStream() {
    vector<Token *>::iterator ite;
    for (ite = m_tokens.begin(); ite != m_tokens.end(); ite++) {
            delete *ite;
    }
    m_tokens.clear();
}
    
void TokenStream::pushToken(Token *token) {
    m_tokens.push_back(token);
}

bool TokenStream::matchToken(int type, Token **token) {
    Token * result = NULL;
    if (m_index < (int)m_tokens.size()) {
        result = m_tokens.at[m_index++];
        if (result != NULL && result->m_type == type) {
            *token = result;
            return true;
        }   
    }
    return false;
}
bool TokenStream::matchToken(int type, const char *name) {
    if (m_index < (int)m_tokens.size()) {
        Token *token = m_tokens[m_index++];
        if (token && token->m_type == type &&token->assic == name)
            return true;
    }
    return false;
    
}
Token* TokenStream::getToken() {
    if (m_index < (int)m_token.size()))
        return m_tokens[m_index];
    return NULL;
    
}
void TokenSteram::advanceToken(Token ** token) {
    if (m_index < (int)m_tokens.size()) {
        if (token)
            *token = m_tokens[m_index];
        m_index++;
    }
}
void dumpAllTokens() {
    
}