#include "NumPattern.hpp"

NumPattern::NumPattern(int n, bool neg): num(n), negative(neg) {}

void NumPattern::printOn(std::ostream &out) const {
  out << "NumPattern(" << (negative ? "-" : "+") << "," << num << ")";
}

void NumPattern::sem() { type = intType; }
