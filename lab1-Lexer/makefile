build: main.o Lexer.o DataType.o
	g++ main.o Lexer.o DataType.o -o Lexer
main.o : DataType.h Lexer.h main.cpp
	g++ -c main.cpp
Lexer.o : DataType.h Lexer.h Lexer.cpp
	g++ -c Lexer.cpp
DataType.o: DataType.h DataType.cpp
	g++ -c DataType.cpp
test: Lexer.exe demo.c 
	./Lexer.exe demo.c tokens.txt
clean:
	del *.o