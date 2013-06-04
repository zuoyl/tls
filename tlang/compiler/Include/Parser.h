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
    int type;
    std::string assic;
    Location location;
    Node *parent;
    vector<Node *> childs;
public:
    Node();
    Node(int type):location(-1) { this->type = type; }
    Node(int type, std::string &value, Location &location);
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
    void shift(int nextState, Token *token);
    void push(GrammarStateEntry *entry, int nextState, int symbolId, Token *token);
    void popup();
    int  classify(Token *token);
    bool isLabelInState(int label, GrammarStateEntry *stateEntry);
    void outputParseTree(Node *node, xmlNodePtr xmlNode);    
private:
    // stack item
    struct Item {
        GrammarStateEntry *stateEntry;
        int stateIndex;
        Node *node;
        int labelId;
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
    // for xml output
    xmlNodePtr m_xmlRootNode;
    xmlDocPtr m_xmlDoc;
};

#endif // TCC_PARSER_H
