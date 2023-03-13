#include "BoolHighPrioExpr.hpp"

BoolHighPrioExpr::BoolHighPrioExpr(bool v): val(v) {}

void BoolHighPrioExpr::printOn(std::ostream &out) const {
  out << "BoolHighPrioExpr(" << (val ? "true" : "false") << ")";
}

void BoolHighPrioExpr::sem() { type = boolType; }

llvm::Value *BoolHighPrioExpr::codegen() {
  return c1(val);
}