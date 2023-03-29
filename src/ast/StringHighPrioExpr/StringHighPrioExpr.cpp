#include "StringHighPrioExpr.hpp"

StringHighPrioExpr::StringHighPrioExpr(std::string s) {
  s.erase(0, 1);
  s.erase(s.size() - 1);

  while (s.find("\\n")  != std::string::npos) s.replace(s.find("\\n"), 2, "\n");
  while (s.find("\\t")  != std::string::npos) s.replace(s.find("\\t"), 2, "\t");
  while (s.find("\\r")  != std::string::npos) s.replace(s.find("\\r"), 2, "\r");
  while (s.find("\\0")  != std::string::npos) s.replace(s.find("\\0"), 2, "\0");
  while (s.find("\\\\") != std::string::npos) s.replace(s.find("\\\\"), 2, "\\");
  while (s.find("\\'")  != std::string::npos) s.replace(s.find("\\'"), 2, "\'");
  while (s.find("\\\"") != std::string::npos) s.replace(s.find("\\\""), 2, "\"");

  val = s;
}

void StringHighPrioExpr::printOn(std::ostream &out) const {
  out << "StringHighPrioExpr(" << val << ")";
}

void StringHighPrioExpr::sem() { type = stringType; }

llvm::Value* StringHighPrioExpr::codegen() {
  llvm::Type *stringType = getLLVMType(new ArrayType(new SimpleType(BaseType::CHAR)))->getPointerElementType();

  auto stringVarMalloc = llvm::CallInst::CreateMalloc(
    Builder.GetInsertBlock(),
    llvm::Type::getIntNTy(TheContext, TheModule->getDataLayout().getMaxPointerSizeInBits()),
    stringType,
    llvm::ConstantExpr::getSizeOf(stringType),
    nullptr,
    nullptr,
    "array_struct_malloc"
  );
  llvm::Value *stringV = Builder.Insert(stringVarMalloc);


  auto arr = llvm::CallInst::CreateMalloc(
    Builder.GetInsertBlock(),
    llvm::Type::getIntNTy(TheContext, TheModule->getDataLayout().getMaxPointerSizeInBits()),
    i8,
    llvm::ConstantExpr::getSizeOf(i8),
    c32(val.length()),
    nullptr,
    "array_malloc"
  );

  Builder.Insert(arr);

  /* append 'metadata' of the array variable { ptr_to_arr, dimsNum, dim1, dim2, ..., dimn } */
  llvm::Value *arrayPtr = Builder.CreateGEP(stringType, stringV, { c32(0), c32(0) }, "string_literal");
  Builder.CreateStore(arr, arrayPtr);
  llvm::Value *arrayDims = Builder.CreateGEP(stringType, stringV, { c32(0), c32(1) }, "string_dim");
  Builder.CreateStore(c32(1), arrayDims);
  llvm::Value *dim = Builder.CreateGEP(stringType, stringV, { c32(0), c32(2) }, "dim_0");
  Builder.CreateStore(c32(val.length()), dim);

  /* add the string to the array */
  std::vector<llvm::Value *> args;
  args.push_back(Builder.CreateLoad(arrayPtr));
  llvm::Value *globalStr = getOrCreateGlobalString(val);
  args.push_back(globalStr);
  Builder.CreateCall(TheStringCopy, args);

  return stringV;
}