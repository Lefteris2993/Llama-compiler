.PHONY: clean distclean default

SOURCES = $(shell find src/ast src/error src/symbol src/types -name '*.cpp')
HEADERS = $(shell find src/ast src/error src/symbol src/types -name '*.hpp')
OBJ = ${SOURCES:.cpp=.o}

LLVMCONFIG=llvm-config
CXX=clang++
CXXFLAGS=-Wall -std=c++11 -g #`$(LLVMCONFIG) --cxxflags`
LDFLAGS=`$(LLVMCONFIG) --ldflags --system-libs --libs all`
PYTHON=python3

default: Llama

lexer.cpp: src/lexer.l
	flex -s -o lexer.cpp src/lexer.l

lexer.o: lexer.cpp src/lexer.hpp parser.hpp src/ast/ast.hpp src/error/error.hpp

parser.hpp parser.cpp: src/parser.y
	bison -dv -o parser.cpp src/parser.y

parser.o: parser.cpp src/lexer.hpp src/ast/ast.hpp src/types/types.hpp src/error/error.hpp src/symbol/symbol.hpp

# %.o: %.cpp ${HEADERS} parser.hpp
# 	${CXX} ${CXXFLAGS} ${LDFLAGS} -c $< -o $@

%.o: %.cpp ${HEADERS} parser.hpp
	${CXX} ${CXXFLAGS} -c $< -o $@

Llama: ${OBJ} parser.o lexer.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test: Llama
	$(PYTHON) runTests.py

clean:
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output
	find . -type f -name '*.o' -delete

distclean: clean
	$(RM) Llama
