#include "BoolPattern.hpp"

BoolPattern::BoolPattern(bool v): val(v) {}

void BoolPattern::printOn(std::ostream &out) const {
  out << "BoolPattern(" << (val ? "true" : "false") << ")";
}

void BoolPattern::sem() { type = boolType; }

llvm::Value* BoolPattern::codegen() {
  return c1(val);
}
