#include "Clause.hpp"

Clause::Clause(Pattern *p, Expr *e): pattern(p), expr(e) {}

Clause::~Clause() { delete pattern; delete expr; }

void Clause::printOn(std::ostream &out) const {
  out << "Clause(" << *pattern << "," << *expr << ")";
}

Pattern *Clause::getPattern() { return pattern; }

Expr *Clause::getExpr() { return expr; }

void Clause::sem() {
  pattern->sem();
  expr->sem();
}
