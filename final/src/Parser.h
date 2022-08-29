#ifndef PARSER_H
#define PARSER_H

#include "DataType.h"

class Parser{
    vector<char> nonTerminators;
    vector<char> Terminators;
    vector<set<char>> pFirstSets;   // the same size of productionRules
    vector<set<char>> nFirstSets;   // the same size of nonTerminators
    vector<set<char>> nFollowSets;  // the same size of nonTerminators

    int indexOf(const vector<char>& v, char x);

    void getFirstOfNonTerminator(char nonTerminator);
    void getFirstOfProductionRules();
    set<char> firstBate(const string& remain);
public:
    Parser();
    void printCharacters();
    void printFirst();
    void printFollow();
    void printTable();
    void printC(char c);
    void printCurrentStatus(const string& s1, const string& s2, char c);
    void AnalyseSentence(const string& sentence);
    void run(Tokens& tokens);		// interface for main function
};

#endif