//
//  SimpleLexer.cpp
//  A toyable language compiler (like a simple c++)


#include "tl-lexer.h"
#include "tl-token.h"

using namespace tl;

SimpleLexer::SimpleLexer()
{
}

SimpleLexer::SimpleLexer(const string& path, const string& file)
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


Token*  SimpleLexer::parseKeyWord(string& name)
{
    Token* token = NULL;
    Grammar& grammar = Grammar::getInstance(); 
    
    if (grammar.isKeyword(name)) {
        token = new Token();
        token->assic = name;
        token->type = TOKEN_TYPE_KEYWORD;
    }
    return token;
}

void SimpleLexer::getAtomString(char ch, string& name)
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

Token*  SimpleLexer::parseDigitLiteral(char ch)
{
    Token* token = NULL;
    string digit = "";
    
    digit += ch;
    while ((ch = getChar()) != EOF) {
        if (!isdigit(ch)) {
            putChar(ch);
            break;
        }
        digit += ch;
    }
    token = new Token();
    token->type = TOKEN_TYPE_INT;
    token->assic = digit; 
    // check to see wether it is a float
    if ((ch = getChar()) == '.') {
        digit += ch;
        string subDigit = "";
        while ((ch = getChar()) != EOF) {
            if (!isalpha(ch)) {
                putChar(ch);
                break;
            }
            subDigit += ch;
        }
        digit += subDigit;
        token->type = TOKEN_TYPE_FLOAT;
    }
    else  putChar(ch);

    return token;
}

bool SimpleLexer::parse(TokenStream* tokenStream)
{
    Token* token = NULL;
    char ch;
    int lineno = 1;
    string atom = "";
    
    if (m_fullFileName.empty()) {
        Error::complain("the source file is not specified rightly\n");
        return false;
    }
    m_ifs.open(m_fullFileName.c_str(), ios::in);
    if (!m_ifs.is_open()) {
        Error::complain("the source file %s can not open\n", m_fullFileName.c_str());
        return false;
    }
    while ((ch = getChar()) != EOF) {
        token = NULL; 
        switch (ch) {
            case '/':
                if (((ch = getChar()) != EOF) && (ch = '/')) {
                    // consume comments
                    while (((ch = getChar()) != EOF)) {
                        if (ch == '\r' || ch == '\n') {
                            putChar(ch);
                            break; 
                        }
                    }
                }
                else {
                    token = new Token('/', TOKEN_TYPE_OP, lineno);
                    putChar(ch);
                    tokenStream->pushToken(token);
                }
                break;
                
            case '+':
            case '-':
            case '*':
            case ':':
            case '.':
            case '&':
            case '!':
            case '=':
            case '>':
            case '<':
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
            case '#':
            case '~':
            case '`':
            case '\\':
                token = new Token(); 
                token->type = TOKEN_TYPE_OP;
                token->assic = ch; 
                if (((ch = getChar()) != EOF)) {  
                    string tval = token->assic;
                    tval += ch;
                    if (Grammar::getInstance().isOperator(tval))
                        token->assic = tval;
                    else
                        putChar(ch);
                } 
                token->location.setLineno(lineno);
                tokenStream->pushToken(token);
                break;
   
            case '"':
                atom = "";
                getAtomString(ch, atom);
                token = new Token();
                token->assic = atom;
                token->type = TOKEN_TYPE_STRING;
                token->location.setLineno(lineno);
                tokenStream->pushToken(token);
                break;
                
            case '\r':
                continue;
                break;
            case '\n':
                lineno ++;
                break;
            case ' ':
                continue;
                break;
            default:
                if (isdigit(ch)) {
                    token = parseDigitLiteral(ch);
                    token->location.setLineno(lineno);
                    tokenStream->pushToken(token);
                }
                else if (isalpha(ch)){
                    atom = ch;
                    while ((ch = getChar()) != EOF) {
                        if (isalpha(ch) || isdigit(ch))
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
                        token = new Token(atom.c_str(), TOKEN_TYPE_ID, lineno);
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


