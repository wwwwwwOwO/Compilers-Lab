#include "Parser.h"
#define WIDTH 8

// from left: i       +       -       *       /       (       )       $
action Action[16][8] = {
    {{'S', 1},{'O', 0},{'O', 0},{'O', 0},{'O', 0},{'S', 5},{'O', 0},{'O', 0}},
    {{'O', 0},{'R', 8},{'R', 8},{'R', 8},{'R', 8},{'O', 0},{'R', 8},{'R', 8}},
    {{'O', 0},{'S', 6},{'S', 7},{'O', 0},{'O', 0},{'O', 0},{'O', 0},{'A', 0}},
    {{'O', 0},{'R', 3},{'R', 3},{'S', 8},{'S', 9},{'O', 0},{'R', 3},{'R', 3}},
    {{'O', 0},{'R', 6},{'R', 6},{'R', 6},{'R', 6},{'O', 0},{'R', 6},{'R', 6}},
    {{'S', 1},{'O', 0},{'O', 0},{'O', 0},{'O', 0},{'S', 5},{'O', 0},{'O', 0}},
    {{'S', 1},{'O', 0},{'O', 0},{'O', 0},{'O', 0},{'S', 5},{'O', 0},{'O', 0}},
    {{'S', 1},{'O', 0},{'O', 0},{'O', 0},{'O', 0},{'S', 5},{'O', 0},{'O', 0}},
    {{'S', 1},{'O', 0},{'O', 0},{'O', 0},{'O', 0},{'S', 5},{'O', 0},{'O', 0}},
    {{'S', 1},{'O', 0},{'O', 0},{'O', 0},{'O', 0},{'S', 5},{'O', 0},{'O', 0}},
    {{'O', 0},{'R', 5},{'R', 5},{'R', 5},{'R', 5},{'O', 0},{'R', 5},{'R', 5}},
    {{'O', 0},{'R', 4},{'R', 4},{'R', 4},{'R', 4},{'O', 0},{'R', 4},{'R', 4}},
    {{'O', 0},{'R', 7},{'R', 7},{'R', 7},{'R', 7},{'O', 0},{'R', 7},{'R', 7}},
    {{'O', 0},{'R', 1},{'R', 1},{'S', 8},{'S', 9},{'O', 0},{'R', 1},{'R', 1}},
    {{'O', 0},{'R', 2},{'R', 2},{'S', 8},{'S', 9},{'O', 0},{'R', 2},{'R', 2}},
    {{'O', 0},{'S', 6},{'S', 7},{'O', 0},{'O', 0},{'O', 0},{'S',12},{'O', 0}},
}; 

int Goto[16][3] = {
    { 2, 3, 4},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {15, 3, 4},
    {-1,13, 4},
    {-1,14, 4},
    {-1,-1,11},
    {-1,-1,10},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
    {-1,-1,-1},
}; 

map<char, char> Terminator2Index = {
{'i', 0},
{'+', 1},
{'-', 2},
{'*', 3},
{'/', 4},
{'(', 5},
{')', 6},
{'$', 7}
};

map<char, char> nonTerminator2Index = {
{'E', 0},
{'T', 1},
{'F', 2}
};

pair<char, string> productionRules[9]={
{'W', "E"},
{'E', "E+T"},
{'E', "E-T"},
{'E', "T"},
{'T', "T*F"},
{'T', "T/F"},
{'T', "F"},
{'F', "(E)"},
{'F', "i"}
};

// interface for main program
void Parser::run(char* inputfile) {
	ifstream in(inputfile);
	if (!in.good()) {
		cout << "Fail to open file \"" << inputfile << "\""<< endl;
		return;
	}

    printTable();

    string sentence;
	while(getline(in, sentence)) 
    {
        this->AnalyseSentence(sentence);
    }
}

void Parser::AnalyseSentence(const string& sentence)
{
    cout << "Analysing: " + sentence << endl;

    char topState, oldState;
    char aIndex, A;
    string str(sentence + "$");
    string charStack;
    string stateStack;
    charStack.push_back('$');
    stateStack.push_back(0);

    cout << setw(WIDTH * 3) << "stateStack" << setw(WIDTH * 2) << "charStack" << setw(WIDTH * 3) << "Action" << endl;
    cout <<setw(WIDTH) << " ";
    printCurrentStatus(stateStack, charStack, string("init"));

    for(int index = 0; index < str.size();)
    {
        topState = stateStack.back();
        aIndex = Terminator2Index[str[index]];
        cout <<setw(WIDTH) << "input: " << str[index];

        if(Action[topState][aIndex].para == 'S')
        {
            charStack.push_back(str[index]);
            stateStack.push_back(Action[topState][aIndex].val);
            ++index;
            printCurrentStatus(stateStack, charStack, string(string("Shift ") + char('0' + Action[topState][aIndex].val)));
        }
        else if(Action[topState][aIndex].para == 'R')
        {
            for(int i = 0; i < productionRules[Action[topState][aIndex].val].second.length(); ++i)
            {
                charStack.pop_back();
                stateStack.pop_back();
            }
            A = productionRules[Action[topState][aIndex].val].first;
            oldState = topState;
            topState = stateStack.back();

            charStack.push_back(A);
            stateStack.push_back(Goto[topState][nonTerminator2Index[A]]);
            printCurrentStatus(stateStack, charStack,string(string(string("Reduce by ") + productionRules[Action[oldState][aIndex].val].first) + "->" + productionRules[Action[oldState][aIndex].val].second));
        }
        else if(Action[topState][aIndex].para == 'A')
        {
            printCurrentStatus(stateStack, charStack, string("Accept!"));
            return ;
        }
        else
        {
            cerr << "Error!" << endl;
            return;
        }
    }
}

void Parser::printTable()
{
    cout << "Analysis Table: " << endl;
    cout << "action" << endl;
    cout << "               i       +       -       *       /       (       )       $" << endl;
    for(int i = 0; i < 16; ++i) 
    {
        cout << setw(WIDTH) << i;
        for(int j = 0; j < 8; ++j)
        {
            if(Action[i][j].para != 'O')
                cout << setw(WIDTH - 1) << Action[i][j].para << (int)Action[i][j].val;
            else
                cout << setw(WIDTH) << " ";
        }
        cout << endl;
    }
    cout << "goto" << endl;
    cout << "               E       T       F" << endl;
    for(int i = 0; i < 16; ++i) 
    {
        cout << setw(WIDTH) << i;
        for(int j = 0; j < 3; ++j)
        {
            if(Goto[i][j] > 0)
                cout << setw(WIDTH) << Goto[i][j];
            else
                cout << setw(WIDTH) << " ";
        }
        cout << endl;
    }
}

void Parser::printCurrentStatus(const string& s1, const string& s2, const string& s3) {
    stringstream sstr;
    for(int i = 0; i < s1.length(); ++i) {
        sstr << " " << (int) s1[i];
    }
	cout << setw(WIDTH * 2) << sstr.str() << setw(WIDTH * 2) << s2 << setw(WIDTH * 3) << s3 << endl;
}