#include "library.hpp"
#include "../types/types.hpp"

Library::Library() { /* do nothing */ }

void Library::insertFunctionsToScope(SymbolTable *sTable) {
  FunctionType *funcType = new FunctionType(intType, unitType);
  FunSymbolEntry *f = sTable->newFunction("print_int", funcType, -1);
  f->paramNum = 1;
  f->returnType = unitType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = intType;
}