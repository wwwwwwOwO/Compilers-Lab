#ifndef DATATYPE_H
#define DATATYPE_H


#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <map>
#include <stack>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <queue>
#include <set>
#include <cassert>
#include <iomanip>
#include <ctime>
#include <list>
#include <cctype>
#include <functional>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

using namespace std;

enum KeyType {
	PROGRAM,	// program
	VAR,		// var
	INTEGER,	// integer
	REAL,		// real
	CHAR,		// char
	BEGIN,		// begin
	END			// end
};


enum PartialType {
	COMMA, 		// ,
	COLON,		// :
	SEMICOLON,	// ;
	ASSIGN,		// :=
	MUL,		// *
	DIV,		// /
	ADD,		// +
	SUB,		// -
    DOT,		// .
	LB, 		// (
	RB			// )
};

enum TokenKind
{
	KEY,		// Keys
	PARTIAL,	// Partials
	ID,			// Identifiers
	CONSTANT,	// Constants
	TERM
};

enum DataType
{
	Integer,		// integer
	Real,		// real
	Char		// char
};

enum Category
{
	function,	// functions
	value 		// values
};

class symbolTable{
public:
	struct Entry{
		string name;
		DataType type;
		Category cat;
		uint64_t addr;
		
		Entry(string id)
		{
			name = id;
		}
	};

	vector<Entry> table;
	// add an entry to the symbol table
	void addEntry(string name);
	// get index of the entry specified by name
	int getIndex(string name) const;
	// print the symbol table
	void print() const;
};


class constantTable{
	struct Entry{
		DataType type;
		int offset;

		Entry(DataType t, int of)
		{
			type = t;
			offset = of;
		}
	};
public:
	char* buffer;
	int p;
	vector<Entry> table;

	constantTable(){ buffer = (char*) malloc(1024); p = 0; }
	~constantTable(){ free(buffer); }

	// add an entry to the constant table
	template<typename T>
	void addEntry(T value, DataType type){
		for(auto& entry : table)
		{
			if(entry.type == type && *((T*) (buffer + entry.offset)) == value)
				return;
		}

		table.push_back(Entry(type, p));
		*((T*) (buffer + p)) = value;
		p += sizeof(T);
	}

	// get index of the entry specified by data type and value
	template<typename T>
	int getIndex(T value, DataType type) const {
		for (int i = 0; i < table.size(); ++i)
		{
			if(table[i].type == type && *((T*) (buffer + table[i].offset)) == value)
				return i;
		}
		return -1;
	}
	
	// print the constant table
	void print() const;
};


class Token {
	private:
		// token kind
		TokenKind kind;
		// token index
		int index;
		// location
		size_t line;
		size_t column;

	public:
		// constructor
		Token();
		Token(TokenKind kind, int index, size_t line = 0, size_t column = 0);
		// accesser
		TokenKind GetKind();
		int GetIndex();
		size_t GetLine();
		size_t GetColumn();
		void print() const;
};


class Tokens {
private:
	vector<Token> tokens_;

public:
	void push(Token token);
	void push(TokenKind kind, int index, size_t line = 0, size_t column = 0);
	void clear();
	void append(Tokens tokens);
	vector<Token>::iterator begin();
	vector<Token>::iterator end();
	Token back();
	size_t size() const;
	void print() const;
};


class ErrorMsg {
private:
	size_t line;
	size_t column;
	string msg;

public:
	ErrorMsg(size_t line_, size_t column_, string msg_) : line(line_), column(column_), msg(msg_) {}
	void print();
};

using ErrorMsgs = vector<ErrorMsg>;


struct Quat{
    Token op;   // TokenKind::KEY, TokenKind::PARTIAL
    Token a, b; // TokenKind::ID, TokenKind::CONSTANT
    Token res;  // TokenKind::ID
};



#endif

