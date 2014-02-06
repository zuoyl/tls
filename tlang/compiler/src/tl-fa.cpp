//
//  FA.cpp
//  A toyable language compiler (like a simple c++)

#include "tl-common.h"
#include "tl-fa.h"
#include <algorithm>

#ifdef dbg
#undef dbg
#define dbg
#endif

using namespace tlang;


int NFA::m_counter = 0;
int DFA::m_counter = 0;

// helper mthod for NFA/DFA
// get epsilon closure for specified NFA
static void 
epsilonClosure(NFA *nfa, vector<NFA*> &result)
{
    assert(nfa != NULL); 
    // check to see wether the nfas is already in result
    vector<NFA* >::iterator ite = result.begin();
    for (; ite != result.end(); ite++) {
        if (*ite == nfa)
            return;
    }
    result.push_back(nfa);
    vector<pair<string, NFA*> >::iterator it = nfa->m_arcs.begin(); 
    for (; it < nfa->m_arcs.end(); it++) {
        pair<string, NFA* > ip = *it;
        if (ip.second && ip.first.empty())
            epsilonClosure(ip.second, result);
    }
}

// get epsilon closure for specified NFA set
static void 
epsilonClosure(vector<NFA*> &nfas, vector<NFA*> &result)
{
    vector<NFA*>::iterator ite = nfas.begin();
    for (; ite != nfas.end(); ite++) {
        epsilonClosure(*ite, result);
    }
}

// get a moveable state according to label in specified NFA
static void 
moveNFA(vector<NFA*> &nfas, const string &symbol, vector<NFA*> &result)
{
    vector<NFA*>::iterator ite;
    for (ite = nfas.begin(); ite != nfas.end(); ite++) {
        NFA* nfa = *ite;
        vector<pair<string, NFA*> >::iterator m;
        for (m = nfa->m_arcs.begin(); m != nfa->m_arcs.end(); m++) {
            pair<string, NFA*> item = *m;
            string label = m->first;
            NFA* next = m->second;
            if (label == symbol) 
                result.push_back(next);
        }
    }
}

// get a DFA new state according to label in specified DFA
static void 
moveDFA(DFA *dfa, const string &symbol, vector<NFA*> &result)
{
    vector<NFA*> nfas; 
    moveNFA(dfa->m_nfas, symbol, nfas);
    epsilonClosure(nfas, result); 
}

NFA::NFA()
{
    m_index = NFA::m_counter++;
}

NFA::~NFA() 
{}

void NFA::arc(NFA *to, const string &label) 
{
    pair<string, NFA*> item;
    item.first = label;
    item.second = to;
    m_arcs.push_back(item);
}

void NFA::arc(NFA *to, const char *label) 
{
    pair<string, NFA*> item;
    if (!label)
        item.first = string("");
    else 
        item.first = label;
    item.second = to;
    m_arcs.push_back(item);
}

bool NFA::operator == (NFA &rhs) 
{
    if (m_arcs.size() != rhs.m_arcs.size())
        return false;
    for (size_t index = 0; index < m_arcs.size(); index++) {
        pair<string, NFA*> left, right;
        left = m_arcs.at(index);
        right = rhs.m_arcs.at(index);
        if (left.first != right.first)
            return false; 
        if (!left.second || !right.second)
            return false;
        if (left.second->m_index != right.second->m_index)
            return false;
    }
    return true;
}


DFA::DFA()
{
    m_index = DFA::m_counter++;
    m_isFinal = false;
}

DFA::DFA(vector<NFA*> &nfas)
{
    m_index = DFA::m_counter++;
    m_nfas = nfas;
    m_isFinal = false;
}

// construct a dfa from nfa set
DFA::DFA(vector<NFA*> &nfaset, NFA *final) 
{
    m_index = DFA::m_counter++;
    m_nfas = nfaset;
    m_isFinal = false;
#if 1
    if (std::find(nfaset.begin(), nfaset.end(), final) !=
            nfaset.end())
        m_isFinal = true;

#else
    vector<NFA*>::iterator ite = nfaset.begin();
    for (; ite != nfaset.end(); ite++) {
        if ((*ite)->m_index == final->m_index) {
            m_isFinal = true;
            break;
        }
    }
#endif
}

DFA::~DFA() 
{
}

