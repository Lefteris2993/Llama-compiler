.PHONY: clean distclean default

CXX=c++
CXXFLAGS=-Wall -std=c++11

default: Llama

lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

lexer.o: lexer.cpp lexer.hpp parser.hpp ast.hpp error.hpp

parser.hpp parser.cpp: parser.y
	bison -dv -o parser.cpp parser.y

parser.o: parser.cpp lexer.hpp ast.hpp types.hpp error.hpp symbol.hpp

types.o: types.hpp types.cpp

ast.o: ast.cpp ast.hpp

sem.o: sem.cpp ast.hpp

error.o: error.cpp error.hpp parser.hpp

symbol.o: symbol.cpp symbol.hpp

Llama: error.o ast.o types.o symbol.o sem.o lexer.o parser.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o

distclean: clean
	$(RM) Llama
