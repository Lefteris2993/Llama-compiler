#include "library.hpp"
#include "../types/types.hpp"

Library::Library() { /* do nothing */ }

void Library::insertFunctionsToScope(SymbolTable *sTable) {
  FunctionType *funcType;
  FunSymbolEntry *f;

  /* print_int */
  funcType = new FunctionType(intType, unitType);
  f = sTable->newFunction("print_int", funcType, -1);
  f->paramNum = 1;
  f->returnType = unitType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = intType;

  /* print_string */
  funcType = new FunctionType(stringType, unitType);
  f = sTable->newFunction("print_string", funcType, -1);
  f->paramNum = 1;
  f->returnType = unitType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = stringType;
}