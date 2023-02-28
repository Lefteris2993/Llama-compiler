#include "StmtBlock.hpp"

StmtBlock::StmtBlock(): stmt_list() {}

StmtBlock::~StmtBlock() { for (Stmt *s : stmt_list) delete s; }

void StmtBlock::append(Stmt *s) { stmt_list.push_back(s); }

void StmtBlock::printOn(std::ostream &out) const {
  out << "StmtBlock(";
  bool first = true;
  for (Stmt *s : stmt_list) {
    if (!first) out << ",";
    first = false;
    out << *s;
  }
  out << ")";
}

void StmtBlock::sem() {
  for (Stmt *s : stmt_list) s->sem();
}

llvm::Value* StmtBlock::codegen() {
  for (Stmt *s : stmt_list) s->codegen();
  return nullptr;
}
