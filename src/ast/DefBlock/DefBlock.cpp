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

llvm::Value* DefBlock::recCodegen() {
  std::vector<llvm::Value *> vals;
  std::vector<std::string> ids;

  for (Def *d : block) {
    if (d->getDefType() == DefType::DEF_IMMUTABLE_FUN) {
      vals.push_back(((ImmutableDefFunc *) d)->defCodeGen());
      ids.push_back(d->getId());
    }
  }
    
  for (unsigned long long i = 0; i < vals.size(); i++) {
    LLVMSymbolEntry *entry  = LLVMValueStore->lookupEntry<LLVMSymbolEntry>(ids[i], false);
    if (entry == nullptr)
      LLVMValueStore->newLLVMValue(ids[i], vals[i]);
    else 
      entry->value = vals[i];
  }

  vals.clear();
  ids.clear();

  for (Def *d : block) {
    if (d->getDefType() == DefType::DEF_IMMUTABLE_FUN) {
      ((ImmutableDefFunc *) d)->funGen();
    } else {
      vals.push_back(d->codegen());
      ids.push_back(d->getId());
    }
  }

  for (unsigned long long i = 0; i < vals.size(); i++) {
    LLVMSymbolEntry *entry  = LLVMValueStore->lookupEntry<LLVMSymbolEntry>(ids[i], false);
    if (entry == nullptr)
      LLVMValueStore->newLLVMValue(ids[i], vals[i]);
    else 
      entry->value = vals[i];
  }
    
  return nullptr;
}

llvm::Value* DefBlock::codegen() {
  std::vector<llvm::Value *> vals;
  std::vector<std::string> ids;

  for (Def *d : block) {
    if (d->getDefType() == DefType::DEF_IMMUTABLE_FUN) {
      llvm::Function *functionInternal = ((ImmutableDefFunc *) d)->defCodeGen();
      vals.push_back(functionInternal);
      ids.push_back(d->getId());
      ((ImmutableDefFunc *) d)->funGen(functionInternal);
    } else {
      vals.push_back(d->codegen());
      ids.push_back(d->getId());
    }
  }

  for (unsigned long long i = 0; i < vals.size(); i++) {
    LLVMSymbolEntry *entry  = LLVMValueStore->lookupEntry<LLVMSymbolEntry>(ids[i], false);
    if (entry == nullptr)
      LLVMValueStore->newLLVMValue(ids[i], vals[i]);
    else 
      entry->value = vals[i];
  }

  return nullptr;
}
