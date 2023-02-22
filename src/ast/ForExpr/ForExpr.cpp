#include "ForExpr.hpp"
#include "../../symbol/symbol.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

ForExpr::ForExpr(
  bool u, 
  std::string i, 
  Expr *q, 
  Expr *t, 
  Expr *b
): up(u), id(i), eq(q), to(t), body(b) {}

ForExpr::~ForExpr() { delete eq; delete to; delete body; }

void ForExpr::printOn(std::ostream &out) const {
  out << "For(" << (up ? "true" : "false") << "," << id << "," << *eq << "," << *to << "," << *body << ")";  
}

void ForExpr::sem() {
  symbolTable->openScope();
  symbolTable->newVariable(id, intType, lineno);
  to->sem();
  if (!Type::equal_types(to->getType(), intType))
    Logger::error(lineno, "For target must be of type INT");

  body->sem();
  type = unitType;
  symbolTable->closeScope();
}
