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
#include "general.hpp"

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
      
  currentScope = NULL;
  tempNumber   = 1;
      
  hashTableSize = size;
  hashTable = (SymbolEntry **) allocate(size * sizeof(SymbolEntry *));
  
  for (i = 0; i < size; i++)
    hashTable[i] = NULL;
}

SymbolTable::~SymbolTable() {
  unsigned int i;
      
  for (i = 0; i < hashTableSize; i++)
    if (hashTable[i] != NULL)
      delete(hashTable[i]);

  deallocate(hashTable);
}

void SymbolTable::openScope() {
  Scope * newScope = (Scope *) allocate(sizeof(Scope));

  newScope->parent    = currentScope;
  newScope->entries   = NULL;

  if (currentScope == NULL)
    newScope->nestingLevel = 1;
  else
    newScope->nestingLevel = currentScope->nestingLevel + 1;
  
  currentScope = newScope;
}

void SymbolTable::closeScope() {
  SymbolEntry * e = currentScope->entries;
  Scope       * t = currentScope;
  
  while (e != NULL) {
    SymbolEntry * next = e->nextInScope;
    
    hashTable[e->hashValue] = e->nextHash;
    delete(e);
    e = next;
  }
  
  currentScope = currentScope->parent;
  deallocate(t);
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
    
  for (e = currentScope->entries; e != NULL; e = e->nextInScope)
    if (name.compare(e->id) == 0) {
      Logger::error(lineno, "Duplicate identifier: %s", name.c_str());
      return NULL;
    }

  T *newEntity = (T *) allocate(sizeof(T));
  newEntity->id = name;

  newEntity->hashValue    = PJW_hash(name.c_str()) % hashTableSize;
  newEntity->nestingLevel = currentScope->nestingLevel;
  insertEntry(newEntity);
  return newEntity;
}

VarSymbolEntry *SymbolTable::newVariable(std::string name, Type *type, unsigned lineno) {
  VarSymbolEntry * e = newEntry<VarSymbolEntry>(name, lineno);
  
  if (e != NULL) {
    e->entryType = ENTRY_VARIABLE;
    e->type = type;
  }
  return e;
}

FunSymbolEntry *SymbolTable::newFunction(std::string name, unsigned lineno) {
  FunSymbolEntry * e = ( FunSymbolEntry* ) lookupEntry(name, LOOKUP_CURRENT_SCOPE, false);

  if (e == NULL) {
    e = newEntry<FunSymbolEntry>(name);
    if (e != NULL) {
      e->entryType = ENTRY_FUNCTION;
      e->isForward = false;
      e->firstArgument = e->lastArgument = NULL;
      e->type = NULL;
    }
    return ((FunSymbolEntry*) e);
  }
  else if (e->entryType == ENTRY_FUNCTION && e->isForward) {
    e->isForward = false;
    e->lastArgument = NULL;
    return e;
  }
  else {
    Logger::error(lineno, "Duplicate identifier: %s", name.c_str());
    return NULL;
  }
}

ParSymbolEntry *SymbolTable::newParameter(std::string name, Type *type, FunSymbolEntry *f, unsigned lineno) {
  ParSymbolEntry * e;
  
  if (f->entryType != ENTRY_FUNCTION)
    Logger::internal("Cannot add a parameter to a non-function");
  switch (f->pardef) {
    case FunSymbolEntry::PARDEF_DEFINE:
      e = newEntry<ParSymbolEntry>(name, lineno);
      if (e != NULL) {
        e->entryType = ENTRY_PARAMETER;
        e->type = type;
        e->next = NULL;
      }
      if (f->lastArgument == NULL)
        f->firstArgument = f->lastArgument = e;
      else {
        f->lastArgument->next = e;
        f->lastArgument = e;
      }
      return e;            
    case FunSymbolEntry::PARDEF_CHECK:
      e = f->lastArgument;
      if (e == NULL)
        e = f->firstArgument;
      else
        e = e->next;
      if (e == NULL)
        Logger::error(lineno, "More parameters than expected in redeclaration "
          "of function %s", f->id.c_str());
      else if (!Type::equal_types(e->type, type))
        Logger::error(lineno, "Parameter type mismatch in redeclaration "
          "of function %s", f->id.c_str());
      else if (name.compare(e->id) != 0)
        Logger::error(lineno, "Parameter name mismatch in redeclaration "
          "of function %s", f->id.c_str());
      else
        insertEntry(e);
      f->lastArgument = e;
      return e;
    case FunSymbolEntry::PARDEF_COMPLETE:
      Logger::fatal("Cannot add a parameter to an already defined function");
  }
  return NULL;
}

void SymbolTable::forwardFunction(FunSymbolEntry * f) {
  if (f->entryType != ENTRY_FUNCTION)
    Logger::internal("Cannot make a non-function forward");
  f->isForward = true;
}

void SymbolTable::endFunctionHeader(FunSymbolEntry *f, Type *type, unsigned lineno) {
  if (f->entryType != ENTRY_FUNCTION)
    Logger::internal("Cannot end parameters in a non-function");
  switch (f->pardef) {
    case FunSymbolEntry::PARDEF_COMPLETE:
      Logger::internal("Cannot end parameters in an already defined function");
      break;
    case FunSymbolEntry::PARDEF_DEFINE:
      f->type = type;
      break;
    case FunSymbolEntry::PARDEF_CHECK:
      if (
        (f->lastArgument != NULL && f->lastArgument->next != NULL) ||
        (f->lastArgument == NULL && f->firstArgument != NULL)
      )
        Logger::error(lineno, "Fewer parameters than expected in redeclaration "
          "of function %s", f->id.c_str());
      if (!Type::equal_types(f->type, type))
        Logger::error(lineno, "Result type mismatch in redeclaration of function %s",
          f->id.c_str());
      break;
  }
  f->pardef = FunSymbolEntry::PARDEF_COMPLETE;
}

SymbolEntry::~SymbolEntry() {}
FunSymbolEntry::~FunSymbolEntry() {
  ParSymbolEntry *args = this->firstArgument;
  while (args != NULL) {
      ParSymbolEntry * p = args;
      args = args->next;
      delete(p);
  }
}

SymbolEntry *SymbolTable::lookupEntry(std::string name, LookupType type, bool err, unsigned lineno) {
  unsigned int  hashValue = PJW_hash(name.c_str()) % hashTableSize;
  SymbolEntry * e         = hashTable[hashValue];
  
  switch (type) {
    case LOOKUP_CURRENT_SCOPE:
      while (e != NULL && e->nestingLevel == currentScope->nestingLevel)
        if (name.compare(e->id) == 0)
          return e;
        else
          e = e->nextHash;
      break;
    case LOOKUP_ALL_SCOPES:
      while (e != NULL)
        if (name.compare(e->id) == 0)
          return e;
        else
          e = e->nextHash;
      break;
  }
  
  if (err)
    Logger::error(lineno, "Unknown identifier: %s", name.c_str());
  return NULL;
}
