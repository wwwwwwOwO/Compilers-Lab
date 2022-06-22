#include "Translator.h"

int main(int argc, char* argv[]) {
	// check args
    if(argc != 2) {
        cout << "Please input the correct file." << endl;
        exit(1);
    }
	
	// Syntactic analysis
	Translator* translator = new Translator();
	translator->run(argv[1]);
}