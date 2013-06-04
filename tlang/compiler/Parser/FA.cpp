//
//  FA.cpp
//  A toyable language compiler (like a simple c++)

#include "FA.h"


int NFA::m_counter = 0;
int DFA::m_counter = 0;



// helper mthod for NFA/DFA
// get epsilon closure for specified NFA
static void epsilonClosure(NFA *nfa, vector<NFA*> &result)
{
     
}
// get epsilon closure for specified NFA set
static void epsilonClosure(vector<NFA *> &nfas, vector<NFA *> &result)
{

}

// get a moveable state according to label in specified NFA
static void moveNFA(NFA *nfa, int label, vector<NFA *> &result)
{
}
// get a DFA new state according to label in specified DFA
static void moveDFA(DFA *dfa, int label, DFA **result)
{

}

NFA::NFA()
{
    m_index = NFA::m_counter++;
}
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
    NFA::m_counter--; 
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
DFA::DFA()
{
    m_index = DFA::m_counter++;
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
    m_index = DFA::m_counter++;
}
DFA::~DFA() 
{
    DFA::m_counter--;
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

void DFA::unifyState(DFA *stat1, DFA *state2)
{
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
    if (!start || !end) return NULL; 
    
    // from the start state, find all unlabeled state
    vector<NFA*> baseNFAs;
    start->findUnlabeldState(baseNFAs);
    // allocate a stack, and push the unlabeled state into stack
    vector<DFA*> *dfas = new vector<DFA*>();
    dfas->push_back(new DFA(baseNFAs, end));
   

    // iterate the stack
    for (int index = 0; index < (int)dfas->size(); index++) { 
        // get current top DFA 
        DFA *state = dfas->at(index); 
        // get all NFAs for the current DFA
        vector<NFA *> &nfas = state->m_nfas;
        // holder for arcs that start with DFA start state
        vector<pair<string, vector<NFA*>*> > arcs;;
        // iterate current DFA
        vector<NFA *>::iterator ite = nfas.begin();
        for (; ite < nfas.end(); ite++) {
            NFA *nfa = *ite;
            // for each NFA,iterate all arcs to find unlabed state
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
            vector<DFA*>::iterator i = dfas->begin();
            for (; i != dfas->end(); i++) {
                if (isSameNFASet((*i)->m_nfas, *nfaset))
                    break;
            }
            // if not found, generate a new DFA state, and arc them
            if (i == dfas->end()) {
                DFA * newState = new DFA(*nfaset, end);
                dfas->push_back(newState);
                state->arc(newState, label);
            }
            // the nfa set should be delete 
            delete nfaset;
        }
    }
    return dfas;
}

void simplifyDFAs(const string &name, vector<DFA *> &dfas) 
{
    vector<DFA *>::iterator i, j, k; 
    for (i = dfas.begin(); i != dfas.end(); i++) {
        DFA *state1 = *i;
        for (j = i + 1; j != dfas.end(); j++) {
            DFA *state2 = *j;
            // if there are two sampe state, just delete one 
            if (*state1 == *state2) {
                delete state2;
                dfas.erase(j);
                for (k = dfas.begin(); k != dfas.end(); k++){
                    DFA *subState = *k;
                    subState->unifyState(state1, state2); 
                }
            }
        }
    }
}


