//
//  TParser.h
//  A toyable language compiler (like a simple c++)
//  The file should be included by tcc parser

#ifndef TCC_TGRAMMAR_H
#define TCC_TGRAMMAR_H

#include <string>
#include <vector>
#include <map>

using namespace std;

struct TState 
{
    vector<pair<int, int> > arcs;
    bool isFinal;
};

struct TStateEntry 
{
    vector<struct TState> states;
    vector<int> first; // should be check
};

struct TGrammar 
{
    map<string, vector<string> > first; 
    vector<struct TStateEntry> states;   // all state entry
    int start;                          // start index 
    vector<int>      labels;            // all labels
    map<string, int> symbolIDs;         // symbol id for non-terminal
    map<int, string> symbolNames;       // symbol name for non-terminal
    map<string, int> symbolToLabel;     // symbol to label mapping
    map<string, int> keywordIDs;        // keyword ids
    map<string, int> operatormap;       // operator maps
    map<string, int> tokens;            // all terminal tokens, such as IDENTIFIER
    map<int, int>    tokenIDs;          // token ID and lable index mapping
};

/// 
/// @brief build tlang grammran and generate dfas
/// @param file the grammar file
/// @param grammra the output grammar state
/// @return error code, it will be greater than zero if fails, else equal zero
int buildGrammar(const string &file, TGrammar *grammar);

#endif // TCC_TGRAMMAR_HTCC_TGRAMMAR_H
