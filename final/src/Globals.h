#ifndef GLOBALS_H
#define GLOBALS_H

#include "DataType.h"


vector<KeyType> KeyTable = {
	KeyType::PROGRAM,		// program
	KeyType::VAR,			// var
	KeyType::INTEGER,		// integer
	KeyType::REAL,			// real
	KeyType::CHAR,			// char
	KeyType::BEGIN,			// begin
	KeyType::END			// end
};

vector<PartialType> PartialTable = {
	PartialType::COMMA, 		// ,
	PartialType::COLON,			// :
	PartialType::SEMICOLON,		// ;
	PartialType::ASSIGN,		// :=
	PartialType::MUL,			// *
	PartialType::DIV,			// /
	PartialType::ADD,			// +
	PartialType::SUB,			// -
    PartialType::DOT,			// .
	PartialType::LB, 			// (
	PartialType::RB				// )
};


symbolTable SymbolTable;
constantTable ConstantTable;

// visible production rules
vector<string> ProductionRules =
{
    "P->k0iGp8",
    "G->VC",
    "V->k1Ip1Dp2",
    "I->iA",
    "A->p0iA",
    "A->@",
    "D->k2",
    "D->k3",
    "D->k4",
    "C->k5Qk6",
    "Q->SB",
    "B->p2SB",
    "B->@",
    "S->ip3E",
    "E->TM",
    "M->p6TM",
    "M->p7TM",
    "M->@",
    "T->FN",
    "N->p4FN",
    "N->p5FN",
    "N->@",
    "F->p9Ep10",
    "F->i",
    "F->c"
};

// production rules with invisible characters, used by parser
vector<string> productionRules;
map<char, Token> Char2Token;
map<int, char> Key2Char;
map<int, char> Partial2Char;

char startChar; 

vector<vector<int>> AnalysisTable;

vector<string> productionRulesWithActions;

#endif
