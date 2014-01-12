//
//  Grammar.h
//  A toyable language compiler (like a simple c++)
//  @author:jenson.zuo@gmail.com

#ifndef __TL_FA_H__
#define __TL_FA_H__

#include "tl-common.h"
#include <vector>
#include <map>
#include <iostream>
#include <utility>


namespace tlang {
    class NFA {
        public:
            NFA();
            ~NFA();
            void arc(NFA *to, const string &label);
            void arc(NFA *to, const char *label = NULL);
            bool operator == (NFA &rhs);    
        public:
            vector<pair<string, NFA*> > m_arcs;
            int m_index;
            static int m_counter;
        public:
            static bool isSameNFAs(const vector<NFA*> &nfas1, const vector<NFA*> &nfas2);
    };

    class DFA {
        public:
            DFA(); 
            DFA(vector<NFA*> &nfas); 
            DFA(vector<NFA*> &nfas, NFA *final);
            ~DFA();
            void arc(DFA *to, const string &label);
            bool operator == (DFA &rhs);
            void unifyState(DFA *state1, DFA *state2);
            void dump();
        public:
            bool m_isFinal;
            int m_first; 
            map<string, DFA*> m_arcs;  
            vector<NFA*> m_nfas;
            int m_index;
            static int m_counter;
        public:
            static vector<DFA*>* convertNFAtoDFA(NFA *start, NFA *end);
            static void simplifyDFAs(const string &name, vector<DFA*> &dfas);
    };

} // namespace tl
#endif // __TL_FA_H__
