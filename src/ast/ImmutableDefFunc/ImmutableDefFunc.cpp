#include "ImmutableDefFunc.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

ImmutableDefFunc::ImmutableDefFunc(
  std::string i,
  ParBlock *p,  
  Expr *e, 
  Type *t
): id(i), expr(e), block(p), type(t) {}

ImmutableDefFunc::~ImmutableDefFunc() { delete expr; delete block; delete type; }

DefType ImmutableDefFunc::getDefType() const { return DefType::DEF_IMMUTABLE_FUN; }

void ImmutableDefFunc::printOn(std::ostream &out) const {
  out << "ImmutableDefFunc(" << id << "," << *expr << "," << *block;
  if (type != nullptr) out << "," << *type; 
  out << ")";
}

void ImmutableDefFunc::sem() {
  const int blockSize = block->block.size();

  // create function type
  Type *prevType = type;
  FunctionType *funcType = nullptr;
  for (int i = blockSize - 1; i >= 0; i--) {
    Type *currType = block->block[i]->getType();
    funcType = new FunctionType(currType, prevType);
    prevType = funcType;
  }

  // create symbol entry
  FunSymbolEntry *f = symbolTable->newFunction(id, funcType == nullptr ? type : funcType, lineno);
  f->paramNum = blockSize;
  f->returnType = type;
  f->paramTypes = new Type *[blockSize];
  for (int i = 0; i < blockSize; i++) {
    f->paramTypes[i] = block->block[i]->getType();
  }
}

void ImmutableDefFunc::decl() {
  FunSymbolEntry *f = symbolTable->lookupEntry<FunSymbolEntry>(id, false, lineno);
  symbolTable->openScope();
  block->insertParams(f);

  expr->sem();
  if (type != nullptr) {
    bool sameTypes = Type::equal_types(f->returnType, expr->getType());
    if (!sameTypes) Logger::error(lineno, "Function does not return specified type");
  }
  if (type == nullptr) type = expr->getType();

  symbolTable->endFunctionDef(f, type, lineno);
  symbolTable->closeScope();
}