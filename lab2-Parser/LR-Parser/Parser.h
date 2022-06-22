#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
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

struct action{
    char para;
    char val;
};

class Parser{
public:
    void printTable();
    void printCurrentStatus(const string& s1, const string& s2, const string& s3);
    void AnalyseSentence(const string& sentence);
    void run(char* inputfile);		// interface for main function
};


#endif