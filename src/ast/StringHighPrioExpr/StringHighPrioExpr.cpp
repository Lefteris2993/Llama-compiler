#include "StringHighPrioExpr.hpp"

StringHighPrioExpr::StringHighPrioExpr(std::string s): val(s) {}

void StringHighPrioExpr::printOn(std::ostream &out) const {
  out << "StringHighPrioExpr(" << val << ")";
}

void StringHighPrioExpr::sem() { type = stringType; }
