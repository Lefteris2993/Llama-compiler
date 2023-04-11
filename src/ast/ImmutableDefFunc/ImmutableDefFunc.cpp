#include "ImmutableDefFunc.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

ImmutableDefFunc::ImmutableDefFunc(
  std::string i,
  ParBlock *p,  
  Expr *e, 
  Type *t
): expr(e), block(p), type(t) {
  this->id = i;
}

ImmutableDefFunc::~ImmutableDefFunc() { delete expr; delete block; delete type; }

DefType ImmutableDefFunc::getDefType() const { return DefType::DEF_IMMUTABLE_FUN; }

void ImmutableDefFunc::printOn(std::ostream &out) const {
  out << "ImmutableDefFunc(" << id << "," << *expr << "," << *block;
  if (type != nullptr) out << "," << *type; 
  out << ")";
}

std::string ImmutableDefFunc::getId() const { return id; }

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

llvm::Function* ImmutableDefFunc::defCodeGen() {
  llvm::FunctionType *FT = 
    llvm::FunctionType::get(getLLVMType(type), block->getParams(), false);
  llvm::Function *functionInternal =
    llvm::Function::Create(FT, llvm::Function::InternalLinkage,
                  id, TheModule.get());
  return functionInternal;
}

void ImmutableDefFunc::funGen(llvm::Function *fi) {
  llvm::BasicBlock *ParentBB = Builder.GetInsertBlock();
  
  llvm::Function *functionInternal = fi;
  if (fi == nullptr) {
    LLVMSymbolEntry *v  = LLVMValueStore->lookupEntry<LLVMSymbolEntry>(id, true);
    functionInternal = (llvm::Function *) v->value;
  } 

  llvm::BasicBlock *functionBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", functionInternal);
  Builder.SetInsertPoint(functionBB);

  LLVMValueStore->openScope();

  // Set names for all arguments.
  unsigned Idx = 0;
  for (auto &Arg : functionInternal->args()) {
    std::string parId = block->block[Idx++]->getId();
    Arg.setName(parId);
    LLVMValueStore->newLLVMValue(parId, &Arg);
  }
  
  llvm::Value *retVal = expr->codegen();

  // TODO: Is this what we want?
  if (retVal == nullptr)
    Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  else 
    Builder.CreateRet(retVal);

  LLVMValueStore->closeScope();

  
  Builder.SetInsertPoint(ParentBB);
}

llvm::Value* ImmutableDefFunc::codegen() { return nullptr; }