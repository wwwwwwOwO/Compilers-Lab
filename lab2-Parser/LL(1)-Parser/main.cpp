#include "Parser.h"

int main(int argc, char* argv[]) {
	// check args
    if(argc != 2) {
        cout << "Please input the correct file." << endl;
        exit(1);
    }
	
	// Syntactic analysis
	Parser* parser = new Parser();
	parser->run(argv[1]);
}