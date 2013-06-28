//
//  Grammar.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_NFADFA_H
#define TCC_NFADFA_H

#include <vector>
#include <map>
#include <iostream>
#include <utility>

using namespace std;

class NFA 
{
public:
    NFA();
    ~NFA();
    void arc(NFA* to, const string& label);
    void arc(NFA* to, const char* label = NULL);
    bool operator == (NFA& rhs);    
public:
    vector<pair<string, NFA* > > m_arcs;
    int m_index;
    static int m_counter;
};

class DFA 
{
public:
    DFA(); 
    DFA(vector<NFA*>& nfas); 
    DFA(vector<NFA*>& nfas, NFA* final);
    ~DFA();
    void arc(DFA* to, const string& label);
    bool operator == (DFA& rhs);
    void unifyState(DFA* state1, DFA* state2);
public:
    bool m_isFinal;
    int m_first; 
    map<string, DFA* > m_arcs;  
    vector<NFA*> m_nfas;
    int m_index;
    static int m_counter;
};

bool isSameNFAs(vector<NFA*>& nfas1, vector<NFA*>& nfas2);
vector<DFA*>* convertNFAToDFA(NFA* start, NFA* end);
void simplifyDFAs(const string& name, vector<DFA* >& dfas);

#endif // TCC_NFADFA_H
