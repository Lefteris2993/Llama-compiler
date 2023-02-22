#include "UnitHighPrioExpr.hpp"

// extern Type *unitType;

void UnitHighPrioExpr::printOn(std::ostream &out) const {
  out << "UnitHighPrioExpr( unit )";
}

void UnitHighPrioExpr::sem() { type = unitType; }