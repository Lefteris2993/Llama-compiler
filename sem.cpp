#include "ast.hpp"

extern SymbolTable *symbolTable;

void AST::sem() { std::cout << "not implemented\n"; } // TODO: change this to so all classes have to implement this.

void ExprBlock::sem() {
  for (Expr *c : block) {
    c->sem();
    if (!Type::equal_types(c->getType(), intType)) {
      Logger::error(c->lineno, "Array dimensions must be of type INT");
    }
  }
}

void NumPattern::sem() { type = intType; }

void CharPattern::sem() { type = charType; }

void BoolPattern::sem() { type = boolType; }

void VarPattern::sem() {
  SymbolEntry *s = symbolTable->lookupEntry<SymbolEntry>(var, LOOKUP_ALL_SCOPES, true, lineno);
  type = s->type;
}

void Clause::sem() {
  pattern->sem();
  expr->sem();
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
}

void BinOpExpr::sem() {
  // TODO: complete for all operators
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
    type = intType;
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
    type = boolType;

    break;
  
  default:
    Logger::error(lineno, "Not implemented");
    break;
  }
}

void SigOpExpr::sem() {
  expr->sem();
  switch (op)
  {
  case SigOp::NOT:
    if (!Type::equal_types(expr->getType(), boolType)) 
      Logger::error(lineno, "Invalid argument for \"not\" expresion");
    type = boolType;
    break;

  case SigOp::SIG_MINUS:
  case SigOp::SIG_PLUS:
    if (!Type::equal_types(expr->getType(), intType))
      Logger::error(lineno, "Invalid argument for sign expresion");
    type = intType;
    break;

  default:
    Logger::error(lineno, "Not implemented");
    break;
  }
}

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
      Logger::error(lineno, "\"then\" and \"else\" excretions have different types");
    }
  }

  this->type = expr1->getType();
}

void MatchExpr::sem() {
  expr->sem();
  block->sem();
}

void DeRefHighPrioExpr::sem() { /*TODO: */ }

void UnitHighPrioExpr::sem() { type = unitType; }

void IntHighPrioExpr::sem() { type = intType; }

void CharHighPrioExpr::sem() { type = charType; }

void StringHighPrioExpr::sem() { type = stringType; }

void BoolHighPrioExpr::sem() { type = boolType; }

void IdHighPrioExpr::sem() {
  SymbolEntry *s = symbolTable->lookupEntry<SymbolEntry>(id, LOOKUP_ALL_SCOPES, true, lineno);
  type = s->type;
}

void HighPrioExprBlock::sem() {
  for (HighPrioExpr *e : block) e->sem();
}
void HighPrioExprBlock::parCheck(FunSymbolEntry *f) {
  for (unsigned i = 0; i < block.size(); i++) {
    if (!Type::equal_types(f->paramTypes[i], block[i]->getType()))
      Logger::error(lineno, "Argument in position %d is NOT of correct type in function \"%s\"", i, f->id.c_str());
  }
}

void StmtBlock::sem() {
  for (Stmt *s : stmt_list) s->sem();
}

void MutableDef::sem() {
  symbolTable->newVariable(id, type, lineno);
}

void MutableArrayDef::sem() {
  symbolTable->newVariable(id, type, lineno);
}

void Par::sem() {}
void Par::insertParam(FunSymbolEntry *f) {
  symbolTable->newParameter(id, type, f, lineno);
}

void ParBlock::sem() {
  for (Par *p : block) p->sem();
}
void ParBlock::insertParams(FunSymbolEntry *f) {
  for (Par *p : block) p->insertParam(f);
}

void ImmutableDef::sem() {
  FunSymbolEntry *f = symbolTable->newFunction(id, type, lineno);
  f->paramNum = block->block.size();
  f->paramTypes = new Type *[block->block.size()];
  for (unsigned i = 0; i < block->block.size(); i++) {
    f->paramTypes[i] = block->block[i]->getType();
  }
}
void ImmutableDef::decl() {
  FunSymbolEntry *f = symbolTable->lookupEntry<FunSymbolEntry>(id, LOOKUP_CURRENT_SCOPE, false, lineno);
  symbolTable->openScope();
  block->insertParams(f);

  expr->sem();
  if (type != nullptr) {
    bool sameTypes = Type::equal_types(type, expr->getType());
    if (!sameTypes) Logger::error(lineno, "Function does not return specified type");
  }
  if (type == nullptr) type = expr->getType();

  symbolTable->endFunctionDef(f, type, lineno);
  symbolTable->closeScope();
}

void FunctionCall::sem() {
  FunSymbolEntry *f = symbolTable->lookupEntry<FunSymbolEntry>(id, LOOKUP_ALL_SCOPES, true, lineno);
  if (f->entryType != EntryType::ENTRY_FUNCTION) 
    Logger::error(lineno, "\"%s\" is not a function", f->id.c_str());

  if (f->paramNum != block->getBlockLength())
    Logger::error(lineno, "Incorrect number of parameters passed in function :%s", id.c_str());

  block->sem();
  block->parCheck(f);

  type = f->type;
}
