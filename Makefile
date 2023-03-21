.PHONY: clean distclean default

LLVMCONFIG=llvm-config-11
CXX=clang++
CXXFLAGS=-Wall -std=c++11 -g -O0 -ggdb3 `$(LLVMCONFIG) --cxxflags`
LDFLAGS=`$(LLVMCONFIG) --ldflags --system-libs --libs all`
PYTHON=python3

AST_SOURCES = $(shell find src/ast -name '*.cpp')

default: Llama

lexer.cpp: src/lexer.l
	flex -s -o lexer.cpp src/lexer.l

lexer.o: lexer.cpp src/lexer.hpp parser.hpp src/ast/ast.hpp src/error/error.hpp

parser.hpp parser.cpp: src/parser.y
	bison -dv -o parser.cpp src/parser.y

parser.o: parser.cpp src/lexer.hpp src/ast/ast.hpp src/types/types.hpp src/error/error.hpp src/symbol/symbol.hpp src/library/library.hpp

# %.o: %.cpp ${HEADERS} parser.hpp
# 	${CXX} ${CXXFLAGS} ${LDFLAGS} -c $< -o $@

ast.o: src/ast/ast.cpp src/ast/ast.hpp ${AST_SOURCES}
	${CXX} ${CXXFLAGS} -c $< -o $@

error.o: src/error/error.cpp src/error/error.hpp parser.hpp
	${CXX} ${CXXFLAGS} -c $< -o $@

symbol.o: src/symbol/symbol.cpp src/symbol/symbol.hpp
	${CXX} ${CXXFLAGS} -c $< -o $@

types.o: src/types/types.cpp src/types/types.hpp
	${CXX} ${CXXFLAGS} -c $< -o $@

library.o: src/library/library.cpp src/library/library.hpp
	${CXX} ${CXXFLAGS} -c $< -o $@

Llama: ast.o error.o symbol.o types.o parser.o lexer.o library.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test: Llama
	$(PYTHON) runTests.py

updateASTs: Llama
	$(PYTHON) updateAst.py

clean:
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output
	find . -type f -name '*.o' -delete

distclean: clean
	$(RM) Llama
