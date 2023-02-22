#include "CharPattern.hpp"

CharPattern::CharPattern(char c): var(c) {}

void CharPattern::printOn(std::ostream &out) const {
  out << "CharPattern(" << var << ")";
}

void CharPattern::sem() { type = charType; }
