#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "DataType.h"


class Translator{
    vector<string> productionRulesWithActions;
    int indexOf(const vector<char>& v, char x);     // return index of x in v

    void getFirstOfNonTerminator(char nonTerminator);   // get first set of nonterminator 
    void getFirstOfProductionRules();                   // get first set of terminator 
    set<char> firstBate(const string& remain);


public:
    Translator();
    void printCurrentStatus(const string& s1, const string& s2, const string& s3, const string& s4);
    void printQuat(const Quat& q);
    void AnalyseSentence(const string& sentence);
    void run(Tokens& tokens, vector<Quat>& quats);	// interface for main function
};

#endif