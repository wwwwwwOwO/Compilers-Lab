#ifndef DATATYPE_H
#define DATATYPE_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <iomanip>

using namespace std;

enum TokenKind {
	TK_AUTO,			// KEY 0~31
	TK_BREAK,
	TK_CASE,
	TK_CHAR,
	TK_CONST,
	TK_CONTINUE,
	TK_DEFAULT,
	TK_DO,
	TK_DOUBLE,
	TK_ELSE,
	TK_ENUM,
	TK_EXTERN,
	TK_FLOAT,
	TK_FOR,
	TK_GOTO,
	TK_IF,
	TK_INT,
	TK_LONG,
	TK_REGISTER,
	TK_RETURN,
	TK_SHORT,
	TK_SIGNED,
	TK_SIZEOF,
	TK_STATIC,
	TK_STRUCT,
	TK_SWITCH,
	TK_TYPEDEF,
	TK_UNSIGNED,
	TK_UNION,
	TK_VOID,
	TK_VOLATILE,
	TK_WHILE,
	TK_ID,				// ID 32
	TK_STRING, 			// CONSTANT 33~35
	TK_INTEGER,
	TK_CHARACTER,
	TK_LEFT_BRACKET, 	// OPERATOR 36~71
	TK_RIGHT_BRACKET,
	TK_LEFT_SQUARE,
	TK_RIGHT_SQUARE,
	TK_ARROW,
    TK_INC,
    TK_DEC, 
	TK_ADD,
	TK_SUB,
	TK_MUL,
	TK_DIV,
    TK_MOD,
    TK_LS, // left shift
    TK_RS,
	TK_GTR,
	TK_LSS,
	TK_GEQ,
	TK_LEQ,
	TK_EQU,
    TK_NEQ,
	TK_NOT,
    TK_OR,
    TK_AND,
	TK_BNOT, // bit operator
    TK_BOR,
    TK_BAND,
    TK_BXOR,
    TK_ADDASSIGN,
    TK_SUBASSIGN,
    TK_MULASSIGN,
    TK_DIVASSIGN,
    TK_MODASSIGN,
    TK_ANDASSIGN,
    TK_ORASSIGN,
    TK_XORASSIGN,
	TK_ASSIGN,
	TK_COMMA, 		// delimiter 72~76
	TK_SEMICOLON,
	TK_COLON,
    TK_LCB,
    TK_RCB,
	TK_NULL,
};


class Token {
	private:
		// map stirng to token code
		static map<string, TokenKind> TokenCode;
		// token kind
		TokenKind kind;
		// token value
		string value;
		// location
		size_t line;
		size_t column;

	public:
		// constructor
		Token();
		Token(TokenKind kind, string value, size_t line = 0, size_t column = 0);
		// accesser
		TokenKind GetKind();
		string GetValue();
		size_t GetLine();
		size_t GetColumn();

		static TokenKind getTokenCode(const string& str);
};


class Tokens {
private:
	vector<Token> tokens_;

public:
	void push(Token token);
	void push(TokenKind kind, string value, size_t line = 0, size_t column = 0);
	void clear();
	void append(Tokens tokens);
	vector<Token>::iterator begin();
	vector<Token>::iterator end();
	Token back();
	size_t size() const;
};


class ErrorMsg {
private:
	size_t line;
	size_t column;
	string msg;

public:
	ErrorMsg(size_t line_, size_t column_, string msg_) : line(line_), column(column_), msg(msg_) {}
	void print();
	string toString();
};

using ErrorMsgs = vector<ErrorMsg>;

#endif

