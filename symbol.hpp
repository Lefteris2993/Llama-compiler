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
#include "types.hpp"

#define START_POSITIVE_OFFSET 8
#define START_NEGATIVE_OFFSET 0

enum EntryType { 
  ENTRY_VARIABLE, 
  ENTRY_CONSTANT,
  ENTRY_FUNCTION,
  ENTRY_PARAMETER,
  ENTRY_TEMPORARY
};

enum FunDefStatus {
  FUN_DEF_DEFINE,
  FUN_DEF_COMPLETE
};

struct SymbolEntry {
  virtual ~SymbolEntry();
  std::string id;
  EntryType entryType;
  unsigned nestingLevel;
  unsigned hashValue;
  SymbolEntry *nextHash;
  SymbolEntry *nextInScope;
  Type *type;
};

struct VarSymbolEntry: SymbolEntry {};

struct ParSymbolEntry: SymbolEntry {
  ParSymbolEntry *next;
};

struct FunSymbolEntry: SymbolEntry {
  FunSymbolEntry();
  ~FunSymbolEntry();
  ParSymbolEntry *firstArgument;
  ParSymbolEntry *lastArgument;
  int firstQuad;
  FunDefStatus status;
  unsigned paramNum;
  Type **paramTypes;
};

enum LookupType { 
  LOOKUP_CURRENT_SCOPE, 
  LOOKUP_ALL_SCOPES 
};

struct Scope {
  unsigned nestingLevel;
  Scope *parent;
  SymbolEntry *entries;
};

class SymbolTable {
public:
  SymbolTable(unsigned size);
  ~SymbolTable();

  void openScope();
  void closeScope();

  VarSymbolEntry *newVariable(std::string name, Type *type, unsigned lineno = -1);
  FunSymbolEntry *newFunction(std::string name, Type *type, unsigned lineno = -1);
  ParSymbolEntry *newParameter(std::string name, Type *type, FunSymbolEntry *f, unsigned lineno = -1);

  void endFunctionDef(FunSymbolEntry *f, Type *type, unsigned lineno = -1);
  template <class T>
  T *lookupEntry(std::string name, LookupType type, bool err, unsigned lineno = -1);

  Scope *currentScope;

private:
  void insertEntry(SymbolEntry *e);
  template <class T>
  T *newEntry(std::string name, unsigned lineno = -1);

  unsigned tempNumber;
  unsigned hashTableSize;
  SymbolEntry **hashTable;
};

#endif
