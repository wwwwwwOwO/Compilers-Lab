#include "Lexer.h"

map<string, int> Key2Index = {
	{"program", 0},  {"var", 1},  {"integer", 2}, {"char", 3},  
	{"real", 4},  {"begin", 5},  {"end", 6}
};

map<string, int> Partial2Index = {
	{",", 0},  {":", 1},  {";", 2}, {":=", 3},  {"*", 4},  {"/", 5},  
	{"+", 6},  {"-", 7},  {".", 8},  {"(", 9},  {")", 10}
};

extern vector<KeyType> KeyTable;
extern vector<PartialType> PartialTable;
extern symbolTable SymbolTable;
extern constantTable ConstantTable;

// interface for main program
void Lexer::run(char* inputfile, Tokens& tokens) {
	ifstream in(inputfile);
	if (!in.good()) {
		cout << "Fail to open file \"" << inputfile << "\""<< endl;
		return;
	}

	// define error messages and token list
	ErrorMsgs error_msgs;
	this->AnalyseTokens(in, error_msgs, tokens);
	
	cout << GRN"[INFO] "<< RESET;
	cout << "Lexical analysis completed successfully." << endl;

	if (!error_msgs.empty())
	{
		for (auto& msg : error_msgs)
		{
			cout << RED"[ERROR]"<< RESET;
			msg.print();
		}

	}
}

bool Lexer::isBlank(char c)
{
    return c == 0x20 || c == 0x09 || c == 0x0B || c == 0x0C || c == 0x0D;
}

bool Lexer::isNumber(char c){
    return c >= '0' && c <= '9';
}

bool Lexer::isLetter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::isSingleQuote(char c){
    return c == '\'';
}

bool Lexer::isPartial(char c)
{
    return c == '(' || c == ')' || c == '-' || c == '+' || c == '*' ||
    c == '=' || c == ';' || c == ':' || c == '.' ||  c == ',';
}

bool Lexer::isNewline(char c)
{
    return c == 0x0A;
}

bool Lexer::isBar(char c)
{
	return c == '/';
}

// get type of current character, define as illegal if it is not recognizable
Lexer::CharType Lexer::getCharType(char c) {
	if (isNewline(c))
		return CharType::cNewline;
	else if (isNumber(c))
		return CharType::cNumber;
	else if (isBlank(c))
		return CharType::cBlank;
	else if (isSingleQuote(c))
		return CharType::cSingleQuote;
	else if (isLetter(c))
		return CharType::cLetter;
	else if (isPartial(c))
		return CharType::cPartial;
    else if (isBar(c))
        return CharType::cBar;
    else
	    return CharType::cIllegal;
}

