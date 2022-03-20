#include "Lexer.h"

// interface for main program
void Lexer::run(char* inputfile, char* outputfile) {
	ifstream in(inputfile);
	if (!in.good()) {
		cout << "Fail to open file \"" << inputfile << "\""<< endl;
		return;
	}

	ofstream out(outputfile);
	if (!out.good()) {
		cout << "Fail to open file \"" << outputfile << "\""<< endl;
		return;
	}

	// define error messages and token list
	ErrorMsgs error_msgs;
	Tokens tokens;

	this->AnalyseTokens(in, error_msgs, tokens);

	for (auto token : tokens)
		out << "<" << setw(2) << setfill('0') << token.GetKind() << "> " << token.GetValue() << endl;

	for (auto msg : error_msgs)
		out << msg.toString() << endl;
	
	cout << "SUCCESS!" << endl;
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

bool Lexer::isDoubleQuote(char c)
{
    return c == '\"';
}

bool Lexer::isOperator(char c)
{
    return c == '(' || c == ')' || c == '[' || c == ']' || 
    c == '-' || c == '+' || c == '*' || c == '~' ||
    c == '%' || c == '<' || c == '>' || c == '=' || 
    c == '|' || c == '&' || c == '^' || c == '!' ||  c == '^';
}

bool Lexer::isNewline(char c)
{
    return c == 0x0A;
}

bool Lexer::isBar(char c)
{
	return c == '/';
}

bool Lexer::isDelimiter(char c){
	return c == ';' || c == '{' || c == '}' || c == ',' || c == ',' || c == ':';
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
	else if (isDoubleQuote(c))
		return CharType::cDoubleQuote;
	else if (isLetter(c))
		return CharType::cLetter;
	else if (isOperator(c))
		return CharType::cOperator;
    else if (isBar(c))
        return CharType::cBar;
    else if (isDelimiter(c))
        return CharType::cDelimiter;
    else
	    return CharType::cIllegal;
}

void Lexer::AnalyseTokens(ifstream& sourceCode, ErrorMsgs& errorMsgs, Tokens& tokens){
	size_t currLine = 0, currColumn = 0;	// initialize current Line and Column

	Status status = Status::Blank;	// initial state
	Tokens currLineTokens;	// stores tokens of current line
	CharType cType;	// char type
	char c;			// current character
	string word;	// current word

	while (sourceCode.get(c)) {
		currColumn++;	// update Column
		cType = getCharType(c);

		// for current state, turn to next state by c
		switch (status) {
			case Status::Blank:
				switch (cType) {
                    case CharType::cNewline:
                        currLine++; currColumn = 0;		// update Line, Column
                        tokens.append(currLineTokens);	// append current line tokens to global tokens
                        currLineTokens.clear();			// new line tokens
                        break;
                    case CharType::cBlank:	// skip blank
                        break;
                    case CharType::cNumber:
						word.clear();
						word += c;
                        status = Status::Number;
                        break;
                    case CharType::cLetter:
						word.clear();
						word += c;
                        status = Status::Letter;
                        break;
                    case CharType::cSingleQuote:
                        status = Status::SingleQuote;
                        break;
                    case CharType::cDoubleQuote:
                        status = Status::DoubleQuote;
                        break;  
                    case CharType::cOperator:
						word.clear();
						word += c;
                        status = Status::Operator;
                        break;
                    case CharType::cBar:
						word.clear();
						word += c;
                        status = Status::Bar;
                        break;
					case CharType::cDelimiter:
						word.clear();
						word += c;
						status = Status::Delimiter;
						break;
                    case CharType::cIllegal:
                        status = Status::Error;
                        errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "illegal character: " + c));
                        break;
				}
				break;
			case Status::Number: // ends when meets other character
				switch (cType) {
					case CharType::cNewline:
						currLine++; currColumn = 0;
						currLineTokens.push({ TokenKind::TK_INTEGER, word, currLine, currColumn - word.length()});
						tokens.append(currLineTokens); currLineTokens.clear();
						word.clear();
						status = Status::Blank;
						break;
					case CharType::cBlank:
						currLineTokens.push({ TokenKind::TK_INTEGER, word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::Blank;
						break;
					case CharType::cNumber:
						word += c;
						break;
					case CharType::cLetter: // letter after number is illegal
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn - word.length() + 1, "Illegal Combination(number,char):"));
						word.clear();
						status = Status::Error;
						break;
					case CharType::cSingleQuote:
						currLineTokens.push({ TokenKind::TK_INTEGER, word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::SingleQuote;
						break;
					case CharType::cDoubleQuote:
						currLineTokens.push({ TokenKind::TK_INTEGER, word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::DoubleQuote;
						break;
					case CharType::cOperator:
						currLineTokens.push({ TokenKind::TK_INTEGER, word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
						status = Status::Operator;
						break;
					case CharType::cBar:
						currLineTokens.push({ TokenKind::TK_INTEGER, word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
						status = Status::Bar;
						break;
					case CharType::cDelimiter:
						currLineTokens.push({ TokenKind::TK_INTEGER, word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
						status = Status::Delimiter;
						break;
					case CharType::cIllegal:
						currLineTokens.push({ TokenKind::TK_INTEGER, word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::Error;
						break;
				}
				break;
			case Status::Letter:
				switch (cType) {
					case CharType::cNewline:	// string end
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)	// TokenCode stores all Keys
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else	// identifiers
							currLineTokens.push({TokenKind::TK_ID, word, currLine, currColumn - word.length()});
						currLine++; currColumn = 0;
						tokens.append(currLineTokens); currLineTokens.clear();
						word.clear();
						status = Status::Blank;
						break;
					case CharType::cBlank:	// stirng end
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							currLineTokens.push({TokenKind::TK_ID, word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::Blank;
						break;
					case CharType::cNumber:	// keep state
					case CharType::cLetter:
						word += c;
						break;
					case CharType::cSingleQuote:	// string end
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							currLineTokens.push({TokenKind::TK_ID, word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::SingleQuote;
						break;
					case CharType::cDoubleQuote:	// string end
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							currLineTokens.push({TokenKind::TK_ID, word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::DoubleQuote;
						break;
					case CharType::cBar:	// string end
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							currLineTokens.push({TokenKind::TK_ID, word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
						status = Status::Bar;
						break;
					case CharType::cDelimiter:
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							currLineTokens.push({TokenKind::TK_ID, word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
						status = Status::Delimiter;
						break;
					case CharType::cOperator:	//	string end
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							currLineTokens.push({TokenKind::TK_ID, word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
						status = Status::Operator;
						break;
					case CharType::cIllegal:	// string end and record error
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							currLineTokens.push({ TokenKind::TK_ID, word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::Error;
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
					status = Status::Blank;
					break;
				// keep state unchanged
				case CharType::cBlank:
				case CharType::cNumber:
				case CharType::cLetter:
				case CharType::cOperator:
				case CharType::cBar:
				case CharType::cIllegal:
				case CharType::cDoubleQuote:
				case CharType::cDelimiter:
					word += c;
					break;
				case CharType::cSingleQuote:
					if(word.length() < 2 || word.length() == 2 && word[0] == '\\')	// legal format
					{
						currLineTokens.push({TokenKind::TK_CHARACTER, word, currLine, currColumn - word.length()});
					}
					else
					{
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn - word.length() + 1, "Char constant contains too much charaters"));
					}
					word.clear();
					status = Status::Blank;
					break;
				}
				break;
			case Status::DoubleQuote:	// ends when meets double quote or newline
				switch (cType) {
				case CharType::cNewline:
					errorMsgs.emplace_back(ErrorMsg(currLine, currColumn - word.length() + 1, "Identifier \" is not matched"));
					currLine++; currColumn = 0;
					tokens.append(currLineTokens); currLineTokens.clear();
					status = Status::Blank;
					break;
				// keep state unchanged
				case CharType::cBlank:
				case CharType::cNumber:
				case CharType::cLetter:
				case CharType::cOperator:
				case CharType::cBar:
				case CharType::cDelimiter:
				case CharType::cIllegal:
				case CharType::cSingleQuote:
					word += c;
					break;
				case CharType::cDoubleQuote:
					currLineTokens.push({TokenKind::TK_STRING, word, currLine, currColumn - word.length()});
					word.clear();
					status = Status::Blank;
					break;
				}
				break;
			case Status::Operator:
				switch (cType) {
					case CharType::cNewline:
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)		// TokenCode stores all Operators
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
						word.clear();
						currLine++; currColumn = 0;
						tokens.append(currLineTokens); currLineTokens.clear();
						status = Status::Blank;
						break;
					case CharType::cBlank:
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)		// TokenCode stores all Operators
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
						word.clear();
						status = Status::Blank;
						break;
					case CharType::cNumber:
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)		// TokenCode stores all Operators
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
						word.clear();
						word += c;
						status = Status::Number;
						break;
					case CharType::cLetter:
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)		// TokenCode stores all Operators
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
						word.clear();
						word += c;
						status = Status::Letter;
						break;
					case CharType::cSingleQuote:
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)		// TokenCode stores all Operators
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
						word.clear();
						status = Status::SingleQuote;
						break;
					case CharType::cDoubleQuote:
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)		// TokenCode stores all Operators
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
						word.clear();
						status = Status::DoubleQuote;
						break;
					case CharType::cBar:
						word += c;
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
						word.clear();
						status = Status::Bar;
						break;
					case CharType::cDelimiter:
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
						word.clear();
						word += c;
						status = Status::Delimiter;
						break;
					case CharType::cOperator:
						if(word[word.length()-1] == ')' || word[word.length()-1] == ']')
						{
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
							word.clear();
							word += c;
							status = Status::Operator;
						}
						else if (Token::getTokenCode(word + c) != TokenKind::TK_NULL){
							currLineTokens.push({Token::getTokenCode(word + c), word + c, currLine, currColumn - word.length()});
							status = Status::Blank;
						}
						else
						{
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word + c));
							word.clear();
							status = Status::Error;
						}
						break;
					case CharType::cIllegal:
						if (Token::getTokenCode(word) != TokenKind::TK_NULL)
							currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						else
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
						word.clear();
						status = Status::Error;
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal character after string: " + word));
						break;
				}
				break;
			case Status::Error: //end when meet space or newline
				switch (cType) {
				case CharType::cNewline:
					currLine++;
					currColumn = 0;
					status = Status::Blank;
					break;
				case CharType::cBlank:
					status = Status::Blank;
					break;

				case CharType::cNumber:
				case CharType::cLetter:
				case CharType::cSingleQuote:
				case CharType::cDoubleQuote:
				case CharType::cOperator:
				case CharType::cDelimiter:
				case CharType::cBar:
				case CharType::cIllegal:
					break;
				}
				break;
			case Status::Bar:
				switch (cType) {
					case CharType::cNewline:
						currLineTokens.push({Token::getTokenCode("/"), word, currLine, currColumn - word.length()});
						currLine++; currColumn = 0;
						tokens.append(currLineTokens); currLineTokens.clear();
						word.clear();
						status = Status::Blank;
						break;
					case CharType::cBlank:
						currLineTokens.push({Token::getTokenCode("/"), word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::Blank;
						break;
					case CharType::cNumber:
						currLineTokens.push({Token::getTokenCode("/"), word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::Number;
						break;
					case CharType::cLetter:
						currLineTokens.push({Token::getTokenCode("/"), word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::Letter;
						break;
					case CharType::cSingleQuote:
						currLineTokens.push({Token::getTokenCode("/"), word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::SingleQuote;
						break;
					case CharType::cDoubleQuote:
						currLineTokens.push({Token::getTokenCode("/"), word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::DoubleQuote;
						break;  
					case CharType::cOperator:
						word += c;
						if(c == '='){
							currLineTokens.push({Token::getTokenCode("/="), word, currLine, currColumn - word.length()});
							status = Status::Blank;
						}
						else
						{
							errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
							status = Status::Error;
						}
						break;
					case CharType::cBar: // '/' meets '/' turn to state comment
						word.clear();
						status = Status::Comment;
						break;
					case CharType::cDelimiter:
						currLineTokens.push({Token::getTokenCode("/"), word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
						status = Status::Delimiter;
						break;
					case CharType::cIllegal:
						currLineTokens.push({Token::getTokenCode("/"), word, currLine, currColumn - word.length()});
						errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal character after string: " + c));
						status = Status::Error;
						break;
				}
				break;
			case Status::Delimiter:
				switch (cType) {
					case CharType::cNewline:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						currLine++; currColumn = 0;
						tokens.append(currLineTokens); currLineTokens.clear();
						word.clear();
						status = Status::Blank;
						break;
                    case CharType::cBlank:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						word.clear();
						status = Status::Blank;
                        break;
                    case CharType::cNumber:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
                        status = Status::Number;
                        break;
                    case CharType::cLetter:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
                        status = Status::Letter;
                        break;
                    case CharType::cSingleQuote:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						word.clear();
                        status = Status::SingleQuote;
                        break;
                    case CharType::cDoubleQuote:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						word.clear();
                        status = Status::DoubleQuote;
                        break;  
                    case CharType::cOperator:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
                        status = Status::Operator;
                        break;
                    case CharType::cBar:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
                        status = Status::Bar;
                        break;
					case CharType::cDelimiter:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						word.clear();
						word += c;
						status = Status::Delimiter;
						break;
                    case CharType::cIllegal:
						currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
						word.clear();
                        status = Status::Error;
                        errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "illegal character: " + c));
                        break;
				}
				break;
			case Status::Comment: // only ends when meets newline
				if (isNewline(c)) {
					currLine++; currColumn = 0;
					tokens.append(currLineTokens); currLineTokens.clear();
					word.clear();
					status = Status::Blank;
				}
				break;
		}
	}

	// last state and word
	switch (status) {
		case Status::Letter:
			if (Token::getTokenCode(word) != TokenKind::TK_NULL)
				currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
			else
				currLineTokens.push({TokenKind::TK_ID, word, currLine, currColumn - word.length()});
			break;
		case Status::Number:
			currLineTokens.push({TokenKind::TK_INTEGER, word, currLine, currColumn - word.length()});
			break;
		case Status::SingleQuote:
			errorMsgs.emplace_back(ErrorMsg(currLine, currColumn - word.length() + 1, "Identifier \' is not matched"));
			break;
		case Status::DoubleQuote:
			errorMsgs.emplace_back(ErrorMsg(currLine, currColumn - word.length() + 1, "Identifier \" is not matched"));
			break;
		case Status::Operator:
			if (Token::getTokenCode(word) != TokenKind::TK_NULL)		// TokenCode stores all Operators
				currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
			else
				errorMsgs.emplace_back(ErrorMsg(currLine, currColumn, "Illegal Operator: " + word));
			break;
		case Status::Delimiter:
		case Status::Bar:
			currLineTokens.push({Token::getTokenCode(word), word, currLine, currColumn - word.length()});
			break;
		case Status::Blank:
		case Status::Error:
		case Status::Comment:
			break;
	}
	tokens.append(currLineTokens);
}
		