void DFA::arc(DFA *to, const string &label) 
{
    if (m_arcs.find(label) == m_arcs.end())
        m_arcs[label] = to;
}


bool DFA::operator == (DFA &rhs) 
{
    if (m_arcs.size() != rhs.m_arcs.size())
        return false;
    
    for (map<string, DFA*>::iterator ite = m_arcs.begin(); 
            ite != m_arcs.end(); ite++) { 
        string label = ite->first;
        DFA* next = ite->second;

        // wether the label is in the rhs
        if (rhs.m_arcs.find(label) == rhs.m_arcs.end())
            return false;
        if (!rhs.m_arcs[label])
            return false;

        // wether the target dfa is same 
        if (next->m_index != rhs.m_arcs[label]->m_index)
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
bool NFA::isSameNFAs(const vector<NFA*> &nfas1, const vector<NFA*> &nfas2) 
{
    if (nfas1.size() != nfas2.size())
        return false;
    
    for (size_t i = 0; i < nfas1.size(); i++) {
        if (!nfas1[i] || !nfas2[2])
            return false;
        if (!(*nfas1[i] == *nfas2[i]))
            return false;
    }
    return true;
}

#if 0
void 
DFA::dumpDFA()
{
    dbg("###DFA(%d) = {", m_index);
    vector<NFA*>::iterator ite = m_nfas.begin();
    for (; ite != m_nfas.end(); ite++) { 
        NFA* nfa = *ite;
        dbg("%d,", nfa->m_index);
    }
    dbg("}\n");
}
#endif
/// convert a NFA to a DFA
vector<DFA*>* 
DFA::convertNFAtoDFA(NFA *start, NFA *end) 
{
    if (!start || !end) return NULL; 
    
    // from the start state, make a epsilon closure 
    vector<NFA*> nfas1;
    epsilonClosure(start, nfas1);
    // allocate a stack, and push the unlabeled state into stack
    vector<DFA*>* dfas = new vector<DFA*>();
    dfas->push_back(new DFA(nfas1, end));
    // track the DFA wether it has been dealed 
    map<int, bool> marked; 

    // iterate the stack
    for (size_t index = 0; index < dfas->size(); index++) { 
        // check marked state
        if (marked[index]) continue;
        else marked[index] = true;
        
        // get current top DFA 
        DFA *dfa = dfas->at(index);
        // dor debug, dump the dfa
        // dumpDFA(dfa);
        // holder for arcs that start with DFA start state
        vector<string> labels;;
        
        // get all NFAs for the current DFA
        vector<NFA*> &nfas = dfa->m_nfas;
        // iterate current DFA to find all unlabeled arcs
        for (vector<NFA*>::iterator ite = nfas.begin(); ite != nfas.end(); ite++) {
            NFA* nfa = *ite;
            // for each NFA,iterate all arcs to find unlabeled arc 
            for (size_t index = 0; index < nfa->m_arcs.size(); index++) {
                pair<string, NFA*> ip = nfa->m_arcs[index];
                if (!ip.first.empty()) 
                    labels.push_back(ip.first);
            }
        }
        
        // for all non-null labels
        for (vector<string>::iterator ite = labels.begin(); 
                ite != labels.end(); ite++) {
            string label = *ite;
            // for the the label, get the nfa set that can arrive from the label 
            // compute epsilon-closure for the nfa set 
            vector<NFA*> result; 
            moveDFA(dfa, label, result);   
            if (!result.empty()) {
                DFA *ndfa = new DFA(result, end);
                vector<DFA*>::iterator i = dfas->begin();
                for (; i != dfas->end(); i++) {
                    DFA* sdfa =* i;
                    // if the two dfa's nfaset is same 
                    if (NFA::isSameNFAs(sdfa->m_nfas, ndfa->m_nfas)) {
                        dfa->arc(sdfa, label);
                        delete ndfa; 
                        break; 
                    }
                }
                if (i == dfas->end()){
                    dfa->arc(ndfa, label); 
                    dfas->push_back(ndfa); 
                }
            }
        }
    }
    return dfas;
}

void DFA::simplifyDFAs(const string &name, vector<DFA*> &dfas) 
{
    size_t count = dfas.size();
    DFA *dfaset[count];
    size_t index = 0; 

    vector<DFA*>::iterator ite; 
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
                    DFA* subState = dfaset[k];
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

