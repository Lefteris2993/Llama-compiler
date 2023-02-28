#ifndef __LIBRARY_HPP__
#define __LIBRARY_HPP__

#include "../symbol/symbol.hpp"

class Library {
public:
  Library();
  static void insertFunctionsToScope(SymbolTable *sTable);
};


#endif