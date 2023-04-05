#include "CharPattern.hpp"

CharPattern::CharPattern(char c): var(c) {}

void CharPattern::printOn(std::ostream &out) const {
  out << "CharPattern(" << var << ")";
}

void CharPattern::sem() { type = charType; }

llvm::Value* CharPattern::codegen() {
  return c8(var);
}
