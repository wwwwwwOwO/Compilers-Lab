#ifndef LEXER_H
#define LEXER_H

#include "DataType.h"


class Lexer {
private:
	enum Status {
		Blank,
		Number,
		Letter,
		SingleQuote,
        DoubleQuote,
		Operator,
		Delimiter,
        Bar,
		Error,
		Comment,
	};
	enum CharType {
		cBlank,
		cNumber,
		cLetter,
		cSingleQuote,
        cDoubleQuote,
		cOperator,
		cNewline,
        cBar,
		cDelimiter,
		cIllegal,
	};

	bool isBlank(char c);
	bool isNumber(char c);
	bool isLetter(char c);
	bool isSingleQuote(char c);
	bool isDoubleQuote(char c);
	bool isOperator(char c);
	bool isNewline(char c);
	bool isBar(char c);
	bool isDelimiter(char c);
	CharType getCharType(char c);

public:
	void AnalyseTokens(ifstream& sourceCode, ErrorMsgs& errorMsgs, Tokens& tokens);	// Lexical Analysis function
	void run(char * inputfile, char * outputfile);		// interface for main function
};

#endif
