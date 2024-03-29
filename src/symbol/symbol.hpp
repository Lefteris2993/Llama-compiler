/******************************************************************************
 *  CVS version:
 *     $Id: symbol.h,v 1.1 2003/05/13 22:21:01 nickie Exp $
 ******************************************************************************
 *
 *  C header file : symbol.hpp
 *  Project       : PCL Compiler
 *  Version       : 1.0 alpha
 *  Written by    : Nikolaos S. Papaspyrou (nickie@softlab.ntua.gr)
 *  Date          : May 14, 2003
 *  Description   : Generic symbol table in C
 *  Modified by   : Chaidemenos E., Siana A.
 *
 */

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string>
#include <llvm/IR/Value.h>

#include "../types/types.hpp"

#define START_POSITIVE_OFFSET 8
#define START_NEGATIVE_OFFSET 0

enum EntryType { 
  ENTRY_VARIABLE, 
  ENTRY_CONSTANT,
  ENTRY_FUNCTION,
  ENTRY_PARAMETER,
  ENTRY_TEMPORARY,
  ENTRY_LLVM
};

enum FunDefStatus {
  FUN_DEF_DEFINE,
  FUN_DEF_COMPLETE
};

struct SymbolEntry {
  std::string id;
  EntryType entryType;
  unsigned nestingLevel;
  unsigned hashValue;
  SymbolEntry *nextInHashTable;
  SymbolEntry *nextInScope;
  Type *type;
};

struct VarSymbolEntry: SymbolEntry {};

struct ParSymbolEntry: SymbolEntry {
  ParSymbolEntry *nextParam;
};

struct FunSymbolEntry: SymbolEntry {
  ParSymbolEntry *firstArgument;
  ParSymbolEntry *lastArgument;
  FunDefStatus status;
  unsigned paramNum;
  Type *returnType;
  Type **paramTypes;
};

struct LLVMSymbolEntry: SymbolEntry {
  llvm::Value *value;
};

struct Scope {
  unsigned nestingLevel;
  Scope *parent;
  SymbolEntry *entries;
};

class SymbolTable {
public:
  SymbolTable(unsigned size);

  void openScope();
  void closeScope();

  VarSymbolEntry *newVariable(std::string name, Type *type, unsigned lineno = -1);
  FunSymbolEntry *newFunction(std::string name, Type *type, unsigned lineno = -1);
  ParSymbolEntry *newParameter(std::string name, Type *type, FunSymbolEntry *f, unsigned lineno = -1);
  LLVMSymbolEntry *newLLVMValue(std::string name, llvm::Value *v, unsigned lineno = -1);

  void endFunctionDef(FunSymbolEntry *f, Type *type, unsigned lineno = -1);
  template <class T>
  T *lookupEntry(std::string name, bool err, unsigned lineno = -1);

  Scope *currentScope;

private:
  template <class T>
  T *newEntry(std::string name, Type *type, unsigned lineno = -1);

  unsigned hashTableSize;
  SymbolEntry **hashTable;
};

#endif
