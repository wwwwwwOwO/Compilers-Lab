build: main.cpp Lexer.h Lexer.cpp DataType.h DataType.cpp Globals.h Parser.cpp Parser.h
	g++ main.cpp Lexer.cpp DataType.cpp -o main

main.exe: main.cpp Lexer.h Lexer.cpp DataType.h DataType.cpp Globals.h Parser.cpp Parser.h
	g++ main.cpp Lexer.cpp DataType.cpp Parser.cpp -o main

test: main.exe demo.pas 
	./main.exe demo.pas 

clean:
	del *.o