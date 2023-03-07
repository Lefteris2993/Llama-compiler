#include "IdHighPrioExpr.hpp"
#include "../../symbol/symbol.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

IdHighPrioExpr::IdHighPrioExpr(std::string i, ExprBlock *c): id(i), commaExprList(c) {}

IdHighPrioExpr::~IdHighPrioExpr() { delete commaExprList; }

void IdHighPrioExpr::printOn(std::ostream &out) const {
  out << "IdHighPrioExpr(" << id;
  if (commaExprList != nullptr) out << "," << *commaExprList;
  out << ")";
}

void IdHighPrioExpr::sem() {
  SymbolEntry *s = symbolTable->lookupEntry<SymbolEntry>(id, true, lineno);

  if (commaExprList != nullptr) {
    if (s->type->getClassType() != TypeClassType::ARRAY )
      Logger::error(lineno, "\"%s\" is not an array", s->id.c_str());

    type = ((ArrayType *) s->type)->getType();
  } else {
    type = s->type;
  }
}

llvm::Value* IdHighPrioExpr::codegen() {
  LLVMSymbolEntry *v  = LLVMValueStore->lookupEntry<LLVMSymbolEntry>(id, true);

  return v->value;
}