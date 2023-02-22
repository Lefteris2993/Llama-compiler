#include "DimExpr.hpp"
#include "../../symbol/symbol.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

DimExpr::DimExpr(std::string i, unsigned c): id(i), dimension(c) {}

void DimExpr::printOn(std::ostream &out) const {
  out << "Dim(" << dimension << "," << id << ")";
}

void DimExpr::sem() {
  VarSymbolEntry *e = symbolTable->lookupEntry<VarSymbolEntry>(id, true, lineno);
  
  if (
    e->entryType == EntryType::ENTRY_FUNCTION &&
    ((FunSymbolEntry *) e)->paramNum != 0
  )
    Logger::error(lineno, "\"%s\" is function 2", e->id.c_str());

  if (e->type->getClassType() != TypeClassType::ARRAY)
    Logger::error(lineno, "Can not get \"dim\" of non array variable");

  if (dimension > ((ArrayType *) e->type)->getStars())
    Logger::error(lineno, "Array \"%s\" has only %d dimensions", id.c_str(), ((ArrayType *) e->type)->getStars());

  type = intType;
}