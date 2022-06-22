#ifndef PARSER_H
#define PARSER_H

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

class Parser{
    vector<string> productionRules;
    char startChar; 
    vector<char> nonTerminators;
    vector<char> Terminators;
    vector<set<char>> pFirstSets;   // the same size of productionRules
    vector<set<char>> nFirstSets;   // the same size of nonTerminators
    vector<set<char>> nFollowSets;  // the same size of nonTerminators
    vector<vector<int>> analysisTable;

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
    void printCurrentStatus(const string& s1, const string& s2, const string& s3);
    void AnalyseSentence(const string& sentence);
    void run(char* inputfile);		// interface for main function
};

#endif