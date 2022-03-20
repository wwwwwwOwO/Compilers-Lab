#include "DataType.h"

// stores keys, legal oparators and delimiters
map<string, TokenKind> Token::TokenCode={
	{"auto", TK_AUTO},  {"break", TK_BREAK},  {"case", TK_CASE}, {"char", TK_CHAR},  
	{"const", TK_CONST},  {"continue", TK_CONTINUE},  {"default", TK_DEFAULT},  {"do", TK_DO},  
	{"double", TK_DOUBLE}, {"else", TK_ELSE},  {"enum", TK_ENUM},  {"extern", TK_EXTERN}, 
	{"float", TK_FLOAT},  {"for", TK_FOR},  {"goto", TK_GOTO},  {"if", TK_IF},  
	{"int", TK_INT},  {"long", TK_LONG},  {"register", TK_REGISTER},  {"return", TK_RETURN},  
	{"short", TK_SHORT},  {"signed", TK_SIGNED},  {"sizeof", TK_SIZEOF},  {"static", TK_STATIC},  
	{"struct", TK_STRUCT},  {"switch", TK_SWITCH},  {"typedef", TK_TYPEDEF},  {"unsigned", TK_UNSIGNED},  
	{"union", TK_UNION},  {"void", TK_VOID},  {"volatile", TK_VOLATILE},  {"while", TK_WHILE},
	{"(", TK_LEFT_BRACKET}, {")", TK_RIGHT_BRACKET},  {"[", TK_LEFT_SQUARE}, {"]", TK_RIGHT_SQUARE},  
	{"->", TK_ARROW}, {"++", TK_INC},  {"--", TK_DEC}, {"+", TK_ADD},  {"-", TK_SUB}, 
	{"*", TK_MUL}, {"/", TK_DIV}, {"%", TK_MOD}, {"<<", TK_LS}, {">>", TK_RS}, {">", TK_GTR}, {"<", TK_LSS}, 
	{">=", TK_GEQ}, {"<=", TK_LEQ}, {"==", TK_EQU}, {"!=", TK_NEQ}, {"!", TK_NOT}, {"||", TK_OR}, 
    {"&&", TK_AND}, {"~", TK_BNOT}, {"^", TK_BOR}, {"&", TK_BAND}, {"|", TK_BXOR}, 
    {"+=", TK_ADDASSIGN}, {"-=", TK_SUBASSIGN}, {"*=", TK_MULASSIGN}, {"/=", TK_DIVASSIGN}, 
    {"%=", TK_MODASSIGN}, {"&=", TK_ANDASSIGN}, {"|=", TK_ORASSIGN}, {"^=", TK_XORASSIGN}, {"=", TK_ASSIGN}, 
	{",", TK_COMMA},{";", TK_SEMICOLON}, {":", TK_COLON}, {"{", TK_LCB}, {"}", TK_RCB}, 
};

// default constructor
Token::Token() {}

// constructor
Token::Token(TokenKind kind_, std::string value_, size_t line_, size_t column_) {
	kind = kind_;
	value = value_;
	line = line_;
	column = column_;
}

// get kind of token
TokenKind Token::GetKind() {
	return this->kind;
}

// get value of token
string Token::GetValue() {
	return this->value;
}

// get line of token
size_t Token::GetLine() {
	return this->line;
}

// get column of token
size_t Token::GetColumn() {
	return this->column;
}

// returns Token Code when str is a keyword, operator or delimiter, if not, returns null
TokenKind Token:: getTokenCode(const string& str){
	if(TokenCode.find(str) == TokenCode.end())
		return TokenKind::TK_NULL;
	else
		return TokenCode[str];
}

// stores token in token list
void Tokens::push(Token token) {
	tokens_.emplace_back(token);
}

void Tokens::push(TokenKind kind, std::string value, size_t line, size_t column) {
	Token token(kind, value, line, column);
	tokens_.emplace_back(token);
}

void Tokens::clear() {
	tokens_.clear();
}

void Tokens::append(Tokens tokens) {
	tokens_.insert(tokens_.end(), tokens.begin(), tokens.end());
}

vector<Token>::iterator Tokens::begin() {
	return tokens_.begin();
}

vector<Token>::iterator Tokens::end() {
	return tokens_.end();
}

Token Tokens::back() {
	return tokens_.back();
}

size_t Tokens::size() const {
	return tokens_.size();
}

// reports errors
std::string ErrorMsg::toString() {
	return "Line " + std::to_string(line) + " Column " + std::to_string(column) + ", " + msg;
}

void ErrorMsg::print() {
	std::cout << toString() << std::endl;
}

