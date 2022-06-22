#include "Translator.h"
#define WIDTH 15

Translator::Translator()
{
    ifstream grammar("grammar.txt", ios::in);
    string temp;
	if (!grammar.good()) {
		cout << "Fail to open file \"grammar.txt\"."<< endl;
		return;
	}
    cout << "\nGrammar: " << endl;
    while(getline(grammar,temp))
    {
        productionRules.push_back(temp);
        cout << "(" << productionRules.size() << ") ";
        cout << temp << endl;
    }
    grammar.close();

    grammar.open("translation_grammar.txt", ios::in);
	if (!grammar.good()) {
		cout << "Fail to open file \"translation_grammar.txt\"."<< endl;
		return;
	}
    cout << "\nTranslation Grammar: " << endl;
    while(getline(grammar,temp))
    {
        cout << temp << endl;
        productionRulesWithActions.push_back(temp);
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

    // generate analysis table
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

void Translator::getFirstOfNonTerminator(char nonTerminator)
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

void Translator::getFirstOfProductionRules()
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

set<char> Translator::firstBate(const string& remain){
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


int Translator::indexOf(const vector<char>& v, char x) {
	vector<char>::const_iterator it = find(v.begin(), v.end(), x);
	return it == v.end() ? -1 : it - v.begin();
}

void Translator::printCharacters()
{
    cout << "\nTerminators with size of " << Terminators.size() << ": {";
    for(char c : Terminators)
        cout << c << " ";
    cout << "}"<< endl;
    cout << "\nnonTerminators with size of " << nonTerminators.size() << ": {";
    for(char c : nonTerminators)
        cout << c << " ";
    cout << "}"<< endl;
}

void Translator::printFirst()
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

void Translator::printFollow()
{
    cout << "\nFollow set of nonTerminators" << endl;
    for(int i = 0; i < nFollowSets.size(); ++i)
    {
        cout << nonTerminators[i] << ": { ";
        for(char c : nFollowSets[i])
            cout << c << " ";
        cout << "}"<< endl;
    }
    cout << endl;
}

void Translator::printTable()
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

void Translator::printCurrentStatus(const string& s1, const string& s2, const string& s3, const string& s4) {
    cout << endl;
    cout << setw(WIDTH) << setiosflags(ios::left) << s1;
    cout << setw(WIDTH) << setiosflags(ios::left) << s2;
    cout << setw(WIDTH) << setiosflags(ios::left) << s3;
    cout << setw(WIDTH) << setiosflags(ios::left) << s4;
}

void Translator::printQuat(const Quat& q){
    cout << "(" << q.op;
    cout <<  ", " + q.a + ", " + q.b + ", "+ q.res + ")";
}

vector<Quat> Translator::AnalyseSentence(const string& sentence)
{
    vector<Quat> quats;
    cout << "Analysing: " + sentence << endl;

    string str(sentence);   // make a copy
    str.push_back('$');
    char top;

    string synStk, semStk;
    synStk.push_back('$'); 
    synStk.push_back(startChar);


    printCurrentStatus("SYN Stack", "Input", "Operation", "SEM stack");
    cout << setw(WIDTH) << setiosflags(ios::left) << "Quat";
	for (int i = 0;i < str.size();) {
		top = synStk.back();
		
		if (top == '$') {
			if (str[i] == '$') {
				printCurrentStatus("$", "$", string("Accept!"), semStk);
				cout << "\nAnalyze successfully!\n" << endl;
				break;
			}
			else {
				printCurrentStatus(synStk, str.substr(i), string("Error!"), semStk);
				cerr << "Top of stack is '$', but input is not fully processed." << endl;
				break;
			}
		}
        else if(top == 'P')
        {
            printCurrentStatus(synStk, str.substr(i), string("Push ") + synStk[synStk.size() - 2], semStk);
            // get next character, which is ready to push into semantic Stack
            synStk.pop_back();
		    top = synStk.back();
            semStk.push_back(top);
            synStk.pop_back();
            printCurrentStatus(synStk, str.substr(i), " ", semStk);
        }
        else if(top == 'G')
        {
            printCurrentStatus(synStk, str.substr(i), string("Geq ") + synStk[synStk.size() - 2], semStk);
            Quat quat;

            // get op, op is the next character in SYN Stack 
		    synStk.pop_back();
            top = synStk.back();
            synStk.pop_back();
            quat.op = top;

            // get 2nd operand b
		    quat.b.push_back(semStk.back());
            if(quat.b[0] == 't')
                quat.b += '0' + quats.size();
		    semStk.pop_back();

            // get 1st operand a
		    quat.a.push_back(semStk.back());
            if(quat.a[0] == 't')
                quat.a += '0' + quats.size();
		    semStk.pop_back();

            // get res
            quat.res.push_back('t');
            quat.res.push_back('1' + quats.size());

            semStk.push_back('t');
            quats.push_back(quat);
            printCurrentStatus(synStk, str.substr(i), " ", semStk);
            printQuat(quat);
        }
		else if (indexOf(Terminators, top) != -1 || top >= 'a' && top <= 'z') {
			if (top == str[i]) {
				printCurrentStatus(synStk, str.substr(i), string("Match ") + top, semStk);
                synStk.pop_back();
				++i; // push 
			}
			else {
				printCurrentStatus(synStk, str.substr(i), string("Error!"), semStk);
				cerr << "Top of stack is a terminator '" << top << "', but input character is '" << str[i] << "'" << endl;
				break;
			}
		}
		else if (indexOf(nonTerminators, top) != -1) {
            char c = str[i];
            if(c >= 'a' && c <= 'z')
                c = 'i';

			int indexProductionRule = analysisTable[indexOf(nonTerminators, top)][indexOf(Terminators, c)];

			if (indexProductionRule != -1) {
				printCurrentStatus(synStk, str.substr(i), productionRulesWithActions[indexProductionRule], semStk);
                synStk.pop_back();
				if (productionRulesWithActions[indexProductionRule][3] != '@') { // if right side is @, skip it 
					for (int j = productionRulesWithActions[indexProductionRule].size() - 1; j >= 3; --j) {
                        if(productionRulesWithActions[indexProductionRule][j] == 'i')
                        {
                            synStk.push_back(str[i]);   // push character(a - z) instead of i
                        }
                        else
                        {
                            synStk.push_back(productionRulesWithActions[indexProductionRule][j]);
                        }
					}
				}
			}
			else {
				printCurrentStatus(synStk, str.substr(i), string("Error!"), semStk);
				cerr << "analysisTable[" << top << "][" << str[i] << "] is empty." << endl;
				break;
			}
		}
		else {
			cerr << "Analyze failed.\n" << endl;
			break;
		}
	}
    
    return quats;
}

// interface for main program
void Translator::run(char* inputfile) {
	ifstream in(inputfile);
	if (!in.good()) {
		cout << "Fail to open file \"" << inputfile << "\""<< endl;
		return;
	}

    string sentence;
    vector<Quat> quats;
	while(getline(in, sentence)) 
    {
        quats = this->AnalyseSentence(sentence);
        cout << "Quats:" << endl;
        for(int i = 0; i < quats.size(); ++i)
        {
            cout << '(' << i + 1 << ')' << ' ';
            printQuat(quats[i]);
            cout << endl;
        }
        cout << endl;
    }
}

