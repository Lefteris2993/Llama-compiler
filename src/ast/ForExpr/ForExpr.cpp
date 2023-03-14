#include "ForExpr.hpp"
#include "../../symbol/symbol.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

ForExpr::ForExpr(
  bool u, 
  std::string i, 
  Expr *q, 
  Expr *t, 
  Expr *b
): up(u), id(i), eq(q), to(t), body(b) {}

ForExpr::~ForExpr() { delete eq; delete to; delete body; }

void ForExpr::printOn(std::ostream &out) const {
  out << "ForExpr(" << (up ? "true" : "false") << "," << id << "," << *eq << "," << *to << "," << *body << ")";  
}

void ForExpr::sem() {
  symbolTable->openScope();
  symbolTable->newVariable(id, intType, lineno);
  to->sem();
  if (!Type::equal_types(to->getType(), intType))
    Logger::error(lineno, "For target must be of type INT");

  body->sem();
  type = unitType;
  symbolTable->closeScope();
}

llvm::Value* ForExpr::codegen() {
  llvm::Value *iVal = eq->codegen();

  LLVMValueStore->openScope();
  
  
  llvm::BasicBlock *PrevBB = Builder.GetInsertBlock();
  llvm::Function *TheFunction = PrevBB->getParent();
  llvm::BasicBlock *LoopBB =
    llvm::BasicBlock::Create(TheContext, "loop", TheFunction);
  llvm::BasicBlock *BodyBB =
    llvm::BasicBlock::Create(TheContext, "body", TheFunction);
  llvm::BasicBlock *AfterBB =
    llvm::BasicBlock::Create(TheContext, "endfor", TheFunction);
  
  Builder.CreateBr(LoopBB);
  Builder.SetInsertPoint(LoopBB);
  llvm::PHINode *phiIter = Builder.CreatePHI(i32, 2, id);
  phiIter->addIncoming(iVal, PrevBB);

  LLVMValueStore->newLLVMValue(id, phiIter);

  llvm::Value *toVar = to->codegen();
  
  llvm::Value *loopCond;
  if (up)
    loopCond = Builder.CreateICmpULT(phiIter, toVar, "loop_cond");
  else
    loopCond = Builder.CreateICmpSGT(phiIter, toVar, "loop_cond");
    
  Builder.CreateCondBr(loopCond, BodyBB, AfterBB);
  Builder.SetInsertPoint(BodyBB);

  llvm::Value *remaining;
  if (up)
    remaining = Builder.CreateAdd(phiIter, c32(1), "remaining");
  else 
    remaining = Builder.CreateSub(phiIter, c32(1), "remaining");
  body->codegen();
  phiIter->addIncoming(remaining, Builder.GetInsertBlock());
  Builder.CreateBr(LoopBB);
  Builder.SetInsertPoint(AfterBB);
  LLVMValueStore->closeScope();
  return llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit"));
}
