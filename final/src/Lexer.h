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
		Partial,
        Bar,
		Error,
		Comment,
	};
	enum CharType {
		cBlank,
		cNewline,
		cNumber,
		cLetter,
		cSingleQuote,
		cPartial,
        cBar,
		cIllegal,
	};

	bool isBlank(char c);
	bool isNewline(char c);
	bool isNumber(char c);
	bool isLetter(char c);
	bool isSingleQuote(char c);
	bool isPartial(char c);
	bool isBar(char c);
	CharType getCharType(char c);

public:

	void AnalyseTokens(ifstream& sourceCode, ErrorMsgs& errorMsgs, Tokens& tokens);	// Lexical Analysis function
	void run(char * inputfile, Tokens& tokens);		// interface for main function
};

#endif
