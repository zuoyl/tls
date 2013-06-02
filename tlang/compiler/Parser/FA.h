//
//  Grammar.h
//  A toyable language compiler (like a simple c++)


#ifndef TCC_NFADFA_H
#define TCC_NFADFA_H

#include <string>>
#include <vector>
#include <map>
#include <iostream>
#include <utility>

using namespace std;

class NFA 
{
public:
    NFA(){}
    ~NFA();
    void arc(NFA *to, const string &label);
    void arc(NFA *to, const char *label = NULL);
    void findUnlabeldState(vector<NFA *> &nfaset);
    NFA& operator = (NFA &rhs);
    
public:
    vector<pair<string, NFA *> > m_arcs;
    
};

class DFA 
{
public:
    DFA(vector<NFA*> &nfaset, NFA *finalState);
    ~DFA();
    void arc(DFA *to, string &label);
    void arc(DFA *to, const char *label = NULL);
    bool operator == (DFA &rhs);
    void unifyState(DFA *state1, DFA *state2);
public:
    bool m_isFinal;
    DFA *m_first;
    map<string, DFA * > m_arcs;
    vector<NFA*> m_nfas;
};

bool isSameNFASet(vector<NFA*> &nfas1, vector<NFA*> &nfas2);
vector<DFA*>* convertNFAToDFA(NFA *start, NFA *end);
void simplifyDFAs(const string &name, vector<DFA *> &dfas);


#endif // TCC_NFADFA_H
