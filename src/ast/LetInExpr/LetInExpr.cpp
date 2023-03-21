#include "LetInExpr.hpp"

LetInExpr::LetInExpr(LetDef *l, Expr *e): let(l), expr(e) {}

LetInExpr::~LetInExpr() { delete let; delete expr; }

void LetInExpr::printOn(std::ostream &out) const {
  out << "LetInExpr(" << *let << "," << *expr << ")";
}

void LetInExpr::sem() {
  let->sem();
  expr->sem();

  type = expr->getType();
}

llvm::Value* LetInExpr::codegen() {
  let->codegen();
  expr->codegen();
  return nullptr;
}