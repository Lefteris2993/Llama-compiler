#include "WhileExpr.hpp"
#include "../../error/error.hpp"

WhileExpr::WhileExpr(Expr *c, Expr *b): cond(c), body(b) {}

WhileExpr::~WhileExpr() { delete cond; delete body; }

void WhileExpr::printOn(std::ostream &out) const {
  out << "While(" << *cond << "," << *body << ")";
}

void WhileExpr::sem() {
  cond->sem();

  if (
    !Type::equal_types(cond->getType(), boolType) &&
    !Type::equal_types(cond->getType(), intType)
  ) {
    Logger::error(lineno, "While condition must be of type BOOl or INT");
  }

  body->sem();

  type = unitType;
}