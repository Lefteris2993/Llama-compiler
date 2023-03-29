#include "ExprBlock.hpp"
#include "../../error/error.hpp"

ExprBlock::ExprBlock(): block() {}

ExprBlock::~ExprBlock() { for (Expr *b : block) delete b; }

void ExprBlock::append(Expr *c) { block.push_back(c); }

void ExprBlock::printOn(std::ostream &out) const {
  out << "ExprBlock(";
  bool first = true;
  for (Expr *c : block) {
    if (!first) out << ",";
    first = false;
    out << *c;
  }
  out << ")";
}

void ExprBlock::sem() {
  for (Expr *c : block) {
    c->sem();
    if (!Type::equal_types(c->getType(), intType)) {
      Logger::error(c->lineno, "Array dimensions must be of type INT");
    }
  }
}

std::vector<llvm::Value *> ExprBlock::codegenValues() {
  std::vector<llvm::Value *> values;
  for (Expr *e : block){
    values.push_back(e->codegen());
  }
  return values;
}