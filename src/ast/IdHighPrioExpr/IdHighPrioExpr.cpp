#include "IdHighPrioExpr.hpp"
#include "../../symbol/symbol.hpp"
#include "../../error/error.hpp"

extern SymbolTable *symbolTable;

IdHighPrioExpr::IdHighPrioExpr(std::string i, ExprBlock *c): id(i), commaExprList(c) {}

IdHighPrioExpr::~IdHighPrioExpr() { delete commaExprList; }

void IdHighPrioExpr::printOn(std::ostream &out) const {
  out << "IdHighPrioExpr(" << id;
  if (commaExprList != nullptr) out << "," << *commaExprList;
  out << ")";
}

void IdHighPrioExpr::sem() {
  SymbolEntry *s = symbolTable->lookupEntry<SymbolEntry>(id, true, lineno);

  if (commaExprList != nullptr) {
    if (s->type->getClassType() != TypeClassType::ARRAY )
      Logger::error(lineno, "\"%s\" is not an array", s->id.c_str());

    if (commaExprList->getBlockSize() != ((ArrayType *) s->type)->getStars()) 
      Logger::error(lineno, "\"%s\": incorrect number of array dimension parameters", s->id.c_str());

    type = ((ArrayType *) s->type)->getType();
  } else {
    type = s->type;
  }
}

llvm::Value* IdHighPrioExpr::codegen() {
  LLVMSymbolEntry *s  = LLVMValueStore->lookupEntry<LLVMSymbolEntry>(id, true);
  
  if (commaExprList == nullptr) return s->value;

  llvm::Value *accessEl = nullptr;
  std::vector<llvm::Value *> dimValues = commaExprList->codegenValues();
  llvm::Value *accessSeValue = s->value;
  llvm::Value *mulTemp = c32(1);

  for (long unsigned int i = dimValues.size(); i > 0; i--) {
    if (i != dimValues.size()) {
      mulTemp = Builder.CreateMul(
        mulTemp,
        Builder.CreateLoad(
          Builder.CreateGEP(
            // (s->getType()->isPointerTy()) ? s->getType()->getPointerElementType() : s->getType(),
            accessSeValue->getType()->getPointerElementType(),
            accessSeValue,
            {c32(0), c32(i + 2)}
          )
        )
      );
      accessEl = Builder.CreateAdd(accessEl, Builder.CreateMul(mulTemp, dimValues.at(i - 1)));
    }
    else {
      accessEl = dimValues.at(i - 1);
    }
  }
  llvm::Value *arrPtr = Builder.CreateGEP(
    // (s->getType()->isPointerTy()) ? s->getType()->getPointerElementType() : s->getType(),
    accessSeValue->getType()->getPointerElementType(),
    accessSeValue,
    {c32(0), c32(0)}
  );
  arrPtr = Builder.CreateLoad(arrPtr, s->id);
  return Builder.CreateGEP(arrPtr, accessEl);
}