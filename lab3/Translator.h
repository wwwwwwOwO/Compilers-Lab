#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <map>
#include <stack>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <queue>
#include <set>
#include <cassert>
#include <iomanip>
#include <ctime>
#include <list>
#include <cctype>
#include <functional>
using namespace std;

struct Quat{
    char op;
    string a, b;
    string res;
};

class Translator{
    vector<string> productionRules;
    vector<string> productionRulesWithActions;
    char startChar; 
    vector<char> nonTerminators;
    vector<char> Terminators;
    vector<set<char>> pFirstSets;   // the same size of productionRules
    vector<set<char>> nFirstSets;   // the same size of nonTerminators
    vector<set<char>> nFollowSets;  // the same size of nonTerminators
    vector<vector<int>> analysisTable;

    int indexOf(const vector<char>& v, char x);     // return index of x in v

    void getFirstOfNonTerminator(char nonTerminator);   // get first set of nonterminator 
    void getFirstOfProductionRules();                   // get first set of terminator 
    set<char> firstBate(const string& remain);


public:
    Translator();
    void printCharacters();
    void printFirst();
    void printFollow();
    void printTable();
    void printCurrentStatus(const string& s1, const string& s2, const string& s3, const string& s4);
    void printQuat(const Quat& q);
    vector<Quat> AnalyseSentence(const string& sentence);
    void run(char* inputfile);		// interface for main function
};

#endif