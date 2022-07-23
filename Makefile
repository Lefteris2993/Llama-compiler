.PHONY: clean distclean default

CXX=c++
CXXFLAGS=-Wall -std=c++11

default: Llama

lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

lexer.o: lexer.cpp lexer.hpp parser.hpp ast.hpp general.hpp error.hpp

parser.hpp parser.cpp: parser.y
	bison -dv -o parser.cpp parser.y

parser.o: parser.cpp lexer.hpp ast.hpp types.hpp general.hpp error.hpp symbol.hpp

Llama: lexer.o parser.o
	$(CXX) $(CXXFLAGS) -o Llama lexer.o parser.o types.cpp general.cpp error.cpp symbol.cpp

clean:
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o

distclean: clean
	$(RM) Llama
