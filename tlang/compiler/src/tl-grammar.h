//
//  tl-grammar.h
//  A toyable language compiler (like a simple c++)
//  @author:jenson.zuo@gmail.com

#ifndef __TL_GRAMMAR_H__
#define __TL_GRAMMAR_H__

#include "tl-fa.h"
#include "tl-token.h"

namespace tlang {

    // GrammarState will be used by parser
    struct GrammarState {
        map<int, int> arcs;
        bool isFinal;
    };

    struct GrammarNonterminalState {
        string name;
        vector<GrammarState> states;
        vector<int> first;
        vector<int> follow;
    };

    class Grammar {
        public:
            enum { Terminal, Nonterminal };
            static const int endmark = -1;
            static const int epsilon = -1;
        public:
            // get instance for Grammar 
            static Grammar& getInstance();
           
            // parse the grammar file and construct the internal data 
            bool build(const string &fullFileName);
           
            // get states
            GrammarNonterminalState* getNonterminalState(int index);
           
            // the parser get first nonterminal index 
            int getStartStateIndex();
          
            // get symbol id
            int getSymbolID(const string &name);

            // get symbol id for the specified kind token name
            int getSymbolID(int kind, const string &name);
           
            // get the symbol name  
            void getSymbolName(int symbol, string& name);
           
            // check wether the label is nonterminal 
            bool isNonterminal(int id);
            
            // check wether the label is terminal  
            bool isTerminal(int label);
           
            // check wether the specified token is keyword 
            bool isKeyword(const string &w);

            // check wether the specified token is operator 
            bool isOperator(const string &w);
        private:
            Grammar();
            ~Grammar();

            bool parseGrammarFile(const string &file);
            void parseRule(string &ruleName, NFA **start, NFA **end);
            void parseAlternative(string &ruleName, NFA **start, NFA **end);
            void parseItems(string &ruleName, NFA **start, NFA **end);
            void parseItem(string &ruleName, NFA **start, NFA **end);
            void parseAtom(string &ruleName, NFA **start, NFA **end);
            
            void match(int type,Token **token = NULL);
            bool isMatch(int type, const char *name = NULL);
            void match(int type, const char *name = NULL);
            void advanceToken(Token **token = NULL);
            
            int  makeLabel(string &label);
            int  getStateIndex(vector<DFA*> *dfas, DFA *dfa);
            void stripLabel(string &label);
            
            void initializeBuiltinIds();
            void dumpAllBuiltinIds();    
            bool isKeyword(int id); 
            void dumpNFAs(const string &name, NFA *start, NFA *end);
            void dumpDFAs(const string &name, vector<DFA*> &dfas);
            void dumpDFAsToXml(); 
            void dumpDFAXml(xmlNodePtr node, DFA *dfa); 
            void makeNonterminalState(const string &name, vector<DFA* > &dfas);
            
            // first and follow 
            void makeFirst(const string &name, vector<int> &result);
            void makeFollow(const string &name, vector<int> &result);
            void getFirst(const string &name, vector<int> &result); 
            // make final parse table
            void makeFinalParseTable();
            bool isFirstSymbol(DFA *dfa, int symbol);
        private:
            // token holder for all tokens from grammar file 
            TokenStream m_tokens;
           
            // all grammar symbols place holder, nonterminals are biger than 256 
            vector<int>      m_labels;            
            map<string, int> m_symbols;
            map<int, string> m_symbolName;
            
            // nonterminals name and dfa map 
            map<string, vector<DFA*>* > m_dfas;
            map<string, vector<int> > m_first; 
            map<string, vector<int> > m_follow; 
            
            // first state 
            int m_start;  
            // all states, key is nonterminal id
            map<int, GrammarNonterminalState* > m_states;
            
            // first nonterminal 
            string m_firstNonterminal;           
            
            // terminal label and name map 
            map<string, int> m_terminals; 
            map<int, string> m_terminalName;
           
            // nonterminal label and name map
            map<string, int> m_nonterminals;   
            map<int, string> m_nonterminalName; 
            map<string, int> m_nonterminalState;

            // keyword label and name map
            map<string, int> m_keywords; 
            map<int, string> m_keywordName; 

            // operator label and name map 
            map<string, int> m_operators;
            map<int, string> m_operatorName; 

            // flag to indicate wether the grammar is initialized
            static bool m_isInitialized;
            // for xml output
            xmlNodePtr m_xmlRootNode;
            xmlDocPtr m_xmlDoc;
        };
} // namespace tlang 
#endif // __TL_GRAMMAR_H__
