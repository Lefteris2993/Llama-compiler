#include "MutableArrayDef.hpp"
#include "../../symbol/symbol.hpp"

extern SymbolTable *symbolTable;

MutableArrayDef::MutableArrayDef(
  std::string i, 
  ExprBlock *b, 
  Type *t
): id(i), block(b) {
  type = new ArrayType(t, b->getBlockSize());
}

MutableArrayDef::~MutableArrayDef() { delete block; delete type; }

DefType MutableArrayDef::getDefType() const { return DefType::DEF_ARRAY; }

void MutableArrayDef::printOn(std::ostream &out) const {
  out << "MutableArrayDef(" << id << "," << *block;
  if (type != nullptr) out << "," << *type;
  out << ")";
}

void MutableArrayDef::sem() {
  symbolTable->newVariable(id, type, lineno);
}

llvm::Value* MutableArrayDef::codegen() {
  llvm::Type *arrayType = getLLVMType(type)->getPointerElementType();

  auto arrayVarMalloc = llvm::CallInst::CreateMalloc(
      Builder.GetInsertBlock(),
      llvm::Type::getIntNTy(TheContext, TheModule->getDataLayout().getMaxPointerSizeInBits()),
      arrayType,
      llvm::ConstantExpr::getSizeOf(arrayType),
      nullptr,
      nullptr,
      "array_struct_malloc"
  );
  llvm::Value *arrayV = Builder.Insert(arrayVarMalloc);

  std::vector<llvm::Value *> values = block->codegenValues();

  llvm::Type* llvmType = getLLVMType(((RefType *)((ArrayType *) type)->getType())->getType());

  llvm::Value *mulTemp = c32(1);
  for (llvm::Value *val : values) {
    mulTemp = Builder.CreateMul(mulTemp, val);
  }

  auto arr = llvm::CallInst::CreateMalloc(
    Builder.GetInsertBlock(),
    llvm::Type::getIntNTy(TheContext, TheModule->getDataLayout().getMaxPointerSizeInBits()),
    llvmType,
    llvm::ConstantExpr::getSizeOf(llvmType),
    mulTemp,
    nullptr,
    "array_malloc"
  );

  Builder.Insert(arr);

  /* append 'metadata' of the array variable { ptr_to_arr, dimsNum, dim1, dim2, ..., dimn } */
  llvm::Value *arrayPtr = Builder.CreateGEP(arrayType, arrayV, { c32(0), c32(0) }, "arr_ptr");
  Builder.CreateStore(arr, arrayPtr);
  llvm::Value *dim = Builder.CreateGEP(arrayType, arrayV, { c32(0), c32(1) }, "arr_dim");
  Builder.CreateStore(c32(values.size()), dim);
  int count = 2;
  for (llvm::Value *val : values) {
    llvm::Value *arrayDims = Builder.CreateGEP(arrayType, arrayV, { c32(0), c32(count) }, "dim_0");
    Builder.CreateStore(val, arrayDims);
    count++;
  }

  LLVMValueStore->newLLVMValue(id, arrayV);

  return arrayV;
}
