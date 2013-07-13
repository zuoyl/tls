//
//  Parser.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_PARSER_H
#define TCC_PARSER_H

#include "Common.h"
#include "Grammar.h"
#include "Location.h"

class TokenStream;
class Token;
class ParserTree;

class Node 
{
public:
    string name; // node name, such as terminal, or nonterminal  
    string assic; // token's assic
    int type;     // tokens' type 
    Location location;
    Node* parent;
    vector<Node* > childs;
public:
    Node();
    Node(const string& name, int type, const string& value, Location& location); 
    ~Node();
    int count();
    void addChild(Node* node);
};


class Parser 
{
public:
    Parser(const string& path, const string& file);
    ~Parser();
    bool prepare(); 
    Node*  build(TokenStream* tokenStream);
    
private:
    bool pushToken(Token* token);
    void push(GrammarNonterminalState* state, int nextState,  Token* token);
    void shift(int nextState, Token* token);
    void reduce(GrammarNonterminalState* state); 
    void popup();
    int  classify(Token* token);
    bool isFinalState(GrammarNonterminalState* nonterminalState, 
            GrammarState* state); 
    void outputParseTree(Node* node, xmlNodePtr xmlNode);    
    GrammarNonterminalState* selectNonterminal(
            map<GrammarNonterminalState* , int>& nonterminals, 
            Token* token); 
    bool tryNonterminal(GrammarNonterminalState* state, Token* token);
    bool recoveryError(GrammarNonterminalState* nonterminal, 
            GrammarState* state,
            Token* token);
private:
    // stack item
    struct Item 
    {
        GrammarNonterminalState* state; 
        int stateIndex;   
        int label;
        Node* node;
        Token* token;
    };
    
private:
    string m_path;
    string m_file;
    string m_fullFileName; 
    Grammar* m_grammar;
    Node* m_root;
    Node* m_curNode;
    int m_start;
    stack<Item > m_items;
    TokenStream* m_tokens; 
    // for xml output
    xmlNodePtr m_xmlRootNode;
    xmlDocPtr m_xmlDoc;
    // alternative nonterminal's match
    stack<Item> m_alternative;
};

#endif // TCC_PARSER_H
