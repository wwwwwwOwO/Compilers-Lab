#include "Parser.h"
#define WIDTH 12

extern vector<string> ProductionRules;
extern vector<vector<int>> AnalysisTable;
extern vector<string> productionRules;
extern map<char, Token> Char2Token;
extern map<int, char> Key2Char;
extern map<int, char> Partial2Char;
extern char startChar; 

Parser::Parser()
{
	cout << GRN"[INFO] "<< RESET;
	cout << "Initializing Parser..." << endl;


    // translate grammar
    set<char> CharSet;
    char c = 0;
    for(const auto& ProductionRule : ProductionRules) 
    {
        string newRule;
        newRule.append(ProductionRule.substr(0, 3));
        CharSet.insert(ProductionRule[0]);

        for(int i = 3; i < ProductionRule.length(); ++i) 
        {
            if(ProductionRule[i] == 'k')
            {
                ++i;
                int index = ProductionRule[i] - '0';
                if(Key2Char.find(index) == Key2Char.end())    // the character appears for the first time
                {
                    while(CharSet.find(c) != CharSet.end()) ++c;    // allocate a new character for it
                    CharSet.insert(c);                              // add it to the char set
                    Char2Token.insert(make_pair(c, Token(TokenKind::KEY, index, 0, 0)));  // create a mapping
                    Key2Char.insert(make_pair(index, c));
                }
                newRule.push_back(Key2Char[index]);
            }
            else if(ProductionRule[i] == 'p')
            {
                int index;
                if(i + 2 < ProductionRule.length() && ProductionRule[i + 2] == '0'){
                    index = 10;
                    i += 2;
                }
                else
                {
                    ++i;
                    index = ProductionRule[i] - '0';
                }

                if(Partial2Char.find(index) == Partial2Char.end())    // the character appears for the first time
                {
                    while(CharSet.find(c) != CharSet.end()) ++c;    // allocate a new character for it
                    CharSet.insert(c);                              // add it to the char set
                    Char2Token.insert(make_pair(c, Token(TokenKind::PARTIAL, index, 0, 0)));  // create a mapping
                    Partial2Char.insert(make_pair(index, c));
                }
                newRule.push_back(Partial2Char[index]);
            }
            else{
                newRule.push_back(ProductionRule[i]);
                CharSet.insert(ProductionRule[i]);
            }
        }
        productionRules.push_back(newRule);
    }



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
    {
        getFirstOfNonTerminator(nonTerminator);
    }
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
    AnalysisTable.resize(nonTerminators.size(), vector<int>(Terminators.size(), -1));

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
                AnalysisTable[indexNonTerminator][indexOf(Terminators, c)] = indexProductionRule;
            }
        }
        if(hasEmpty)
        {
            for(char c : nFollowSets[indexNonTerminator])
            {
                AnalysisTable[indexNonTerminator][indexOf(Terminators, c)] = indexProductionRule;
            }
        }
    }
    cout << BLU"[DEBUG] "<< RESET;
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
void Parser::run(Tokens& tokens) {
    string sentence;
    for(vector<Token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
        switch(it->GetKind())
        {
            case TokenKind::CONSTANT:
                sentence.push_back('c');
                break;
            case TokenKind::ID:
                sentence.push_back('i');
                break;
            case TokenKind::KEY:
                sentence.push_back(Key2Char[it->GetIndex()]);
                break;
            case TokenKind::PARTIAL:
                sentence.push_back(Partial2Char[it->GetIndex()]);
                break;
            default: break;
        }
    }
    this->AnalyseSentence(sentence);
}

void Parser::AnalyseSentence(const string& sentence)
{
	cout << GRN"[INFO] "<< RESET;
	cout << "Parsing..." << endl;
    for(auto c:sentence){
        printC(c);
    }
    cout << endl;

    string str(sentence);
    str.push_back('$');
    char top;

    string stk;
    stk.push_back('$');
    stk.push_back(startChar);

	for (int i = 0;i < str.size();) {
		top = stk.back();
		stk.pop_back();

		if (top == '$') {
			if (str[i] == '$') {
				printCurrentStatus(string("$"), string("Accept!"), str[i]);
				cout << "Analyze successfully!\n" << endl;
				break;
			}
			else {
				printCurrentStatus(stk + top, string("Error!"), str[i]);
				cerr << "Top of stack is '$', but input is not fully processed." << endl;
				return;
			}
		}
		else if (indexOf(Terminators, top) != -1) {
			if (top == str[i]) {
                printCurrentStatus(stk + top, string("Match top"), str[i]);
				++i; //输入字符提取走
			}
			else {
				printCurrentStatus(stk + top, string("Error!"), str[i]);
				cerr << "Top of stack is a terminator '" ;
                printC(top);
                cout << "', but input character is '" ;
                printC(str[i]);
                cout <<"'" << endl;
				return;
			}
		}
		else if (indexOf(nonTerminators, top) != -1) {
			int indexProductionRule = AnalysisTable[indexOf(nonTerminators, top)][indexOf(Terminators, str[i])];
			if (indexProductionRule != -1) {
                printCurrentStatus(stk + top, ProductionRules[indexProductionRule], str[i]);
				if (productionRules[indexProductionRule][3] != '@') { // 如果右边 是 @，那么不向栈中增加东西
					for (int j = productionRules[indexProductionRule].size() - 1; j >= 3; --j) {
						stk.push_back(productionRules[indexProductionRule][j]);
					}
				}
				
			}
			else {
				printCurrentStatus(stk + top, string("Error!"), str[i]);
				cerr << "AnalysisTable[" << top << "][" << str[i] << "] is empty." << endl;
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

void Parser::printC(char c){
    if(Char2Token.find(c) != Char2Token.end())
        Char2Token[c].print();
    else
        cout << setw(3)<< c;
}

void Parser::printCharacters()
{
    cout << "Terminators with size of " << Terminators.size() << ": {";
    for(char c : Terminators)
    {
        printC(c);
        cout << " ";
    }
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
        cout << setw(2) << i << ": {";
        for(char c : pFirstSets[i])
        {
            printC(c);
            cout << " ";
        }
        cout << "}"<< endl;
    }
    cout << endl;

    cout << "First set of nonTerminators" << endl;
    for(int i = 0; i < nFirstSets.size(); ++i)
    {
        cout << nonTerminators[i] << ": { ";
        for(char c : nFirstSets[i])
        {
            printC(c);
            cout << " ";
        }
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
        {
            printC(c);
            cout << " ";
        }
        cout << "}"<< endl;
    }
    cout << endl;
}

void Parser::printTable()
{
    cout << "Analysis Table" << endl;
    cout << setw(4) << " ";
    for(auto c : Terminators)
    {
        printC(c);
        cout << " ";
    }
    cout << endl;
    for(int x = 0; x < AnalysisTable.size(); ++x)
    {
        cout << setw(4) << nonTerminators[x];
        for(const auto& i: AnalysisTable[x])
        {
            if(i != -1)
                cout << setw(4) << i ;
            else
                cout << setw(4) << " " ;
        }
        cout << endl;
    }
}

void Parser::printCurrentStatus(const string& stk, const string& op, char input) {
	cout << setw(WIDTH) <<std::left<< "stack:";
    for(auto c : stk) {
        printC(c);
    }
    cout << endl;

    cout << setw(WIDTH) <<std::left<< "input:";
    printC(input);
    cout << endl;

    cout << setw(WIDTH) <<std::left<< "operation:";
    cout << op << endl;



    cout << endl;
}