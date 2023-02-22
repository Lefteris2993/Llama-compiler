#include "MutableArrayDef.hpp"
#include "../../symbol/symbol.hpp"

extern SymbolTable *symbolTable;

MutableArrayDef::MutableArrayDef(
  std::string i, 
  ExprBlock *b, 
  Type *t
): id(i), block(b), type(t) {}

MutableArrayDef::~MutableArrayDef() { delete block; delete type; }

DefType MutableArrayDef::getDefType() const { return DefType::DEF_ARRAY; }

void MutableArrayDef::printOn(std::ostream &out) const {
  out << "MutableArrayDef(" << id << "," << *block;
  if (type != nullptr) out << "," << *type;
  out << ")";
}

void MutableArrayDef::sem() {
  symbolTable->newVariable(id, type, lineno);
}