void Lexer::AnalyseTokens(ifstream& sourceCode, ErrorMsgs& errorMsgs, Tokens& tokens){
	size_t currLine = 0, currColumn = 0;	// initialize current Line and Column

	Status currentState = Status::Blank;	// initial state
	Tokens currLineTokens;	// stores tokens of current line
	CharType cType;	// char type
	char c;			// current character
	string word;	// current word

	while (sourceCode.get(c)) {
		currColumn++;	// update Column
		cType = getCharType(c);

		// for current state, turn to next state by c
		switch (currentState) {
			case Status::Blank:
				word.clear();
				switch (cType) {
                    case CharType::cNewline:
                        currLine++; currColumn = 0;		// update Line, Column
                        tokens.append(currLineTokens);	// append current line tokens to global tokens
                        currLineTokens.clear();			// new line tokens
                        break;
                    case CharType::cBlank:	// skip blank
                        break;
                    case CharType::cNumber:
						word += c;
                        currentState = Status::Number;
                        break;
                    case CharType::cLetter:
						word += c;
                        currentState = Status::Letter;
                        break;
                    case CharType::cSingleQuote:
                        currentState = Status::SingleQuote;
                        break;
                    case CharType::cPartial:
						word += c;
                        currentState = Status::Partial;
                        break;
                    case CharType::cBar:
						word += c;
                        currentState = Status::Bar;
                        break;
                    case CharType::cIllegal:
                        currentState = Status::Error;
                        errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Character: " + c));
                        break;
				}
				break;
			case Status::Number: // ends when meets other character
				switch (cType) {
					case CharType::cNewline:
						if(word.find_first_of(".") == -1)	// integer: no "." appears in word
						{
							ConstantTable.addEntry(stoi(word), DataType::Integer);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stoi(word), DataType::Integer), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else if(word.find_first_of(".") == word.find_last_of("."))	// double: has one "."
						{
							ConstantTable.addEntry(stod(word), DataType::Real);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stod(word), DataType::Real), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else{	// illegal number
							currentState = Status::Error;
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Number: " + word));
						}
						tokens.append(currLineTokens); currLineTokens.clear();
						currLine++; currColumn = 0;
						word.clear();
						currentState = Status::Blank;
						break;
					case CharType::cBlank:
						if(word.find_first_of(".") == -1)	// integer: no "." appears in word
						{
							ConstantTable.addEntry(stoi(word), DataType::Integer);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stoi(word), DataType::Integer), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else if(word.find_first_of(".") == word.find_last_of("."))	// double: has one "."
						{
							ConstantTable.addEntry(stod(word), DataType::Real);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stod(word), DataType::Real), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else{	// illegal number
							currentState = Status::Error;
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Number: " + word));
						}
						word.clear();
						currentState = Status::Blank;
						break;
					case CharType::cNumber:
						word += c;
						break;
					case CharType::cLetter: // letter after number is illegal
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn - word.length() + 1, "Illegal Combination(number, char):"));
						word.clear();
						currentState = Status::Error;
						break;
					case CharType::cSingleQuote:
						if(word.find_first_of(".") == -1)	// integer: no "." appears in word
						{
							ConstantTable.addEntry(stoi(word), DataType::Integer);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stoi(word), DataType::Integer), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else if(word.find_first_of(".") == word.find_last_of("."))	// double: has one "."
						{
							ConstantTable.addEntry(stod(word), DataType::Real);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stod(word), DataType::Real), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else{	// illegal number
							currentState = Status::Error;
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Number: " + word));
						}
						word.clear();
						currentState = Status::SingleQuote;
						break;
					case CharType::cPartial:
						if (c == '.')
						{
							word += c;
							break;
						}
						if(word.find_first_of(".") == -1)	// integer: no "." appears in word
						{
							ConstantTable.addEntry(stoi(word), DataType::Integer);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stoi(word), DataType::Integer), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else if(word.find_first_of(".") == word.find_last_of("."))	// double: has one "."
						{
							ConstantTable.addEntry(stod(word), DataType::Real);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stod(word), DataType::Real), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else{	// illegal number
							currentState = Status::Error;
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Number: " + word));
						}
						word.clear();
						word += c;
						currentState = Status::Partial;
						break;
					case CharType::cBar:
						if(word.find_first_of(".") == -1)	// integer: no "." appears in word
						{
							ConstantTable.addEntry(stoi(word), DataType::Integer);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stoi(word), DataType::Integer), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else if(word.find_first_of(".") == word.find_last_of("."))	// double: has one "."
						{
							ConstantTable.addEntry(stod(word), DataType::Real);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stod(word), DataType::Real), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else{	// illegal number
							currentState = Status::Error;
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Number: " + word));
						}
						word.clear();
						currentState = Status::Bar;
						break;
					case CharType::cIllegal:
						if(word.find_first_of(".") == -1)	// integer: no "." appears in word
						{
							ConstantTable.addEntry(stoi(word), DataType::Integer);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stoi(word), DataType::Integer), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else if(word.find_first_of(".") == word.find_last_of("."))	// double: has one "."
						{
							ConstantTable.addEntry(stod(word), DataType::Real);
							currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stod(word), DataType::Real), currLine, currColumn - word.length()});
							currentState = Status::Blank;
						}
						else{	// illegal number
							currentState = Status::Error;
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Number: " + word));
						}
						word.clear();
						currentState = Status::Error;
						break;
				}
				break;
			case Status::Letter:
				switch (cType) {
					case CharType::cNewline:	// string end
						if(Key2Index.find(word) != Key2Index.end())	// key
						{
							currLineTokens.push({ TokenKind::KEY, Key2Index[word], currLine, currColumn - word.length()});
						}
						else // identifiers
						{
							SymbolTable.addEntry(word);
							currLineTokens.push({ TokenKind::ID, SymbolTable.getIndex(word), currLine, currColumn - word.length()});
						}
						currLine++; currColumn = 0;
						tokens.append(currLineTokens); currLineTokens.clear();
						word.clear();
						currentState = Status::Blank;
						break;
					case CharType::cBlank:	// stirng end
						if(Key2Index.find(word) != Key2Index.end())	// key
						{
							currLineTokens.push({ TokenKind::KEY, Key2Index[word], currLine, currColumn - word.length()});
						}
						else // identifiers
						{
							SymbolTable.addEntry(word);
							currLineTokens.push({ TokenKind::ID, SymbolTable.getIndex(word), currLine, currColumn - word.length()});
						}
						word.clear();
						currentState = Status::Blank;
						break;
					case CharType::cNumber:	// keep state
					case CharType::cLetter:
						word += c;
						break;
					case CharType::cSingleQuote:
						if(Key2Index.find(word) != Key2Index.end())	// key
						{
							currLineTokens.push({ TokenKind::KEY, Key2Index[word], currLine, currColumn - word.length()});
						}
						else // identifiers
						{
							SymbolTable.addEntry(word);
							currLineTokens.push({ TokenKind::ID, SymbolTable.getIndex(word), currLine, currColumn - word.length()});
						}
						word.clear();
						currentState = Status::SingleQuote;
						break;
					case CharType::cBar:	// string end
						if(Key2Index.find(word) != Key2Index.end())	// key
						{
							currLineTokens.push({ TokenKind::KEY, Key2Index[word], currLine, currColumn - word.length()});
						}
						else // identifiers
						{
							SymbolTable.addEntry(word);
							currLineTokens.push({ TokenKind::ID, SymbolTable.getIndex(word), currLine, currColumn - word.length()});
						}
						word.clear();
						word += c;
						currentState = Status::Bar;
						break;

					case CharType::cPartial:	//	string end
						if(Key2Index.find(word) != Key2Index.end())	// key
						{
							currLineTokens.push({ TokenKind::KEY, Key2Index[word], currLine, currColumn - word.length()});
						}
						else // identifiers
						{
							SymbolTable.addEntry(word);
							currLineTokens.push({ TokenKind::ID, SymbolTable.getIndex(word), currLine, currColumn - word.length()});
						}
						word.clear();
						word += c;
						currentState = Status::Partial;
						break;
					case CharType::cIllegal:	// string end and record error
						if(Key2Index.find(word) != Key2Index.end())	// key
						{
							currLineTokens.push({ TokenKind::KEY, Key2Index[word], currLine, currColumn - word.length()});
						}
						else // identifiers
						{
							SymbolTable.addEntry(word);
							currLineTokens.push({ TokenKind::ID, SymbolTable.getIndex(word), currLine, currColumn - word.length()});
						}
						word.clear();
						currentState = Status::Error;
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal character after string: " + word));
						break;
				}
				break;
			case Status::SingleQuote:	// ends when meets single quote or newline
				switch (cType) {
				case CharType::cNewline:
					errorMsgs.emplace_back(ErrorMsg(currLine, currColumn - word.length() + 1, "Identifier \' is not matched"));
					currLine++; currColumn = 0;
					tokens.append(currLineTokens); currLineTokens.clear();
					currentState = Status::Blank;
					break;
				// keep state unchanged
				case CharType::cBlank:
				case CharType::cNumber:
				case CharType::cLetter:
				case CharType::cPartial:
				case CharType::cBar:
				case CharType::cIllegal:
					word += c;
					break;
				case CharType::cSingleQuote:
					if(word.length() == 1)	// legal format
					{
						ConstantTable.addEntry(word[0], DataType::Char);
						currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(word[0], DataType::Char), currLine, currColumn - word.length()});
						currentState = Status::Blank;
					}
					else
					{
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn - word.length() + 1, "Char constant contains too much charaters"));
					}
					word.clear();
					currentState = Status::Blank;
					break;
				}
				break;
			case Status::Partial:
				switch (cType) {
					case CharType::cNewline:
						if(Partial2Index.find(word) != Partial2Index.end())	// legal partials
							currLineTokens.push({ TokenKind::PARTIAL, Partial2Index[word], currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Partial: " + word));
						word.clear();
						currLine++; currColumn = 0;
						tokens.append(currLineTokens); currLineTokens.clear();
						currentState = Status::Blank;
						break;
					case CharType::cBlank:
						if(Partial2Index.find(word) != Partial2Index.end())	// legal partials
							currLineTokens.push({ TokenKind::PARTIAL, Partial2Index[word], currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Partial: " + word));
						word.clear();
						currentState = Status::Blank;
						break;
					case CharType::cNumber:
						if(Partial2Index.find(word) != Partial2Index.end())	// legal partials
							currLineTokens.push({ TokenKind::PARTIAL, Partial2Index[word], currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Partial: " + word));
						word.clear();
						word += c;
						currentState = Status::Number;
						break;
					case CharType::cLetter:
						if(Partial2Index.find(word) != Partial2Index.end())	// legal partials
							currLineTokens.push({ TokenKind::PARTIAL, Partial2Index[word], currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Partial: " + word));
						word.clear();
						word += c;
						currentState = Status::Letter;
						break;
					case CharType::cSingleQuote:
						if(Partial2Index.find(word) != Partial2Index.end())	// legal partials
							currLineTokens.push({ TokenKind::PARTIAL, Partial2Index[word], currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Partial: " + word));
						word.clear();
						currentState = Status::SingleQuote;
						break;
					case CharType::cBar:
						if(Partial2Index.find(word) != Partial2Index.end())	// legal partials
							currLineTokens.push({ TokenKind::PARTIAL, Partial2Index[word], currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Partial: " + word));
						word.clear();
						currentState = Status::Bar;
						break;
					case CharType::cPartial:
						word += c;
						if(Partial2Index.find(word) != Partial2Index.end())	// legal partials
							currLineTokens.push({ TokenKind::PARTIAL, Partial2Index[word], currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Partial: " + word));
						word.clear();
						currentState = Status::Blank;
						break;
					case CharType::cIllegal:
						if(Partial2Index.find(word) != Partial2Index.end())	// legal partials
							currLineTokens.push({ TokenKind::PARTIAL, Partial2Index[word], currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Partial: " + word));
						word.clear();
						currentState = Status::Error;
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal character after string: " + word));
						break;
				}
				break;
			case Status::Error: //end when meet space or newline
				switch (cType) {
				case CharType::cNewline:
					currLine++;
					currColumn = 0;
					currentState = Status::Blank;
					break;
				case CharType::cBlank:
					currentState = Status::Blank;
					break;

				case CharType::cNumber:
				case CharType::cLetter:
				case CharType::cSingleQuote:
				case CharType::cPartial:
				case CharType::cBar:
				case CharType::cIllegal:
					break;
				}
				break;
			case Status::Bar:
				switch (cType) {
					case CharType::cNewline:
						currLineTokens.push({ TokenKind::PARTIAL, Partial2Index["/"], currLine, currColumn - word.length()});
						currLine++; currColumn = 0;
						tokens.append(currLineTokens); currLineTokens.clear();
						word.clear();
						currentState = Status::Blank;
						break;
					case CharType::cBlank:
						currLineTokens.push({ TokenKind::PARTIAL, Partial2Index["/"], currLine, currColumn - word.length()});
						word.clear();
						currentState = Status::Blank;
						break;
					case CharType::cNumber:
						currLineTokens.push({ TokenKind::PARTIAL, Partial2Index["/"], currLine, currColumn - word.length()});
						word.clear();
						currentState = Status::Number;
						break;
					case CharType::cLetter:
						currLineTokens.push({ TokenKind::PARTIAL, Partial2Index["/"], currLine, currColumn - word.length()});
						word.clear();
						currentState = Status::Letter;
						break;
					case CharType::cSingleQuote:
						currLineTokens.push({ TokenKind::PARTIAL, Partial2Index["/"], currLine, currColumn - word.length()});
						word.clear();
						currentState = Status::SingleQuote;
						break;
					case CharType::cPartial:
						word += c;
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Can not recognize " + word));
						currentState = Status::Error;
						break;
					case CharType::cBar: // '/' meets '/' turn to state comment
						word.clear();
						currentState = Status::Comment;
						break;
					case CharType::cIllegal:
						currLineTokens.push({ TokenKind::PARTIAL, Partial2Index["/"], currLine, currColumn - word.length()});
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal character after string: " + c));
						currentState = Status::Error;
						break;
				}
				break;
			case Status::Comment: // only ends when meets newline
				if (isNewline(c)) {
					currLine++; currColumn = 0;
					tokens.append(currLineTokens); currLineTokens.clear();
					word.clear();
					currentState = Status::Blank;
				}
				break;
		}
	}

	// last state and word
	switch (currentState) {
		case Status::Letter:
			if(Key2Index.find(word) != Key2Index.end())	// key
			{
				currLineTokens.push({ TokenKind::KEY, Key2Index[word], currLine, currColumn - word.length()});
			}
			else // identifiers
			{
				SymbolTable.addEntry(word);
				currLineTokens.push({ TokenKind::ID, SymbolTable.getIndex(word), currLine, currColumn - word.length()});
			}
			break;
		case Status::Number:

			if(word.find_first_of(".") == -1)	// integer: no "." appears in word
			{
				ConstantTable.addEntry(stoi(word), DataType::Integer);
				currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stoi(word), DataType::Integer), currLine, currColumn - word.length()});
				currentState = Status::Blank;
			}
			else if(word.find_first_of(".") == word.find_last_of("."))	// double: has one "."
			{
				ConstantTable.addEntry(stod(word), DataType::Real);
				currLineTokens.push({ TokenKind::CONSTANT, ConstantTable.getIndex(stod(word), DataType::Real), currLine, currColumn - word.length()});
				currentState = Status::Blank;
			}
			else{	// illegal number
				currentState = Status::Error;
				errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Number: " + word));
			}
			break;
		case Status::SingleQuote:
			errorMsgs.emplace_back(ErrorMsg(currLine, currColumn - word.length() + 1, "Identifier \' is not matched"));
			break;
		case Status::Partial:
			if(Partial2Index.find(word) != Partial2Index.end())	// legal partials
				currLineTokens.push({ TokenKind::PARTIAL, Partial2Index[word], currLine, currColumn - word.length()});
			else
				errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Partial: " + word));
			break;
		case Status::Bar:
			currLineTokens.push({ TokenKind::PARTIAL, Partial2Index["/"], currLine, currColumn - word.length()});
			break;
		case Status::Blank:
		case Status::Error:
		case Status::Comment:
			break;
	}
	tokens.append(currLineTokens);
}
		
