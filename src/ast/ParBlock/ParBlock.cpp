#include "ParBlock.hpp"

ParBlock::ParBlock(): block() {}

ParBlock::~ParBlock() { for (Par *b : block) delete b; }

void ParBlock::append(Par *p) { block.push_back(p); }

void ParBlock::printOn(std::ostream &out) const {
  out << "ParBlock(";
  bool first = true;
  for (Par *p : block) {
    if (!first) out << ",";
    first = false;
    out << *p;
  }
  out << ")";
}

void ParBlock::sem() {
  for (Par *p : block) p->sem();
}

void ParBlock::insertParams(FunSymbolEntry *f) {
  for (Par *p : block) p->insertParam(f);
}

std::vector<llvm::Type*> ParBlock::getParams() {
  std::vector<llvm::Type *> params;
  for (Par *p : block){
    params.push_back(p->typeGen());
  }
  return params;
}