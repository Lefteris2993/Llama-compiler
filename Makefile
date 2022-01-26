.PHONY: clean distclean default

CXX=c++
CXXFLAGS=-Wall -std=c++11

default: Llama

lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

lexer.o: lexer.cpp lexer.hpp parser.hpp

parser.hpp parser.cpp: parser.y
	bison -dv -o parser.cpp parser.y

parser.o: parser.cpp lexer.hpp

Llama: lexer.o parser.o
	$(CXX) $(CXXFLAGS) -o Llama lexer.o parser.o

clean:
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o

distclean: clean
	$(RM) Llama