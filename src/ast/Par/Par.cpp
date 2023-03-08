#include "Par.hpp"

extern SymbolTable *symbolTable;

Par::Par(std::string i, Type *t): id(i), type(t) {}

Par::~Par() { delete type; }

Type *Par::getType() { return type; }

void Par::printOn(std::ostream &out) const {
  out << "Par(" << id;
  if (type != nullptr) out << "," << *type;
  out << ")";
}

void Par::sem() {}

void Par::insertParam(FunSymbolEntry *f) {
  symbolTable->newParameter(id, type, f, lineno);
}

std::string Par::getId() {
  return id;
}

llvm::Type* Par::typeGen() {
  return getLLVMType(type);
}