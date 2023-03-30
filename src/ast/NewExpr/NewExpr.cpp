#include "NewExpr.hpp"

NewExpr::NewExpr(Type *t) { type = new RefType(t);}

NewExpr::~NewExpr() { delete type; }

void NewExpr::printOn(std::ostream &out) const {
  out << "New(" << *type << ")"; 
}

void NewExpr::sem() { /* Do Nothing */ }

llvm::Value* NewExpr::codegen() {
  auto v = llvm::CallInst::CreateMalloc(
    Builder.GetInsertBlock(),
    llvm::Type::getIntNTy(TheContext, TheModule->getDataLayout().getMaxPointerSizeInBits()),
    getLLVMType(type)->getPointerElementType(),
    llvm::ConstantExpr::getSizeOf(getLLVMType(type)->getPointerElementType()),
    nullptr,
    nullptr,
    ""
  );

  return Builder.Insert(v);
}