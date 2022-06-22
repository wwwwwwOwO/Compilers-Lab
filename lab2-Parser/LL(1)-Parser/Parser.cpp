#include "Parser.h"
#define WIDTH 8

Parser::Parser()
{
    ifstream grammar("grammar.txt", ios::in);
	if (!grammar.good()) {
		cout << "Fail to open file \"grammar.txt\"."<< endl;
		return;
	}

    cout << "Grammar: " << endl;

    string temp;
    while(getline(grammar,temp))
    {
        cout << temp << endl;
        productionRules.push_back(temp);
    }
    grammar.close();

	bool visited[256] = {0};
    // the left most element of each productionRule is nonTerminator
    for(const auto& productionRule : productionRules) 
    {
        pFirstSets.push_back(set<char>());
        if(!visited[productionRule[0]])
        {
            nonTerminators.push_back(productionRule[0]);
            nFirstSets.push_back(set<char>());
            nFollowSets.push_back(set<char>());
            visited[productionRule[0]] = true;
        }
    }

	// each character is either nonTerminator or terminator, so the rest of characters are terminators
    for(const auto& productionRule : productionRules)
    {
        // the format of the first 3 characters of the production rule is like "E->"
        for(int i = 3; i < productionRule.length(); ++i) 
        {
            if(!visited[productionRule[i]])
            {
                Terminators.push_back(productionRule[i]);
                visited[productionRule[i]] = true;
            }
        }
    }

	Terminators.push_back('$');
	startChar = productionRules[0][0];

    printCharacters();

    for(char nonTerminator : nonTerminators)
        getFirstOfNonTerminator(nonTerminator);
    getFirstOfProductionRules();

    printFirst();

    bool isSameSize;
    set<pair<int, int>> toDo;
    vector<int> nFollowSetsSize(nFollowSets.size());
	nFollowSets[indexOf(nonTerminators, startChar)].insert('$');
    for(int index = 0; index < nonTerminators.size(); ++index)
    {
        for(const auto& productionRule : productionRules)
        {
            int indexHead = indexOf(nonTerminators, productionRule[0]); // the nonTerminators conresponding to this production rule
            bool hasat;
            for(int i = 3; i < productionRule.length(); ++i)
            {
                if(productionRule[i] != nonTerminators[index])
                    continue;
                hasat = false;
                for(auto d : firstBate(productionRule.substr(i + 1)))
                {
                    if(d == '@')
                        hasat = true;
                    else
                        nFollowSets[index].insert(d);
                }
                if(hasat && indexHead != index)
                    toDo.insert(make_pair(indexHead, index));
            }
        }
    }


	do {
        for(int i = 0; i < nFollowSets.size(); ++i)
        {
            nFollowSetsSize[i] = nFollowSets[i].size();
        }
		for (auto p : toDo) {
            for(auto c : nFollowSets[p.first])
                nFollowSets[p.second].insert(c);
		}

        isSameSize = true;
        for(int i = 0; i < nFollowSets.size(); ++i)
        {
            if (nFollowSetsSize[i] != nFollowSets[i].size())
            {
                isSameSize = false;
                break;
            }
        }
	} while (!isSameSize); // repeat if not consistent

    printFollow();

    bool hasEmpty;
    analysisTable.resize(nonTerminators.size(), vector<int>(Terminators.size(), -1));

    for(int indexProductionRule = 0; indexProductionRule < productionRules.size(); ++indexProductionRule) 
    {
        hasEmpty = false;
        int indexNonTerminator = indexOf(nonTerminators, productionRules[indexProductionRule][0]); // the i th production rule
        for(char c : pFirstSets[indexProductionRule])
        {
            if(c == '@')
            {
                hasEmpty = true;
            }
            else
            {
                analysisTable[indexNonTerminator][indexOf(Terminators, c)] = indexProductionRule;
            }
        }
        if(hasEmpty)
        {
            for(char c : nFollowSets[indexNonTerminator])
            {
                analysisTable[indexNonTerminator][indexOf(Terminators, c)] = indexProductionRule;
            }
        }
    }
    printTable();
}

void Parser::getFirstOfNonTerminator(char nonTerminator)
{
    bool keep;
    int index = indexOf(nonTerminators, nonTerminator);
    for(const auto & productionRule : productionRules)
    {
        if(productionRule[0] != nonTerminator)
            continue;
        keep = true;

        for(char c : productionRule.substr(3)) 
        {
            if(!keep) 
                break;
            if(indexOf(Terminators, c) != -1) // c is a terminator, add it to first set
            {
                nFirstSets[index].insert(c);
                keep = false;
            }
            else    // add c's first set to first set
            {
                getFirstOfNonTerminator(c);
                keep = false;
                for(char b : nFirstSets[indexOf(nonTerminators, c)])
                {
                    if(b == '@')
                        keep = true;
                    else
                        nFirstSets[index].insert(b);
                }
            }

            if(keep)
                nFirstSets[index].insert('@');
        }
    } 
}

void Parser::getFirstOfProductionRules()
{
    bool keep;
    for(int index = 0; index < productionRules.size(); ++index)
    {
        keep = true;
        for(char c : productionRules[index].substr(3))
        {
            if (!keep)
                break;
            if(indexOf(Terminators, c) != -1) // c is a terminator, add it to first set
            {
                pFirstSets[index].insert(c);
                keep = false;
            }
            else
            {
                keep = false;
                for (char b : nFirstSets[indexOf(nonTerminators, c)]){
                    if (b == '@')
                        keep = true;
                    else
                        pFirstSets[index].insert(b);
                }
            }
            if(keep)
                pFirstSets[index].insert('@');
        }
    }
}

