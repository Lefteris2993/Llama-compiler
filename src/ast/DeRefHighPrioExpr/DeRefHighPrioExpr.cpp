#include "DeRefHighPrioExpr.hpp"
#include "../../error/error.hpp"

DeRefHighPrioExpr::DeRefHighPrioExpr(HighPrioExpr *e): expr(e) {}

DeRefHighPrioExpr::~DeRefHighPrioExpr() { delete expr; }

void DeRefHighPrioExpr::printOn(std::ostream &out) const {
  out << "DeRefHighPrioExpr(" << *expr << ")";
}

void DeRefHighPrioExpr::sem() {
  expr->sem();
  if (expr->getType() == nullptr)
    Logger::error(lineno, "Expresion has unkown type");
  if (expr->getType()->getClassType() != TypeClassType::REF)
    Logger::error(lineno, "Can not de-reference non REF type");

  type = ((RefType *) expr->getType())->getType();
}