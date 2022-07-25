#include "ast.hpp"

extern SymbolTable *symbolaTable;

void AST::sem() { std::cout << "not implemented\n"; } // TODO: change this to so all classes have to implement this.

void ExprBlock::sem() {
  for (Expr *c : block) {
    c->sem();
    if (!Type::equal_types(c->getType(), intType)) {
      Logger::error(c->lineno, "Array dimentions must be of type INT");
    }
  }
}

void NumPattern::sem() { type = intType; }

void CharPattern::sem() { type = charType; }

void BoolPattern::sem() { type = boolType; }

void VarPattern::sem() { /*TODO: */ }

void Clause::sem() {
  pattern->sem();
  expr->sem();
}

void ClauseBlock::sem() {
  for (Clause *c : clauses) c->sem();
  if (clauses.size() < 1) Logger::error(this->lineno, "Match expretion must have at least one clause");
  Type *patternType = clauses[0]->getPattern()->getType();
  Type *exprType = clauses[0]->getExpr()->getType();
  for (Clause *c : clauses) { 
    if (!Type::equal_types(c->getExpr()->getType(), exprType)) {
      Logger::error(c->lineno, "Match expretion clauses must return the same type");
    }  
  }
  for (Clause *c : clauses) {
    if (!Type::equal_types(c->getPattern()->getType(), patternType)) {
      Logger::error(c->lineno, "Match expretion patterns must be the same type");
    }
  }
}

void BinOpExpr::sem() {
  lhs->sem();
  rhs->sem();

  Type *l = lhs->getType();
  Type *r = rhs->getType();
  switch (op)
  {
  case BIN_PLUS:
  case BIN_MINUS:
  case STAR:
  case DIV:
  case MOD:
  case L:
  case G:
  case LE:
  case GE:
    if (
      !Type::equal_types(l, intType) 
      || !Type::equal_types(r, intType)
    ) {
      Logger::error(lineno, "Operands must be of type INT");
    }
    this->type = intType;
    break; 

  case STRUCT_EQ:
  case STRUCT_NE:
  case EQ:
  case NE:
    if (
      !Type::equal_types(l, r) ||
      (!Type::equal_types(l, boolType) && !Type::equal_types(l, intType))
    ) {
      Logger::error(lineno, "Operands must be of type INT or BOOL");
    }

    break;
  
  default:
    Logger::error(lineno, "Not implemented");
    break;
  }
}

void SigOpExpr::sem() { /*TODO: */ }

void DefBlock::sem() {
  for (Def *d : block) {
    d->sem();
    if (d->getDefType() == DefType::DEF_IMMUTABLE) ((ImmutableDef *) d)->decl();
  } 
}
void DefBlock::recSem() {
  for (Def *d : block) 
    d->sem();
  for (Def *d : block) 
    if (d->getDefType() == DefType::DEF_IMMUTABLE)
      ((ImmutableDef *) d)->decl();
}


void LetDef::sem() {
  if (rec) defBlock->recSem();
  else defBlock->sem(); 
}

void LetInExpr::sem() { /*TODO: */ }

void WhileExpr::sem() { /*TODO: */ }

void ForExpr::sem() { /*TODO: */ }

void DimExpr::sem() { /*TODO: */ }

void NewExpr::sem() { /*TODO: */ }

void DeleteExpr::sem() { /*TODO: */ }

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
      Logger::error(lineno, "\"then\" and \"else\" expretions have different types");
    }
  }

  this->type = expr1->getType();
}

void MatchExpr::sem() {
  expr->sem();
  block->sem();
}

void DeRefHighPrioExpr::sem() { /*TODO: */ }

void UnitHighPrioExpr::sem() { /*TODO: */ }

void IntHighPrioExpr::sem() {
  this->type = intType;
}

void CharHighPrioExpr::sem() {
  this->type = charType;
}

void StringHighPrioExpr::sem() { /*TODO: */ }

void BoolHighPrioExpr::sem() { type = boolType; }

void IdHighPrioExpr::sem() {
  SymbolEntry *s = symbolaTable->lookupEntry<SymbolEntry>(id, LOOKUP_CURRENT_SCOPE, false, lineno);
  type = s->type;
}

void HighPrioExprBlock::sem() { /*TODO: */ }

void StmtBlock::sem() {
  for (Stmt *s : stmt_list) s->sem();
}

void MutableDef::sem() {
  symbolaTable->newVariable(id, type, lineno);
}

void MutableArrayDef::sem() {
  symbolaTable->newVariable(id, type, lineno);
}

void Par::sem() {}
void Par::insertParam(FunSymbolEntry *f) {
  symbolaTable->newParameter(id, type, f, lineno);
}

void ParBlock::sem() {
  for (Par *p : block) p->sem();
}
void ParBlock::insertParams(FunSymbolEntry *f) {
  for (Par *p : block) p->insertParam(f);
}

void ImmutableDef::sem() {
  FunSymbolEntry *f =  symbolaTable->newFunction(id, lineno);
}
void ImmutableDef::decl() {
  FunSymbolEntry *f = symbolaTable->lookupEntry<FunSymbolEntry>(id, LOOKUP_CURRENT_SCOPE, false, lineno);
  symbolaTable->openScope();
  block->insertParams(f);

  expr->sem();
  if (type != nullptr) {
    bool sameTypes = Type::equal_types(type, expr->getType());
    if (!sameTypes) Logger::error(lineno, "Function does not return specified type");
  }
  if (type == nullptr) type = expr->getType();

  symbolaTable->endFunctionDef(f, type, lineno);
  symbolaTable->closeScope();
}

void FunctionCall::sem() {
  FunSymbolEntry *f = symbolaTable->lookupEntry<FunSymbolEntry>(id, LOOKUP_CURRENT_SCOPE, false, lineno);
  if (f->entryType != EntryType::ENTRY_FUNCTION) 
    Logger::error(lineno, "\"%s\" is not a function", f->id);

  // TODO:
  // sem expr list... check arguments if they are the same number and types...
}