set<char> Parser::firstBate(const string& remain){
    set<char> res;
    bool keep = true;

	for (char c : remain) {
		if (!keep)
			break;
		if(indexOf(Terminators, c) != -1) // c is a terminator
        {
			res.insert(c);
			keep = false;
		}
		else { 
            keep = false;
            for (char b : nFirstSets[indexOf(nonTerminators, c)]){
                if (b == '@')
                    keep = true;
                else
                    res.insert(b);
            }
		}
	}
	if (keep)
		res.insert('@');
    return res;
}

// interface for main program
void Parser::run(char* inputfile) {
	ifstream in(inputfile);
	if (!in.good()) {
		cout << "Fail to open file \"" << inputfile << "\""<< endl;
		return;
	}

    string sentence;
	while(getline(in, sentence)) 
    {
        this->AnalyseSentence(sentence);
    }
}

void Parser::AnalyseSentence(const string& sentence)
{
    cout << "Analysing: " + sentence << endl;

    string str(sentence);
    str.push_back('$');
    char top;

    string stk;
    stk.push_back('$');
    stk.push_back(startChar);

	cout << setw(WIDTH * 2) << "stack" << setw(WIDTH * 2) << "input" << setw(WIDTH * 2) << "operation" << endl;
	
	printCurrentStatus(stk, str, string("Initialize"));
	for (int i = 0;i < str.size();) {
		top = stk.back();
		stk.pop_back();

		if (top == '$') {
			if (str[i] == '$') {
				printCurrentStatus("$", "$", string("Accept!"));
				cout << "Analyze successfully!\n" << endl;
				break;
			}
			else {
				printCurrentStatus(stk, str.substr(i), string("Error!"));
				cerr << "Top of stack is '$', but input is not fully processed." << endl;
				return;
			}
		}
		else if (indexOf(Terminators, top) != -1) {
			if (top == str[i]) {
				++i; //输入字符提取走
				printCurrentStatus(stk, str.substr(i), string("Match ") + top);
			}
			else {
				printCurrentStatus(stk, str.substr(i), string("Error!"));
				cerr << "Top of stack is a terminator '" << top << "', but input character is '" << str[i] << "'" << endl;
				return;
			}
		}
		else if (indexOf(nonTerminators, top) != -1) {
			int indexProductionRule = analysisTable[indexOf(nonTerminators, top)][indexOf(Terminators, str[i])];
			if (indexProductionRule != -1) {
				if (productionRules[indexProductionRule][3] != '@') { // 如果右边 是 @，那么不向栈中增加东西
					for (int j = productionRules[indexProductionRule].size() - 1; j >= 3; --j) {
						stk.push_back(productionRules[indexProductionRule][j]);
					}
				}
				printCurrentStatus(stk, str.substr(i), productionRules[indexProductionRule]);
			}
			else {
				printCurrentStatus(stk, str.substr(i), string("Error!"));
				cerr << "analysisTable[" << top << "][" << str[i] << "] is empty." << endl;
				return;
			}
		}
		else {
			cerr << "Analyze failed.\n" << endl;
			return;
		}
	}
}

int Parser::indexOf(const vector<char>& v, char x) {
	vector<char>::const_iterator it = find(v.begin(), v.end(), x);
	return it == v.end() ? -1 : it - v.begin();
}

void Parser::printCharacters()
{
    cout << "Terminators with size of " << Terminators.size() << ": {";
    for(char c : Terminators)
        cout << c << " ";
    cout << "}"<< endl;
    cout << "nonTerminators with size of " << nonTerminators.size() << ": {";
    for(char c : nonTerminators)
        cout << c << " ";
    cout << "}"<< endl;
}

void Parser::printFirst()
{
    cout << "First set of production rules" << endl;
    for(int i = 0; i < pFirstSets.size(); ++i)
    {
        cout << productionRules[i] << ": {";
        for(char c : pFirstSets[i])
            cout << c << " ";
        cout << "}"<< endl;
    }
    cout << endl;

    cout << "First set of nonTerminators" << endl;
    for(int i = 0; i < nFirstSets.size(); ++i)
    {
        cout << nonTerminators[i] << ": { ";
        for(char c : nFirstSets[i])
            cout << c << " ";
        cout << "}"<< endl;
    }
    cout << endl;
}

void Parser::printFollow()
{
    cout << "Follow set of nonTerminators" << endl;
    for(int i = 0; i < nFollowSets.size(); ++i)
    {
        cout << nonTerminators[i] << ": { ";
        for(char c : nFollowSets[i])
            cout << c << " ";
        cout << "}"<< endl;
    }
    cout << endl;
}

void Parser::printTable()
{
    cout << "Analysis Table" << endl;
    cout << setw(WIDTH) << " ";
    for(auto c : Terminators)
    {
        cout << setw(WIDTH) << c ;
    }
    cout << endl;
    for(int x = 0; x < analysisTable.size(); ++x)
    {
        cout << setw(WIDTH) << nonTerminators[x];
        for(const auto& i: analysisTable[x])
        {
            if(i != -1)
                cout << setw(WIDTH) << productionRules[i] ;
            else
                cout << setw(WIDTH) << " " ;
        }
        cout << endl;
    }
}

void Parser::printCurrentStatus(const string& s1, const string& s2, const string& s3) {
	cout << setw(WIDTH * 2) << s1 << setw(WIDTH * 2) << s2 << setw(WIDTH * 2) << s3 << endl;
}