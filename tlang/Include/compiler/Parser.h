//
//  Parser.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_PARSER_H
#define TCC_PARSER_H

#include "compiler/Common.h"
#include "compiler/Grammar.h

class TokenStream;
class Grammar;
class Token;
class ParserTree;
class NFA;
class DFA;


class Node {
public:
    int type;
    std::string assic;
    int lineno;
    int column;
    Node *parent;
    std::vector<Node *> childs;
public:
    Node();
    Node(int type) { this->type = type; }
    Node(int type, std::string &value, int lineno, int column);
    ~Node();
    int count();
    void addChild(Node *node);
};


class Parser {
public:
    Parser(Grammar *grammar);
    ~Parser();
    Node * parse(TokenStream *tokenStream);
    
private:
    bool pushToken(Token *token);
    void shift(int nextState, Token *token);
    void push(GrammarStateEntry entry, int nextState, int symbolId, Token *token);
    void popup();
    int  classify(Token *token);
    bool isLabelInState(int label, GrammarStateEntry *stateEntry);
    
private:
    // statc item
    struct StackItem {
        GrammarStateEntry stateEntry;
        int stateIndex;
        Node *node;
        int symbolId;
        Token *token;
    };
    StackItem &getStackTopReference();
    
private:
    Grammar *m_grammar;
    Node *m_root;
    Node *m_curNode;
    int m_start;
    std::vector<StackItem > m_stack;
};


#endif // TCC_PARSER_H
