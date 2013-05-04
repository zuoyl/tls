//
//  FA.cpp
//  A toyable language compiler (like a simple c++)

#include "FA.h"

NFA::~NFA() 
{
    // delete all arcs
    vector<pair<string, NFA*> >::iterator ite;
    for (ite = m_arcs.begin(); ite != m_arcs.end(); ite++) {
        pair<string, NFA *> item = *ite;
        if (item.second)
            delete item.second;
    }
    m_arcs.clear();
    
}

void NFA::arc(NFA *to, const std::string &label) 
{
    std::pair<string, NFA *> item;
    item.first = label;
    item.second = to;
    m_arcs.push_back(item);
}

void NFA::arc(NFA *to, const char *label) 
{
    std::pair<string, NFA*> item;
    if (!label)
        item.first = string("");
    else 
        item.first = label;
    item.second = to;
    m_arcs.push_back(item);
}

NFA& NFA::operator=(NFA &rhs) 
{
    // delete all arcs  and insert the new arcs
    // dummy
    return *this;
    
}

void NFA::findUnlabeldState(vector<NFA *> &nfaset) 
{
    // check to see wether myself is in the state
    vector<NFA *>::iterator ite;
    for (ite = nfaset.begin(); ite < nfaset.end(); ite++) {
        if (this == *ite) 
            return;
    }
    // add myself into the set
    nfaset.push_back(this);
    
    vector<pair<string, NFA *> >::iterator it;
    
    for (it = m_arcs.begin(); it < m_arcs.end(); it++) {
        pair<string, NFA *> ip = *it;
        if (!ip.first.empty())
            ip.second->findUnlabeldState(nfaset);
    }
}


DFA::DFA(vector<NFA *> &nfaset, NFA *finalState) 
{
    m_nfas = nfaset;
    vector<NFA*>::iterator ite = nfaset.begin();
    for (; ite < nfaset.end(); ite++) {
        if (finalState == *ite) {
            m_isFinal = true;
            break;
        }
    }
}
DFA::~DFA() {
}

void DFA::arc(DFA *to, string &label) 
{
    m_arcs[label] = to;
}


bool DFA::operator == (DFA &rhs) 
{
    if (rhs.m_isFinal != m_isFinal)
        return false;
    if (rhs.m_first != m_first)
        return false;
    if (rhs.m_arcs.size() != rhs.m_arcs.size())
        return false;
    
    std::map<std::string, DFA*>::iterator ite;
    for (ite = m_arcs.begin(); ite != m_arcs.end(); ite++) {
        if (m_arcs[ite->first] != rhs.m_arcs[ite->first])
            return false;
    }
    
    return true;
}
/// check to see wether the two NFAset is same
bool isSameNFASet(vector<NFA*> &nfas1, vector<NFA*> &nfas2) 
{
    if (nfas1.size() != nfas2.size())
        return false;
    
    for (int i = 0; i < nfas1.size(); i++) {
        if (nfas1[i] != nfas2[i])
            return false;
    }
    return true;
}

/// convert a NFA to a DFA
vector<DFA*>* convertNFAToDFA(NFA *start, NFA *end) 
{
    // from the start state, find all unlabeled state
    vector<NFA*> baseNFAs;
    start->findUnlabeldState(baseNFAs);
    // allocate a stack, and push the unlabeled state into stack
    vector<DFA*> *stack = new vector<DFA *>();
    stack->push_back(new DFA(baseNFAs, end));
    
    // iterate the stack
    for (int i = 0; i < stack->size(); i++) {
        
        DFA *state = stack->at(i);
        vector<NFA *> &nfas = state->m_nfas;
        
        // holder for arcs that start with DFA start state
        vector<pair<string, vector<NFA*>*> > arcs;;
        // iterate current DFA
        vector<NFA *>::iterator ite;
        for (ite = nfas.begin(); ite < nfas.end(); ite++) {
            NFA * nfa = *ite;
            // for each NFA
            for (int arcIndex = 0; arcIndex < nfa->m_arcs.size(); arcIndex++) {
                pair<string, NFA *> ip = nfa->m_arcs[arcIndex];
                if (!ip.first.empty()) {
                    vector<NFA *> *nfaset = new vector<NFA *>();
                    ip.second->findUnlabeldState(*nfaset);
                    arcs.push_back(make_pair(ip.first, nfaset));
             
                }
            }
        }
        
        // for all arcs
        vector<pair<string, vector<NFA*>*> >::iterator it;
        for (it = arcs.begin(); it != arcs.end(); it++) {
            string label = (*it).first;
            vector<NFA*> *nfaset = (*it).second;
            // check to see wether the state is in stack
            int i = 0;
            for (; i < stack->size(); i++) {
                if (isSameNFASet(stack->at(i)->m_nfas, *nfaset))
                    break;
            }
            if (i > stack->size()) {
                DFA * newState = new DFA(*nfaset, end);
                stack->push_back(newState);
                state->arc(newState, label);
            }
        }
    }
    return stack;
}

void simplifyDFA(const string &name, DFA *dfa) 
{
   
}
