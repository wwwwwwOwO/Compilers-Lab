#include "Translator.h"
#define WIDTH 12

extern vector<string> ProductionRules;
extern vector<vector<int>> AnalysisTable;
extern vector<string> productionRules;
extern map<char, Token> Char2Token;
extern map<int, char> Key2Char;
extern map<int, char> Partial2Char;
extern char startChar; 

extern vector<string> productionRulesWithActions;

Translator::Translator()
{

    // grammar.open("translation_grammar.txt", ios::in);
	// if (!grammar.good()) {
	// 	cout << "Fail to open file \"translation_grammar.txt\"."<< endl;
	// 	return;
	// }
    // cout << "\nTranslation Grammar: " << endl;
    // while(getline(grammar,temp))
    // {
    //     cout << temp << endl;
    //     productionRulesWithActions.push_back(temp);
    // }
    // grammar.close();

}



int Translator::indexOf(const vector<char>& v, char x) {
	vector<char>::const_iterator it = find(v.begin(), v.end(), x);
	return it == v.end() ? -1 : it - v.begin();
}

void Translator::printCurrentStatus(const string& s1, const string& s2, const string& s3, const string& s4) {
    cout << endl;
    cout << setw(WIDTH) << setiosflags(ios::left) << s1;
    cout << setw(WIDTH) << setiosflags(ios::left) << s2;
    cout << setw(WIDTH) << setiosflags(ios::left) << s3;
    cout << setw(WIDTH) << setiosflags(ios::left) << s4;
}

void Translator::printQuat(const Quat& q){

}

void Translator::AnalyseSentence(const string& sentence)
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

