#include "DeleteExpr.hpp"
#include "../../error/error.hpp"

DeleteExpr::DeleteExpr(Expr *e): expr(e) {}

DeleteExpr::~DeleteExpr() { delete expr; }

void DeleteExpr::printOn(std::ostream &out) const {
  out << "Delete(" << *expr << ")";
}

void DeleteExpr::sem() {
  expr->sem();
  if (expr->getType() == nullptr)
    Logger::error(lineno, "Expresion has unkown type");
  if (expr->getType()->getClassType() != TypeClassType::REF)
    Logger::error(lineno, "Can not delete non REF type");
  type = unitType;
}