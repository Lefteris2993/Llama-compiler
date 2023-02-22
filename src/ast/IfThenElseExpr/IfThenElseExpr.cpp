#include "IfThenElseExpr.hpp"
#include "../../error/error.hpp"

IfThenElseExpr::IfThenElseExpr(
  Expr *c, 
  Expr *e1, 
  Expr *e2
): cond(c), expr1(e1), expr2(e2) {}

IfThenElseExpr::~IfThenElseExpr() { delete cond; delete expr1; delete expr2; }

void IfThenElseExpr::printOn(std::ostream &out) const {
  out << "IfThenElse(" << *cond << "," << *expr1;
  if (expr2 != nullptr) out << "," << *expr2;
  out << ")";
}

void IfThenElseExpr::sem() {
  cond->sem();
  Type *condType = cond->getType();
  if (
    !Type::equal_types(condType, boolType) && 
    !Type::equal_types(condType, intType)
  ) {
    Logger::error(lineno, "If condition must be of type BOOL");
  }
  expr1->sem();
  if (expr2 != nullptr) {
    expr2->sem();

    Type *t1 = expr1->getType();
    Type *t2 = expr2->getType();

    if (!Type::equal_types(t1, t2)) {
      Logger::error(lineno, "\"then\" and \"else\" excretions have different types");
    }
  }

  this->type = expr1->getType();
}