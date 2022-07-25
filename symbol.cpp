/******************************************************************************
 *  CVS version:
 *     $Id: symbol.c,v 1.3 2004/05/05 22:00:08 nickie Exp $
 ******************************************************************************
 *
 *  C code file   : symbol.cpp
 *  Project       : PCL Compiler
 *  Version       : 1.0 alpha
 *  Written by    : Nikolaos S. Papaspyrou (nickie@softlab.ntua.gr)
 *  Date          : May 14, 2003
 *  Description   : Generic symbol table in C
 *  Modified by   : Chaidemenos E., Siana A.
 *
 */

#include "symbol.hpp"
#include "error.hpp"


// This function is used to force the compiler to create template functions. It is never callled.
void Templates() {
  SymbolTable *s = new SymbolTable(42);
  s->lookupEntry<SymbolEntry>("", LOOKUP_CURRENT_SCOPE, false);
  s->lookupEntry<FunSymbolEntry>("", LOOKUP_CURRENT_SCOPE, false);
}

typedef unsigned long int HashType;

static HashType PJW_hash (const char * key) {
  const HashType PJW_OVERFLOW =
    (((HashType) 0xf) << (8 * sizeof(HashType) - 4));
  const int PJW_SHIFT = (8 * (sizeof(HashType) - 1));
  
  HashType h, g;
  
  for (h = 0; *key != '\0'; key++) {
    h = (h << 4) + (*key);
    if ((g = h & PJW_OVERFLOW) != 0) {
      h ^= g >> PJW_SHIFT;
      h ^= g;
    }
  }
  return h;
}

SymbolTable::SymbolTable(unsigned size) {
  unsigned int i;
      
  currentScope = nullptr;
  tempNumber   = 1;
      
  hashTableSize = size;
  hashTable = new SymbolEntry *[size];
  
  for (i = 0; i < size; i++)
    hashTable[i] = nullptr;
}

SymbolTable::~SymbolTable() {
  // // TODO:
  // unsigned int i;
      
  // for (i = 0; i < hashTableSize; i++) {
  //   if (hashTable[i] != nullptr) {
  //     delete hashTable[i];
  //   }
  // }
    
      
  // delete currentScope;
}

void SymbolTable::openScope() {
  Scope * newScope = new Scope();

  newScope->parent    = currentScope;
  newScope->entries   = nullptr;

  if (currentScope == nullptr)
    newScope->nestingLevel = 1;
  else
    newScope->nestingLevel = currentScope->nestingLevel + 1;
  
  currentScope = newScope;
}

void SymbolTable::closeScope() {
  SymbolEntry * e = currentScope->entries;
  Scope       * t = currentScope;
  
  while (e != nullptr) {
    SymbolEntry * next = e->nextInScope;
    
    hashTable[e->hashValue] = e->nextHash;
    delete e;
    e = next;
  }
  
  currentScope = currentScope->parent;
  delete t;
}

void SymbolTable::insertEntry(SymbolEntry *e) {
  e->nextHash             = hashTable[e->hashValue];
  hashTable[e->hashValue] = e;
  e->nextInScope          = currentScope->entries;
  currentScope->entries   = e;
}

template <class T>
T *SymbolTable::newEntry(std::string name, unsigned lineno) {
  SymbolEntry * e;
    
  for (e = currentScope->entries; e != nullptr; e = e->nextInScope)
    if (name.compare(e->id) == 0) {
      Logger::error(lineno, "Duplicate identifier: %s", name.c_str());
      return nullptr;
    }

  T *newEntity = new T();
  newEntity->id = name;

  newEntity->hashValue    = PJW_hash(name.c_str()) % hashTableSize;
  newEntity->nestingLevel = currentScope->nestingLevel;
  insertEntry(newEntity);
  return newEntity;
}

VarSymbolEntry *SymbolTable::newVariable(std::string name, Type *type, unsigned lineno) {
  VarSymbolEntry * e = newEntry<VarSymbolEntry>(name, lineno);
  
  if (e != nullptr) {
    e->entryType = ENTRY_VARIABLE;
    e->type = type;
  }
  return e;
}

FunSymbolEntry *SymbolTable::newFunction(std::string name, unsigned lineno) {
  FunSymbolEntry * e = lookupEntry<FunSymbolEntry>(name, LOOKUP_CURRENT_SCOPE, false);

  if (e == nullptr) {
    e = newEntry<FunSymbolEntry>(name);
    if (e != nullptr) {
      e->entryType = ENTRY_FUNCTION;
      e->firstArgument = e->lastArgument = nullptr;
      e->type = nullptr;
    }
    return ((FunSymbolEntry*) e);
  }
  else {
    Logger::error(lineno, "Duplicate identifier: %s", name.c_str());
    return nullptr;
  }
}

ParSymbolEntry *SymbolTable::newParameter(std::string name, Type *type, FunSymbolEntry *f, unsigned lineno) {
  ParSymbolEntry * e;
  
  if (f->entryType != ENTRY_FUNCTION)
    Logger::internal("Cannot add a parameter to a non-function");
  switch (f->status) {
    case FunDefStatus::FUN_DEF_DEFINE:
      e = newEntry<ParSymbolEntry>(name, lineno);
      if (e != nullptr) {
        e->entryType = ENTRY_PARAMETER;
        e->id = name;
        e->type = type;
        e->next = nullptr;
      }
      if (f->lastArgument == nullptr) {
        f->firstArgument = f->lastArgument = e;
      }
      else {
        f->lastArgument->next = e;
        f->lastArgument = e;
      }
      return e;            
    case FunDefStatus::FUN_DEF_COMPLETE:
      Logger::fatal("Cannot add a parameter to an already defined function");
  }
  return nullptr;
}

void SymbolTable::endFunctionDef(FunSymbolEntry *f, Type *type, unsigned lineno) {
  if (f->entryType != ENTRY_FUNCTION)
    Logger::internal("Cannot end parameters in a non-function");
  switch (f->status) {
    case FunDefStatus::FUN_DEF_COMPLETE:
      Logger::internal("Cannot end parameters in an already defined function");
      break;
    case FunDefStatus::FUN_DEF_DEFINE:
      f->type = type;
      break;
  }
  f->status = FunDefStatus::FUN_DEF_COMPLETE;
}

SymbolEntry::~SymbolEntry() {}

FunSymbolEntry::FunSymbolEntry() {
  status = FunDefStatus::FUN_DEF_DEFINE;
}
FunSymbolEntry::~FunSymbolEntry() {
  // // TODO:
  // ParSymbolEntry *args = this->firstArgument;
  // while (args != nullptr) {
  //   ParSymbolEntry * p = args;
  //   args = args->next;
  //   delete p;
  // }
}

// TODO: does this implement screarch throw scopes correctly? 
template <class T>
T *SymbolTable::lookupEntry(std::string name, LookupType type, bool err, unsigned lineno) {
  unsigned int  hashValue = PJW_hash(name.c_str()) % hashTableSize;
  SymbolEntry * e         = hashTable[hashValue];
  
  switch (type) {
    case LOOKUP_CURRENT_SCOPE:
      while (e != nullptr && e->nestingLevel == currentScope->nestingLevel)
        if (name.compare(e->id) == 0)
          return (T *)e;
        else
          e = e->nextHash;
      break;
    case LOOKUP_ALL_SCOPES:
      while (e != nullptr)
        if (name.compare(e->id) == 0)
          return (T *)e;
        else
          e = e->nextHash;
      break;
  }
  
  if (err)
    Logger::error(lineno, "Unknown identifier: %s", name.c_str());
  return nullptr;
}
