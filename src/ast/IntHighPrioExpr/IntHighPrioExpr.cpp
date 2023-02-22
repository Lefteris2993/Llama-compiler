#include "IntHighPrioExpr.hpp"

IntHighPrioExpr::IntHighPrioExpr(int v): val(v) {}

void IntHighPrioExpr::printOn(std::ostream &out) const {
  out << "IntHighPrioExpr(" << val << ")";
}

void IntHighPrioExpr::sem() { type = intType; }

