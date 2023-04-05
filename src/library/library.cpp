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

  /* print_char */
  funcType = new FunctionType(charType, unitType);
  f = sTable->newFunction("print_char", funcType, -1);
  f->paramNum = 1;
  f->returnType = unitType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = charType;

  /* print_bool */
  funcType = new FunctionType(boolType, unitType);
  f = sTable->newFunction("print_bool", funcType, -1);
  f->paramNum = 1;
  f->returnType = unitType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = boolType;

  /* read_int */
  funcType = new FunctionType(unitType, intType);
  f = sTable->newFunction("read_int", funcType, -1);
  f->paramNum = 1;
  f->returnType = intType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = unitType;

  /* read_bool */
  funcType = new FunctionType(unitType, boolType);
  f = sTable->newFunction("read_bool", funcType, -1);
  f->paramNum = 1;
  f->returnType = boolType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = unitType;

  /* read_char */
  funcType = new FunctionType(unitType, charType);
  f = sTable->newFunction("read_char", funcType, -1);
  f->paramNum = 1;
  f->returnType = charType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = unitType;

  /* read_string */
  funcType = new FunctionType(stringType, unitType);
  f = sTable->newFunction("read_string", funcType, -1);
  f->paramNum = 1;
  f->returnType = unitType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = stringType;

  /* abs */
  funcType = new FunctionType(intType, intType);
  f = sTable->newFunction("abs", funcType, -1);
  f->paramNum = 1;
  f->returnType = intType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = intType;

  auto intRefType = new RefType(intType);

  /* incr */
  funcType = new FunctionType(intRefType, unitType);
  f = sTable->newFunction("incr", funcType, -1);
  f->paramNum = 1;
  f->returnType = unitType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = intRefType;

  /* decr */
  funcType = new FunctionType(intRefType, unitType);
  f = sTable->newFunction("decr", funcType, -1);
  f->paramNum = 1;
  f->returnType = unitType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = intRefType;

  /* int_of_char */
  funcType = new FunctionType(charType, intType);
  f = sTable->newFunction("int_of_char", funcType, -1);
  f->paramNum = 1;
  f->returnType = intType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = charType;

  /* char_of_int */
  funcType = new FunctionType(intType, charType);
  f = sTable->newFunction("char_of_int", funcType, -1);
  f->paramNum = 1;
  f->returnType = charType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = intType;

  /* strlen */
  funcType = new FunctionType(stringType, intType);
  f = sTable->newFunction("strlen", funcType, -1);
  f->paramNum = 1;
  f->returnType = intType;
  f->paramTypes = new Type *[1];
  f->paramTypes[0] = stringType;

  /* strcmp */
  funcType = new FunctionType(new FunctionType(stringType, stringType), intType);
  f = sTable->newFunction("strcmp", funcType, -1);
  f->paramNum = 2;
  f->returnType = intType;
  f->paramTypes = new Type *[2];
  f->paramTypes[0] = stringType;
  f->paramTypes[1] = stringType;

  /* strcpy */
  funcType = new FunctionType(new FunctionType(stringType, stringType), unitType);
  f = sTable->newFunction("strcpy", funcType, -1);
  f->paramNum = 2;
  f->returnType = unitType;
  f->paramTypes = new Type *[2];
  f->paramTypes[0] = stringType;
  f->paramTypes[1] = stringType;

  /* strcat */
  funcType = new FunctionType(new FunctionType(stringType, stringType), unitType);
  f = sTable->newFunction("strcat", funcType, -1);
  f->paramNum = 2;
  f->returnType = unitType;
  f->paramTypes = new Type *[2];
  f->paramTypes[0] = stringType;
  f->paramTypes[1] = stringType;
}