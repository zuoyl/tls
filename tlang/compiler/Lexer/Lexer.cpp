//
//  SimpleLexer.cpp
//  A toyable language compiler (like a simple c++)


#include "Common.h"
#include "Lexer.h"
#include "Tokens.h"
#include "Grammar.h"
using namespace std;

SimpleLexer::SimpleLexer()
{
    m_file = "";
}

SimpleLexer::SimpleLexer(const string& path, const string &file)
{
    m_file = file;
    m_path = path;
    if (!m_path.empty()) {
        m_fullFileName = m_path;
        m_fullFileName += "/";
        m_fullFileName += m_file;
    }
    else
        m_fullFileName = m_file;

}

SimpleLexer::~SimpleLexer()
{
    m_file = "";
    if (m_ifs)
        m_ifs.close();
}

char SimpleLexer::getChar()
{
    return m_ifs.get();
}
void SimpleLexer::putChar(char ch)
{
    m_ifs.unget();
}


Token * SimpleLexer::parseKeyWord(std::string &name)
{
    Token *token = NULL;
    Grammar &grammar = Grammar::getInstance(); 
    
    if (grammar.isKeyword(name)) {
        token = new Token();
        token->assic = name;
        token->type = T_KEYWORD;
    }
    return token;
}

void SimpleLexer::getAtomString(char ch, std::string &name)
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

Token * SimpleLexer::parseDigitLiteral(char ch)
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
    token->assic = digit; 
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
    else  putChar(ch);

    return token;
}

bool SimpleLexer::parse(TokenStream *tokenStream)
{
    Token *token = NULL;
    char ch;
    int lineno = 0;
    std::string atom = "";
    
    if (m_fullFileName.empty()) {
        Error::complain("the source file is not specified rightly\n");
        return false;
    }
    m_ifs.open(m_fullFileName.c_str(), ios::in);
    if (!m_ifs.is_open()) {
        Error::complain("the source file %s can not open\n");
        return false;
    }
    while ((ch = getChar()) != EOF) {
        switch (ch) {
            case '+':
                token = new Token();
                token->type = T_OP;
                if (((ch = getChar()) != EOF) && (ch == '+')) {
                    token->assic = "++";
                }
                else {
                    token->assic = "+";
                    putchar(ch);
                }
                token->location.setLineno(lineno);
                tokenStream->pushToken(token);
                break;
                
            case '-':
                token = new Token();
                token->type = T_OP;
                if (((ch = getChar()) != EOF) && (ch == '-')) {
                    token->assic = "--";
                }
                else  {
                    token->assic = '-';
                    putchar(ch);
                }
                token->location.setLineno(lineno);
                tokenStream->pushToken(token);
                break;
                
            case '*':
                token = new Token(ch, T_OP, lineno);;
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
                    token = new Token('/', T_OP, lineno);
                    putChar(ch);
                    tokenStream->pushToken(token);
                }
                break;
                
            case '&':
                token = new Token();
                token->type = T_OP;
                if (((ch = getChar()) != EOF) && (ch == '&')) {
                    token->assic = "&&";
                }
                else  {
                    token->assic = '&';
                    putchar(ch);
                }
                token->location.setLineno(lineno);
                tokenStream->pushToken(token);
                break;
     
            case '!':
                token = new Token();
                token->type = T_OP;
                token->location.setLineno(lineno);
                if (((ch = getChar()) != EOF) && (ch == '=')) {
                    token->assic = "!=";
                }
                else  {
                    token->assic = '!';
                    putchar(ch);
                }                
                tokenStream->pushToken(token);
                break;
            case '=':
            case ';':
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
                token = new Token(ch, T_OP, lineno);
                tokenStream->pushToken(token);
                break;
   
            case '"':
                atom = "";
                getAtomString(ch, atom);
                token = new Token();
                token->assic = atom;
                token->type = T_STRING;
                token->location.setLineno(lineno);
                tokenStream->pushToken(token);
                break;
                
            case '>':
                token = new Token();
                token->type = T_OP;
                token->location.setLineno(lineno);
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
                token->location.setLineno(lineno);
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
                lineno ++;
                break;
            case ' ':
                continue;
                break;
            default:
                if (isnumber(ch)) {
                    token = parseDigitLiteral(ch);
                    token->location.setLineno(lineno);
                    tokenStream->pushToken(token);
                }
                else if (isalpha(ch)){
                    atom = ch;
                    while ((ch = getChar()) != EOF) {
                        if (isalpha(ch) || isnumber(ch))
                            atom += ch;
                        else {
                            putChar(ch);
                            break;
                        }
                    }
                    if (atom.empty())
                        Error::complain("%s:%d invalid token\n", m_fullFileName.c_str(), lineno);

                    if ((token = parseKeyWord(atom)) != NULL) {
                        token->location.setLineno(lineno);
                        tokenStream->pushToken(token);
                    }
                    else {
                        token = new Token(atom.c_str(), T_ID, lineno);
                        tokenStream->pushToken(token);
                    }
                }
                else {
                    Location location(lineno);
                    Error::complain(location, "Unknown character %c\n", ch);
                }
                break;
        }
    }
    return true;
}


