#include "IfThenElseExpr.hpp"
#include "../../error/error.hpp"

IfThenElseExpr::IfThenElseExpr(
  Expr *c, 
  Expr *e1, 
  Expr *e2
): cond(c), expr1(e1), expr2(e2) {}

IfThenElseExpr::~IfThenElseExpr() { delete cond; delete expr1; delete expr2; }

void IfThenElseExpr::printOn(std::ostream &out) const {
  out << "IfThenElseExpr(" << *cond << "," << *expr1;
  if (expr2 != nullptr) out << "," << *expr2;
  out << ")";
}

void IfThenElseExpr::sem() {
  cond->sem();
  Type *condType = cond->getType();
  if (
    !Type::equal_types(condType, boolType) && 
    !Type::equal_types(condType, intType)
  ) {
    Logger::error(lineno, "If condition must be of type BOOL");
  }
  expr1->sem();
  if (expr2 != nullptr) {
    expr2->sem();

    Type *t1 = expr1->getType();
    Type *t2 = expr2->getType();

    if (!Type::equal_types(t1, t2)) {
      Logger::error(lineno, "\"then\" and \"else\" excretions have different types");
    }
  }

  this->type = expr1->getType();
}

llvm::Value* IfThenElseExpr::codegen() {
  llvm::Value *condValue = cond->codegen();
  llvm::Value *cond = Builder.CreateICmpNE(condValue, c1(false), "if_cond");

  llvm::Value *thenValue;
  llvm::Value *elseValue; 

  llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();
  llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(TheContext, "then", TheFunction);
  llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(TheContext, "else", TheFunction);
  llvm::BasicBlock *AfterBB = llvm::BasicBlock::Create(TheContext, "endif", TheFunction);

  Builder.CreateCondBr(cond, ThenBB, ElseBB);
  
  Builder.SetInsertPoint(ThenBB);
  thenValue = expr1->codegen();
  ThenBB = Builder.GetInsertBlock();
  Builder.CreateBr(AfterBB);

  Builder.SetInsertPoint(ElseBB);
  if (expr2 != nullptr)
    elseValue = expr2->codegen();
  else 
    elseValue = llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit"));
  Builder.CreateBr(AfterBB);

  Builder.SetInsertPoint(AfterBB);

  llvm::PHINode *phiNodeRet = Builder.CreatePHI(thenValue->getType(), 2, "if_ret_val");
  phiNodeRet->addIncoming(thenValue, ThenBB);
  phiNodeRet->addIncoming(elseValue, ElseBB);
  return phiNodeRet;
}