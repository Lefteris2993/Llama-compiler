#include "MatchExpr.hpp"

MatchExpr::MatchExpr(Expr *e, ClauseBlock *c): expr(e), block(c) {}

MatchExpr::~MatchExpr() { delete expr; delete block; }

void MatchExpr::printOn(std::ostream &out) const {
  out << "MatchExpr(" << *expr << "," << *block << ")";
}

void MatchExpr::sem() {
  expr->sem();
  block->sem();
}