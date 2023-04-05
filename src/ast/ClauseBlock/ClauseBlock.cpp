#include "ClauseBlock.hpp"
#include "../../error/error.hpp"

ClauseBlock::ClauseBlock(): clauses() {}

ClauseBlock::~ClauseBlock() { for (Clause *c : clauses) delete c; }

void ClauseBlock::append(Clause *c) { clauses.push_back(c); }

void ClauseBlock::printOn(std::ostream &out) const {
  out << "ClauseBlock(";
  bool first = true;
  for (Clause *c : clauses) {
    if (!first) out << ",";
    first = false;
    out << *c;
  }
  out << ")";
}

void ClauseBlock::sem() {
  for (Clause *c : clauses) c->sem();
  if (clauses.size() < 1) Logger::error(this->lineno, "Match expresion must have at least one clause");
  Type *patternType = clauses[0]->getPattern()->getType();
  Type *exprType = clauses[0]->getExpr()->getType();
  for (Clause *c : clauses) { 
    if (!Type::equal_types(c->getExpr()->getType(), exprType)) {
      Logger::error(c->lineno, "Match expresion clauses must return the same type");
    }  
  }
  for (Clause *c : clauses) {
    if (!Type::equal_types(c->getPattern()->getType(), patternType)) {
      Logger::error(c->lineno, "Match expresion patterns must be the same type");
    }
  }
  clausesExprsType = exprType;
}

std::vector<Clause*> ClauseBlock::getClauses() { return clauses; }

Type *ClauseBlock::getClausesExprsType() { return clausesExprsType; }
