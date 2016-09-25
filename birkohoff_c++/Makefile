
CFlAGS = -std=c++11 -Wall
CC = g++ $(CFlAGS)



bi.out: bi.o term.o parser.o lexer.o 
	$(CC) -o $@ $^

bi.o: bi.cpp term.hpp term.cpp
	$(CC) -c -o $@ $<

term.o: term.cpp term.hpp
	$(CC) -c -o $@ $<

parser.o: parser.cpp term.hpp
	$(CC) -c -o $@ $<

lexer.o: lexer.cpp parser.cpp
	$(CC) -Wno-unused-function -c -o $@ $< #zali se za neku unput f-ju

parser.cpp: parser.ypp
	bison -d -o $@ $<

lexer.cpp: lexer.lpp
	flex -o $@ $<


clean:
	rm -f *.o
	rm -f *~
	rm -f parser.cpp
	rm -f lexer.cpp
	rm -f parser.hpp
	rm -f main
