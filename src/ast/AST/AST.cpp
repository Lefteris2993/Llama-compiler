#include "AST.hpp"
#include "../../types/types.hpp"

#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/IR/Verifier.h>

std::unique_ptr<SymbolTable> AST::LLVMValueStore;

llvm::LLVMContext AST::TheContext;
llvm::IRBuilder<> AST::Builder(TheContext);
std::unique_ptr<llvm::Module> AST::TheModule;
std::unique_ptr<llvm::legacy::FunctionPassManager> AST::TheFPM;

std::map<std::string, llvm::Value *> AST::declaredGlobalStrs;


llvm::Type* AST::i1;
llvm::Type* AST::i8;
llvm::Type* AST::i32;
llvm::Type* AST::i64;

llvm::Function *AST::TheWriteString;
llvm::Function *AST::ThePrintStringInternal;
llvm::Function *AST::TheWriteInteger;
llvm::Function *AST::ThePrintIntInternal;
llvm::Function *AST::TheStringCopy;

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

  LLVMValueStore = std::make_unique<SymbolTable>(42);
  LLVMValueStore->openScope();

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
  LLVMValueStore->openScope();
  codegen();

  LLVMValueStore->closeScope();
  LLVMValueStore->closeScope();

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


llvm::Type* AST::getLLVMType(Type* t) {
  if (t == nullptr) {
    return TheModule->getTypeByName("unit");
  }
  
  TypeClassType typeClass = t->getClassType();
  switch (typeClass) {
    case TypeClassType::SIMPLE: {
      SimpleType *simpleType = (SimpleType *) t;
      switch (simpleType->getType()) {
        case BaseType::UNIT:
          return TheModule->getTypeByName("unit");
        case BaseType::INT:
          return i32;
        case BaseType::CHAR:
          return i8;
        case BaseType::BOOL:
          return i1;
        default:
          return nullptr;
      }
    }
    case TypeClassType::FUNCTION:
    case TypeClassType::ARRAY:
      return nullptr;
    case TypeClassType::REF: 
      return getLLVMType(((RefType *)t)->getType());
    default:
      return nullptr;
  }
}

void AST::codegenLibs() {
  /* create unit struct (type opaque -> no body) */
  std::string unitName = "unit";
  llvm::StructType *unitType = llvm::StructType::create(TheContext, unitName);
  std::vector<llvm::Type *> emptyBody;
  unitType->setBody(emptyBody);

  /* create string struct type */
  std::vector<llvm::Type *> members;
  members.push_back(llvm::PointerType::getUnqual(i8));
  /* dimensions number of array */
  members.push_back(i32);
  /* string is defined as an array of one dim */
  members.push_back(i32);
  std::string arrName = "stringType";
  llvm::StructType *arrayStruct = llvm::StructType::create(TheContext, arrName);
  arrayStruct->setBody(members);

  /* writeString - lib.a */
  llvm::FunctionType *writeString_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext),
                      {llvm::PointerType::get(i8, 0)}, false);
  TheWriteString =
    llvm::Function::Create(writeString_type, llvm::Function::ExternalLinkage,
                      "writeString", TheModule.get());

  /* print_string */
  llvm::FunctionType *printString_type = 
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { TheModule->getTypeByName(arrName)->getPointerTo() }, false);
  ThePrintStringInternal =
    llvm::Function::Create(printString_type, llvm::Function::InternalLinkage,
                  "print_string", TheModule.get());
  llvm::BasicBlock *ThePrintStringBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", ThePrintStringInternal);
  Builder.SetInsertPoint(ThePrintStringBB);
  llvm::Value *printstr_strPtr = Builder.CreateLoad(Builder.CreateGEP(TheModule->getTypeByName(arrName), ThePrintStringInternal->getArg(0), { c32(0), c32(0) }, "stringPtr"));
  Builder.CreateCall(TheWriteString, { printstr_strPtr });
  Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  TheFPM->run(*ThePrintStringInternal);
  LLVMValueStore->newLLVMValue("print_string", ThePrintStringInternal);

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
  LLVMValueStore->newLLVMValue("print_int", ThePrintIntInternal);

  llvm::FunctionType *stringCopy_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), { llvm::PointerType::get(i8, 0), llvm::PointerType::get(i8, 0) }, false);
  TheStringCopy =
    llvm::Function::Create(stringCopy_type, llvm::Function::ExternalLinkage,
                  "strcpy", TheModule.get());
}

/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
/// the function.  This is used for mutable variables etc.
llvm::AllocaInst *AST::CreateEntryBlockAlloca(llvm::Function *TheFunction, const std::string &VarName, llvm::Type *type) {
  llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(type, nullptr, VarName);
}

llvm::Value *AST::getOrCreateGlobalString(std::string stringLiteral) {
    if (declaredGlobalStrs.find(stringLiteral) != declaredGlobalStrs.end()) return declaredGlobalStrs[stringLiteral];
    else {
        declaredGlobalStrs[stringLiteral] = Builder.CreateGlobalStringPtr(llvm::StringRef(stringLiteral));
        return declaredGlobalStrs[stringLiteral];
    }
}
