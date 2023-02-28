#include "DefBlock.hpp"
#include "../ImmutableDefFunc/ImmutableDefFunc.hpp"

DefBlock::DefBlock(): block() {}

DefBlock::~DefBlock() { for (Def *b : block) delete b; }

void DefBlock::append(Def *d) { block.push_back(d); }

void DefBlock::printOn(std::ostream &out) const {
  out << "DefBlock(";
  bool first = true;
  for (Stmt *s : block) {
    if (!first) out << ",";
    first = false;
    out << *s;
  }
  out << ")";
}

void DefBlock::sem() {
  for (Def *d : block) {
    d->sem();
    if (d->getDefType() == DefType::DEF_IMMUTABLE_FUN) ((ImmutableDefFunc *) d)->decl();
  }
}

void DefBlock::recSem() {
  for (Def *d : block) 
    d->sem();
  for (Def *d : block) 
    if (d->getDefType() == DefType::DEF_IMMUTABLE_FUN)
      ((ImmutableDefFunc *) d)->decl();
}

llvm::Value* DefBlock::codegen() {
  for (Def *d : block) d->codegen();
  return nullptr;
}
