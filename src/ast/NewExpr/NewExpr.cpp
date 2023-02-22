#include "NewExpr.hpp"

NewExpr::NewExpr(Type *t) { type = new RefType(t);}

NewExpr::~NewExpr() { delete type; }

void NewExpr::printOn(std::ostream &out) const {
  out << "New(" << *type << ")"; 
}

void NewExpr::sem() { /* Do Nothing */ }