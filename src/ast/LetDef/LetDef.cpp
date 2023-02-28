#include "LetDef.hpp"

LetDef::LetDef(DefBlock *b, bool rec): defBlock(b), rec(rec) {}

LetDef::~LetDef() { delete defBlock; }

void LetDef::printOn(std::ostream &out) const {
  out << "LetDef";
  if (rec) out << "Rec";
  out << "(";
  out << *defBlock;
  out << ")";
}

void LetDef::sem() {
  if (rec) defBlock->recSem();
  else defBlock->sem(); 
}

llvm::Value* LetDef::codegen() {
  defBlock->codegen();
  return nullptr;
}