#include "Lexer.h"
#include "Parser.h"
#include "Globals.h"


int main(int argc, char* argv[]) {
	// check args
    if(argc != 2) {
        cout << "Please input the correct file." << endl;
        exit(1);
    }

    Tokens tokens;

	// lexical analysis
    //  input:  file that contains the source code
    // output:  tokens, KeyTable, PartialTable, SymbolTable and ConstantTable (KeyTable and PartialTable is constant.)
	Lexer* lexer = new Lexer();
	lexer->run(argv[1], tokens);


    // check output
	cout << BLU"[DEBUG] "<< RESET;
    tokens.print();
    SymbolTable.print();
    ConstantTable.print();

	// Syntactic analysis
	Parser* parser = new Parser();
	parser->run(tokens);



}