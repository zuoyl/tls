//
//  tl-token.h
//  A toyable language compiler (like a simple c++)
//  @author:jenson.zuo@gmail.com
//


#ifndef __TL_TOKEN_H__
#define __TL_TOKEN_H__

#include "tl-common.h"
#include "tl-exception.h"
#include "tl-location.h"
#include <string>
#include <vector>

namespace tlang {
    struct Token {
        public:
            string assic;
            int type;
            Location location;
        public:
            Token():type(-1), location(-1){}
            Token(const char* name, int type, int lineno)
                :location(lineno) { 
                this->assic = name; 
                this->type = type; 
            }

            Token(char ch, int type, int lineno)
                :location(lineno) { 
                this->assic = ch;  
                this->type = type; 
            }
    };


    class TokenStream {
        public:
            TokenStream();
            ~TokenStream();
            
            void pushToken(Token *token);
            bool matchToken(int type, Token **token);
            bool matchToken(int type, const char *name);
            Token* getToken();
            Token* lookNextToken(); 
            void advanceToken(Token** = NULL);
            void mark() { m_mark = m_index; }
            void clearMark() {m_mark = 0;}
            void gotoMark() { m_index = m_mark; } 
            void reset() { m_index = 0; } 
            void clear();
            void dumpAllTokens();
        private:
            vector<Token*> m_tokens;
            size_t m_mark;
            size_t m_index;
    };

} // namespace 
#endif // __TL_TOKEN_H__
