//
//  LexerBuilder.cpp
//  A toyable language compiler (like a simple c++)


#include "compiler/Common.h"
#include "compiler/Lexer.h"
#include "compiler/TokenStream.h"
#include "compiler/Grammar.h"

using namespace std;

Lexer::Lexer()
{
    m_file = "";
}

Lexer::Lexer(const char *file, Grammar *grammar)
{
    m_file = file;
    m_grammar = grammar;
}

Lexer::~Lexer()
{
    m_file = "";
    m_grammar = NULL;
    if (m_ifs)
        m_ifs.close();
}

char Lexer::getChar()
{
    return m_ifs.get();
}
void Lexer::putChar(char ch)
{
    m_ifs.putback(ch);
}


Token * Lexer::parseKeyWord(std::string &name)
{
    Token *token = NULL;
    
    map<string, int>::iterator ite = m_grammar->keywordIDs.find(name);
    if (ite != m_grammar->keywordIDs.end()) {
            token = new Token();
            token->assic = name;
            token->type = T_KEYWORD;
    }
    return token;
}

void Lexer::getAtomString(char ch, std::string &name)
{
    if (ch != '"')
        return;
    
    while ((ch = getChar()) != EOF) {
        if (ch == '\r' || ch == '\n')
            continue;
        if (ch != '"') {
            name += ch;
        }
        else 
            break;
    }
}

Token * Lexer::parseDigitLiteral(char ch)
{
    Token *token = NULL;
    std::string digit = "";
    
    digit += ch;
    while ((ch = getChar()) != EOF) {
        if (!isdigit(ch)) {
            putChar(ch);
            break;
        }
        digit += ch;
    }
    token = new Token();
    token->type = T_INT;
    
    // check to see wether it is a float
    if ((ch = getChar()) == '.') {
        digit += ch;
        std::string subDigit = "";
        while ((ch == getChar()) != EOF) {
            if (!isalpha(ch)) {
                putChar(ch);
                break;
            }
            subDigit += ch;
        }
        digit += subDigit;
        token->type = T_FLOAT;
    }
    return token;
}

bool Lexer::parse(TokenStream *tokenStream)
{
    Token *token = NULL;
    char ch;
    int line = 0;
    std::string atom = "";
    
    while ((ch = getChar()) != EOF) {
        switch (ch) {
            case '+':
                token = new Token();
                token->type = T_OP;
                token->lineno = line;
                if (((ch = getChar()) != EOF) && (ch == '+')) {
                    token->assic = "++";
                }
                else {
                    token->assic = "+";
                    putchar(ch);
                }
                tokenStream->pushToken(token);
                break;
                
            case '-':
                token = new Token();
                token->type = T_OP;
                token->lineno = line;
                if (((ch = getChar()) != EOF) && (ch == '-')) {
                    token->assic = "--";
                }
                else  {
                    token->assic = '-';
                    putchar(ch);
                }
                tokenStream->pushToken(token);
                break;
                
            case '*':
                token = new Token(ch, T_OP, line);;
                tokenStream->pushToken(token);
                break;
                
            case '/':
                if (((ch = getChar()) != EOF) && (ch = '/')) {
                    // consume comments
                    while (((ch = getChar()) != EOF)) {
                        if (ch != '\r' || ch != '\n')
                            continue;
                    }
                }
                else {
                    token = new Token('/', T_OP, line);
                    putChar(ch);
                }
                tokenStream->pushToken(token);
                break;
                
            case '&':
                token = new Token();
                token->type = T_OP;
                token->lineno = line;
                if (((ch = getChar()) != EOF) && (ch == '&')) {
                    token->assic = "&&";
                }
                else  {
                    token->assic = '&';
                    putchar(ch);
                }
                tokenStream->pushToken(token);
                break;
     
            case '!':
                token = new Token();
                token->type = T_OP;
                token->lineno = line;
                if (((ch = getChar()) != EOF) && (ch == '=')) {
                    token->assic = "!=";
                }
                else  {
                    token->assic = '!';
                    putchar(ch);
                }                
                tokenStream->pushToken(token);
                break;
                
            case '%':
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case '\'':
            case '?':
            case ',':
            case '.':
            case '#':
            case '~':
            case '`':
            case '\\':
                token = new Token(ch, T_OP, line);
                tokenStream->pushToken(token);
                break;
   
            case '"':
                atom = "";
                getAtomString(ch, atom);
                token = new Token();
                token->assic = atom;
                token->type = T_STRING;
                token->lineno = line;
                tokenStream->pushToken(token);
                break;
                
            case '>':
                token = new Token();
                token->lineno = line;
                token->type = T_OP;
                if (((ch = getChar()) != EOF) && (ch == '>')) {
                    token->assic = ">>";
                }
                else {
                    token->assic = ">";
                    putChar(ch);
                }
                tokenStream->pushToken(token);
                break;
                
            case '<':
                token = new Token();
                token->lineno = line;
                token->type = T_OP;
                if (((ch = getChar()) != EOF) && (ch == '>')) {
                    token->assic = "<<";
                }
                else {
                    token->assic = "<";
                    putChar(ch);
                }
                tokenStream->pushToken(token);
                break;
                
            case '\r':
            case '\n':
                line ++;
                break;
                
            default:
                if (isnumber(ch)) {
                    token = parseDigitLiteral(ch);
                    tokenStream->pushToken(token);
                }
                else if (isalpha(ch)){
                    atom = "";
                    getAtomString(ch, atom);
                    if ((token = parseKeyWord(atom)) != NULL) {
                        token->lineno = line;
                    }
                    else {
                        token = new Token();
                        token->assic = atom;
                        token->type = T_ID;
                        token->lineno = line;
                        tokenStream->pushToken(token);
                    }
                }
                else {
                    // throw a exception
                }
                break;
        }
    }
    return true;
}


