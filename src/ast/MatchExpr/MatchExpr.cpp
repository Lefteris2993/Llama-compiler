#include "MatchExpr.hpp"

MatchExpr::MatchExpr(Expr *e, ClauseBlock *c): expr(e), block(c) {}

MatchExpr::~MatchExpr() { delete expr; delete block; }

void MatchExpr::printOn(std::ostream &out) const {
  out << "MatchExpr(" << *expr << "," << *block << ")";
}

void MatchExpr::sem() {
  expr->sem();
  block->sem();
  type = block->getClausesExprsType();
}

llvm::Value* MatchExpr::codegen() {
  llvm::Value *exprValue = expr->codegen();

  llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
  llvm::BasicBlock *AfterBB =
    llvm::BasicBlock::Create(TheContext, "end_match");

  std::vector<Clause*> clauses = block->getClauses();

  std::vector<llvm::Value *> clausesValues;
  std::vector<llvm::BasicBlock *> clausesBlocks;
  llvm::BasicBlock* nextBlock = 
    llvm::BasicBlock::Create(TheContext, "clause_condition");
  llvm::BasicBlock* valueBlock;

  Builder.CreateBr(nextBlock);

  for (Clause *c : clauses) {
    TheFunction->getBasicBlockList().push_back(nextBlock);
    Builder.SetInsertPoint(nextBlock);

    nextBlock = llvm::BasicBlock::Create(TheContext, "clause_condition");
    valueBlock = llvm::BasicBlock::Create(TheContext, "pattern_value");

    llvm::Value* clauseCondition = c->getPattern()->codegen();
    llvm::Value* compare = Builder.CreateICmpEQ(clauseCondition, exprValue);
    Builder.CreateCondBr(compare, valueBlock, nextBlock);

    TheFunction->getBasicBlockList().push_back(valueBlock);
    Builder.SetInsertPoint(valueBlock);
    llvm::Value* clauseValue = c->getExpr()->codegen();
    Builder.CreateBr(AfterBB);

    clausesValues.push_back(clauseValue);
    // use GetInsertBlock, because there maybe another nested block
    clausesBlocks.push_back(Builder.GetInsertBlock());
  }

  // nothing matched
  TheFunction->getBasicBlockList().push_back(nextBlock);
  Builder.SetInsertPoint(nextBlock);

  llvm::Module* module = Builder.GetInsertBlock()->getModule();
  llvm::Function* trapFunc = llvm::Intrinsic::getDeclaration(module, llvm::Intrinsic::trap);
  Builder.CreateCall(trapFunc, {});

  Builder.CreateBr(nextBlock);

  TheFunction->getBasicBlockList().push_back(AfterBB);
  Builder.SetInsertPoint(AfterBB);

  llvm::Type *retValType = clausesValues[0]->getType();
  llvm::PHINode *retVal = Builder.CreatePHI(retValType, clauses.size());
  for (long unsigned int i = 0; i < clauses.size(); i++) {
    retVal->addIncoming(clausesValues[i], clausesBlocks[i]);
  }

  return retVal;
}
