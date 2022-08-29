#include "DataType.h"


void symbolTable::addEntry(string name){
	for (auto& entry : table)
	{
		if(entry.name == name)	// the symbol is already in the table
			return;
	}
	table.push_back(Entry(name));
}


int symbolTable::getIndex(string name) const { 
	for (int i = 0; i < table.size(); ++i)
	{
		if(table[i].name == name)
			return i;
	}
	return -1;
}


void symbolTable::print() const {
	cout << "SymbolTable: " << endl;
	for(int i = 0; i < table.size(); ++i){
		cout << setw(4) << i << setw(10) << table[i].name << endl;
	}
}

void constantTable::print() const {
	cout << "ConstantTable: " << endl;
	for(int i = 0; i < table.size(); ++i){
		cout << setw(4) << i ;
		switch(table[i].type)
		{
			case DataType::Integer:
				cout << setw(10)<<  *((int*) (buffer + table[i].offset)) ;
				cout << setw(10)<< "Integer" << endl; break;
			case DataType::Real:
				cout << setw(10)<<  *((double*) (buffer + table[i].offset)) ;
				cout << setw(10)<< "Real" << endl; break;
			case DataType::Char:
				cout << setw(10)<<  *((char*) (buffer + table[i].offset)) ;
				cout << setw(10)<< "Char" << endl; break;
		}
	}
}


// default constructor
Token::Token() {}

// constructor
Token::Token(TokenKind kind_, int index_, size_t line_, size_t column_) {
	kind = kind_;
	index = index_;
	line = line_;
	column = column_;
}

// get kind of token
TokenKind Token::GetKind() {
	return this->kind;
}

// get index of token
int Token::GetIndex() {
	return this->index;
}

// get line of token
size_t Token::GetLine() {
	return this->line;
}

// get column of token
size_t Token::GetColumn() {
	return this->column;
}

void Token::print() const
{
	// print kind
	switch(this->kind)
	{
		case TokenKind::KEY:
			cout << 'k';
			break;
		case TokenKind::PARTIAL:
			cout << 'p';
			break;
		case TokenKind::ID:
			cout << 'i';
			break;
		case TokenKind::CONSTANT:
			cout << 'c';
			break;
	}
	
	// print index
	cout << setw(2)<<std::left <<index + 1;
}



// stores token in token list
void Tokens::push(Token token) {
	tokens_.emplace_back(token);
}

void Tokens::push(TokenKind kind, int index, size_t line, size_t column) {
	Token token(kind, index, line, column);
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

void Tokens::print() const
{
	for(auto &token : tokens_)
	{
		token.print();
		cout << " ";
	}
	cout << endl;
}


// reports errors
void ErrorMsg::print() {
	std::cout << "Line " + std::to_string(line) + " Column " + std::to_string(column) + ", " + msg;
}

