#include"Lexer.h"

int main(int argc, char* argv[]) {
	// check args
    if(argc != 3) {
        cout << "Please input the correct file." << endl;
        exit(1);
    }

	// lexical analysis
	Lexer* lexer = new Lexer();
	lexer->run(argv[1], argv[2]);
}