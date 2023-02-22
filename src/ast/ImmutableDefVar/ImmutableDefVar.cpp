#include "ImmutableDefVar.hpp"
#include "../../symbol/symbol.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

ImmutableDefVar::ImmutableDefVar(
  std::string i,
  Expr *e, 
  Type *t
): id(i), expr(e), type(t) {}

ImmutableDefVar::~ImmutableDefVar() { delete expr; delete type; }

DefType ImmutableDefVar::getDefType() const { return DefType::DEF_IMMUTABLE_VAR; }

void ImmutableDefVar::printOn(std::ostream &out) const {
  out << "ImmutableDefVar(" << id << "," << *expr;
  if (type != nullptr) out << "," << *type; 
  out << ")";
}

void ImmutableDefVar::sem() {
  VarSymbolEntry *v = symbolTable->newVariable(id, type, lineno);

  expr->sem();
  if (type != nullptr) {
    bool sameTypes = Type::equal_types(v->type, expr->getType());
    if (!sameTypes) Logger::error(lineno, "variable %s is not of specified type", id.c_str());    
  } else {
    v->type = type = expr->getType();
  }

}