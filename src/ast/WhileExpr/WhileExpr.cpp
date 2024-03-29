#include "WhileExpr.hpp"
#include "../../error/error.hpp"

WhileExpr::WhileExpr(Expr *c, Expr *b): cond(c), body(b) {}

WhileExpr::~WhileExpr() { delete cond; delete body; }

void WhileExpr::printOn(std::ostream &out) const {
  out << "While(" << *cond << "," << *body << ")";
}

void WhileExpr::sem() {
  cond->sem();

  if (
    !Type::equal_types(cond->getType(), boolType) &&
    !Type::equal_types(cond->getType(), intType)
  ) {
    Logger::error(lineno, "While condition must be of type BOOl or INT");
  }

  body->sem();

  type = unitType;
}

llvm::Value* WhileExpr::codegen() {
  llvm::BasicBlock *PrevBB = Builder.GetInsertBlock();
  llvm::Function *TheFunction = PrevBB->getParent();
  llvm::BasicBlock *whileBB =
    llvm::BasicBlock::Create(TheContext, "while", TheFunction);
  llvm::BasicBlock *BodyBB =
    llvm::BasicBlock::Create(TheContext, "body", TheFunction);
  llvm::BasicBlock *AfterBB =
    llvm::BasicBlock::Create(TheContext, "end_while", TheFunction);

  Builder.CreateBr(whileBB);
  Builder.SetInsertPoint(whileBB);
  llvm::Value *condition = cond->codegen();
  Builder.CreateCondBr(condition, BodyBB, AfterBB);

  Builder.SetInsertPoint(BodyBB);
  body->codegen();
  Builder.CreateBr(whileBB);

  Builder.SetInsertPoint(AfterBB);
  return llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit"));
}