#include "CharHighPrioExpr.hpp"

CharHighPrioExpr::CharHighPrioExpr(char v): val(v) {}

void CharHighPrioExpr::printOn(std::ostream &out) const {
  out << "CharHighPrioExpr(" << val << ")";
}

void CharHighPrioExpr::sem() { type = charType; }

llvm::Value* CharHighPrioExpr::codegen() {
  return c8(val);
}