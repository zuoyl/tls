//
//  FA.cpp
//  A toyable language compiler (like a simple c++)

#include "Common.h"
#include "FA.h"


int NFA::m_counter = 0;
int DFA::m_counter = 0;



// helper mthod for NFA/DFA
// get epsilon closure for specified NFA
static void epsilonClosure(NFA *nfa, vector<NFA*> &result)
{
    assert(nfa != NULL); 
    // check to see wether the nfas is already in result
    vector<NFA *>::iterator ite = result.begin();
    for (; ite != result.end(); ite++) {
        if (*ite == nfa)
            return;
    }
    result.push_back(nfa);
    vector<pair<string, NFA*> >::iterator it = nfa->m_arcs.begin(); 
    for (; it < nfa->m_arcs.end(); it++) {
        pair<string, NFA *> ip = *it;
        if (ip.second && ip.first.empty())
            epsilonClosure(ip.second, result);
    }
}
// get epsilon closure for specified NFA set
static void epsilonClosure(vector<NFA *> &nfas, vector<NFA *> &result)
{
    vector<NFA *>::iterator ite = nfas.begin();
    for (; ite != nfas.end(); ite++) {
        epsilonClosure(*ite, result);
    }
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
#if 0
    // delete all arcs
    vector<pair<string, NFA*> >::iterator ite;
    for (ite = m_arcs.begin(); ite != m_arcs.end(); ite++) {
        pair<string, NFA *> item = *ite;
        if (item.second)
            delete item.second;
    }
    m_arcs.clear();
    NFA::m_counter--; 
#endif
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


DFA::DFA()
{
    m_index = DFA::m_counter++;
}

DFA::DFA(vector<NFA *> &nfaset, NFA *finalState) 
{
    m_index = DFA::m_counter++;
    m_nfas = nfaset;
    vector<NFA*>::iterator ite = nfaset.begin();
    for (; ite < nfaset.end(); ite++) {
        if (finalState == *ite && 
            m_index == finalState->m_index) {
            m_isFinal = true;
            break;
        }
    }
}
DFA::~DFA() 
{
#if 0
    vector<NFA*>::iterator ite = m_nfas.begin();
    for (; ite < m_nfas.end(); ite++) {
        delete *ite;
    }
#endif
}
void DFA::arc(DFA *to, const string &label) 
{
    if (m_arcs.find(label) == m_arcs.end())
        m_arcs[label] = to;
    else
        Error::complain("There are two same arc in one DFA\n");
}


bool DFA::operator == (DFA &rhs) 
{
    if (rhs.m_isFinal != m_isFinal)
        return false;
    if (rhs.m_first != m_first)
        return false;
    if (rhs.m_arcs.size() != m_arcs.size())
        return false;
    
    map<string, DFA*>::iterator ite;
    for (ite = m_arcs.begin(); ite != m_arcs.end(); ite++) {
        string label = ite->first;
        DFA *next = ite->second;
        if (rhs.m_arcs.find(label) == rhs.m_arcs.end()
            || rhs.m_arcs[label] != next)
            return false;
    }
    
    return true;
}

// replace the old state with new state
void DFA::unifyState(DFA *oldState, DFA *newState)
{
    map<string, DFA*>::iterator ite;
    for (ite = m_arcs.begin(); ite != m_arcs.end(); ite++) {
        if (ite->second == oldState)
            m_arcs[ite->first] = newState;
    }
}

/// check to see wether the two NFAset is same
bool isSameNFAs(vector<NFA*> &nfas1, vector<NFA*> &nfas2) 
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
    epsilonClosure(start, baseNFAs);
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
                    epsilonClosure(ip.second, *nfaset);
                    arcs.push_back(make_pair(ip.first, nfaset));
                }
            }
        }
        
        // for all arcs
        vector<pair<string, vector<NFA*>*> >::iterator it;
        for (it = arcs.begin(); it != arcs.end(); it++) {
            string label = it->first;
            vector<NFA*> *nfaset = it->second;
            // check to see wether the state is in stack
            vector<DFA*>::iterator i = dfas->begin();
            for (; i != dfas->end(); i++) {
                if (isSameNFAs((*i)->m_nfas, *nfaset))
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
    size_t count = dfas.size();
    DFA* dfaset[count];
    size_t index = 0; 
    vector<DFA *>::iterator ite; 
    for (ite = dfas.begin(); ite != dfas.end(); ite++) 
        dfaset[index++] = *ite;

    for (index = 0; index < count; index++) {
        DFA *state1 = dfaset[index];
        if (!state1) continue; 
        for (size_t j = index + 1; j < count; j++) {
            DFA *state2 = dfaset[j];
            if (!state2) continue; 
            // if there are two sampe state, just delete one 
            if (*state1 == *state2) {
                for (size_t k = 0; k < count; k++){
                    DFA *subState = dfaset[k];
                    if (!subState) continue;
                    subState->unifyState(state2, state1); 
                }
                delete state2;
                dfaset[j] = NULL;
            }
        }
    }
    // after all the same state is delete, copy back
    dfas.clear();
    for (index = 0; index < count; index++) {
        if (dfaset[index])
            dfas.push_back(dfaset[index]);
    }
}

