#include "AST.hpp"

#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/IR/Verifier.h>

llvm::LLVMContext AST::TheContext;
llvm::IRBuilder<> AST::Builder(TheContext);
std::unique_ptr<llvm::Module> AST::TheModule;
std::unique_ptr<llvm::legacy::FunctionPassManager> AST::TheFPM;

llvm::Type* AST::i1;
llvm::Type* AST::i8;
llvm::Type* AST::i32;
llvm::Type* AST::i64;

llvm::Function *AST::TheWriteString;
llvm::Function *AST::TheWriteInteger;
llvm::Function *AST::ThePrintIntInternal;

AST::~AST() {}

void AST::setLineno(unsigned l) { this->lineno = l; }

void AST::sem() { /* Do nothing */ }

llvm::Value* AST::codegen() { return nullptr; };

void AST::llvm_compile_and_dump(bool optimize) {
  TheModule = std::make_unique<llvm::Module>("Llama program", TheContext);
  TheFPM = std::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());
  if (optimize) {
    TheFPM->add(llvm::createPromoteMemoryToRegisterPass());
    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createReassociatePass());
    TheFPM->add(llvm::createGVNPass());
    TheFPM->add(llvm::createCFGSimplificationPass());
  }
  TheFPM->doInitialization();
  i1 = llvm::IntegerType::get(TheContext, 1);
  i8 = llvm::IntegerType::get(TheContext, 8);
  i32 = llvm::IntegerType::get(TheContext, 32);
  i64 = llvm::IntegerType::get(TheContext, 64);

  this->codegenLibs();

  // Defint and start the main function.
  llvm::FunctionType *main_type = llvm::FunctionType::get(i32, {}, false);
  llvm::Function *main = llvm::Function::Create(
    main_type,
    llvm::Function::ExternalLinkage,
    "main",
    TheModule.get()
  );
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", main);
  Builder.SetInsertPoint(BB);

  // Emit the program code.
  codegen();

  Builder.CreateRet(c32(0));

  // Verify the IR.
  bool bad = verifyModule(*TheModule, &llvm::errs());
  if (bad) {
    std::cerr << "The IR is bad!" << std::endl;
    TheModule->print(llvm::errs(), nullptr);
    std::exit(1);
  }
  // Optimize!
  TheFPM->run(*main);
  // Print out the IR.
  TheModule->print(llvm::outs(), nullptr);

}

llvm::ConstantInt* AST::c1(int c) {
  return llvm::ConstantInt::get(TheContext, llvm::APInt(1, c, true));
}
llvm::ConstantInt* AST::c8(char c) {
  return llvm::ConstantInt::get(TheContext, llvm::APInt(8, c, true));
}
llvm::ConstantInt* AST::c32(int n) {
  return llvm::ConstantInt::get(TheContext, llvm::APInt(32, n, true));
}
llvm::ConstantInt* AST::c64(int n) {
  return llvm::ConstantInt::get(TheContext, llvm::APInt(64, n, true));
}

void AST::codegenLibs() {
  /* create unit struct (type opaque -> no body) */
  std::string unitName = "unit";
  llvm::StructType *unitType = llvm::StructType::create(TheContext, unitName);
  std::vector<llvm::Type *> emptyBody;
  // emptyBody.push_back(i1);
  unitType->setBody(emptyBody);

  /* writeString - lib.a */
  llvm::FunctionType *writeString_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext),
                      {llvm::PointerType::get(i8, 0)}, false);
  TheWriteString =
    llvm::Function::Create(writeString_type, llvm::Function::ExternalLinkage,
                      "writeString", TheModule.get());

  /* writeInteger - lib.a */
  llvm::FunctionType *writeInteger_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), { i32 }, false);
  TheWriteInteger =
    llvm::Function::Create(writeInteger_type, llvm::Function::ExternalLinkage,
                  "writeInteger", TheModule.get());

    
  /* print_int */
  llvm::FunctionType *printInt_type = 
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { i32 }, false);
  ThePrintIntInternal =
    llvm::Function::Create(printInt_type, llvm::Function::InternalLinkage,
                  "print_int", TheModule.get());
  llvm::BasicBlock *ThePrintIntBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", ThePrintIntInternal);
  Builder.SetInsertPoint(ThePrintIntBB);
  Builder.CreateCall(TheWriteInteger, { ThePrintIntInternal->getArg(0) });
  Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  TheFPM->run(*ThePrintIntInternal);
}
