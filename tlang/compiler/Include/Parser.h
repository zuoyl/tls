//
//  Parser.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_PARSER_H
#define TCC_PARSER_H

#include "Common.h"
#include "Grammar.h"
#include "Location.h"

#define LIBXML_TREE_ENABLED
#define LIBXML_OUTPUT_ENABLED
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>

class TokenStream;
class Token;
class ParserTree;

class Node 
{
public:
    string type; 
    string assic;
    Location location;
    Node *parent;
    vector<Node *> childs;
public:
    Node();
    Node(const string &type, const string &value, Location &location); 
    ~Node();
    int count();
    void addChild(Node *node);
};


class Parser 
{
public:
    Parser(const string &path, const string &file);
    ~Parser();
    bool prepare(); 
    Node * build(TokenStream *tokenStream);
    
private:
    bool pushToken(Token *token);
    void push(GrammarNonterminalState *state, int nextState,  Token *token);
    void shift(int nextState, Token *token);
    void reduce(GrammarNonterminalState *state); 
    void popup();
    int  classify(Token *token);
    bool isStateFinish(GrammarNonterminalState *nonterminalState); 
    bool isLabelInState(int label, GrammarState *state);
    void outputParseTree(Node *node, xmlNodePtr xmlNode);    
    GrammarNonterminalState* findBestMatchedNonterminal(map<GrammarNonterminalState *, int> &nonterminals, int &nextState);
private:
    // stack item
    struct Item 
    {
        GrammarNonterminalState *state; 
        int stateIndex;   
        int label;
        Node *node;
        Token *token;
    };
    
private:
    string m_path;
    string m_file;
    string m_fullFileName; 
    Grammar *m_grammar;
    Node *m_root;
    Node *m_curNode;
    int m_start;
    stack<Item > m_items;
    TokenStream *m_tokens; 
    // for xml output
    xmlNodePtr m_xmlRootNode;
    xmlDocPtr m_xmlDoc;
};

#endif // TCC_PARSER_H
