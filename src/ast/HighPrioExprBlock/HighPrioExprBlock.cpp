#include "HighPrioExprBlock.hpp"
#include "../../error/error.hpp"

HighPrioExprBlock::HighPrioExprBlock(): block() {}

HighPrioExprBlock::~HighPrioExprBlock() { for (HighPrioExpr *b : block) delete b; }

void HighPrioExprBlock::append(HighPrioExpr *d) { block.push_back(d); }

unsigned HighPrioExprBlock::getBlockLength() { return block.size(); } 

void HighPrioExprBlock::printOn(std::ostream &out) const {
  out << "HighPrioExprBlock(";
  bool first = true;
  for (HighPrioExpr *s : block) {
    if (!first) out << ",";
    first = false;
    out << *s;
  }
  out << ")";
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

std::vector<llvm::Value*> HighPrioExprBlock::getArgs() {
  std::vector<llvm::Value *> args;
  for (HighPrioExpr *e : block){
    args.push_back(e->codegen());
  }
  return args;
}