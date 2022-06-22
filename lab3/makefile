build: main.o Translator.o 
	g++ main.o Translator.o -o Translator
main.o : Translator.h main.cpp
	g++ -c main.cpp
Translator.o : Translator.h Translator.cpp
	g++ -c Translator.cpp
test: Translator.exe demo.txt
	./Translator.exe demo.txt
clean:
	del *.o