build: main.o Parser.o 
	g++ main.o Parser.o -o Parser
main.o : Parser.h main.cpp
	g++ -c main.cpp
Parser.o : Parser.h Parser.cpp
	g++ -c Parser.cpp
test: Parser.exe demo.txt
	./Parser.exe demo.txt
clean:
	del *.o