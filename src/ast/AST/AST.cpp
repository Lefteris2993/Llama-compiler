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
llvm::Function *AST::TheWriteChar;
llvm::Function *AST::ThePrintCharInternal;
llvm::Function *AST::TheWriteBoolean;
llvm::Function *AST::ThePrintBoolInternal;

llvm::Function *AST::TheReadInteger;
llvm::Function *AST::TheReadIntInternal;
llvm::Function *AST::TheReadBoolean;
llvm::Function *AST::TheReadBoolInternal;
llvm::Function *AST::TheReadChar;
llvm::Function *AST::TheReadCharInternal;
llvm::Function *AST::TheReadString;
llvm::Function *AST::TheReadStringInternal;

llvm::Function *AST::TheAbs;
llvm::Function *AST::TheIncr;
llvm::Function *AST::TheDecr;

llvm::Function *AST::TheOrd;
llvm::Function *AST::TheIntOfChar;
llvm::Function *AST::TheChr;
llvm::Function *AST::TheCharOfInt;

llvm::Function *AST::TheStringLength;
llvm::Function *AST::TheStringLengthInternal;
llvm::Function *AST::TheStringCompare;
llvm::Function *AST::TheStringCompareInternal;
llvm::Function *AST::TheStringCopy;
llvm::Function *AST::TheStringCopyInternal;
llvm::Function *AST::TheStringConcat;
llvm::Function *AST::TheStringConcatInternal;

AST::~AST() {}

void AST::setLineno(unsigned l) { this->lineno = l; }

void AST::sem() { /* Do nothing */ }

llvm::Value* AST::codegen() { return nullptr; };

void AST::llvm_compile_and_dump(bool optimize) {
  TheModule = std::make_unique<llvm::Module>("Llama program", TheContext);
  TheFPM = std::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());
  if (optimize) {
    TheFPM->add(llvm::createPromoteMemoryToRegisterPass());
    TheFPM->add(llvm::createMemCpyOptPass());
    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createDeadCodeEliminationPass());
    TheFPM->add(llvm::createReassociatePass());
    TheFPM->add(llvm::createGVNPass());
    TheFPM->add(llvm::createCFGSimplificationPass());
    // TheFPM->add(llvm::createDeadArgEliminationPass());
    // TheFPM->add(llvm::createConstantMergePass());
    TheFPM->add(llvm::createCorrelatedValuePropagationPass());
    TheFPM->add(llvm::createDeadCodeEliminationPass());
    // TheFPM->add(llvm::createGlobalDCEPass());
    // TheFPM->add(llvm::createGlobalOptimizerPass());
    TheFPM->add(llvm::createGVNPass());
    TheFPM->add(llvm::createIndVarSimplifyPass());
    TheFPM->add(llvm::createInstructionCombiningPass());
    TheFPM->add(llvm::createJumpThreadingPass());
    // TheFPM->add(llvm::createFunctionInliningPass());
    TheFPM->add(llvm::createLICMPass());
    TheFPM->add(llvm::createLoopDeletionPass());
    TheFPM->add(llvm::createLoopIdiomPass());
    TheFPM->add(llvm::createLoopUnrollPass());
    TheFPM->add(llvm::createLoopUnswitchPass());
    TheFPM->add(llvm::createSCCPPass());
    // TheFPM->add(llvm::createGlobalDCEPass());
    // TheFPM->add(llvm::createStripDeadPrototypesPass());
    TheFPM->add(llvm::createTailCallEliminationPass());
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
      return nullptr;
    case TypeClassType::ARRAY: {
      ArrayType *arrayType = (ArrayType *) t;
      std::string typeName = arrayType->typeName();

      if (TheModule->getTypeByName(typeName) != nullptr) {
        return TheModule->getTypeByName(typeName)->getPointerTo();
      }

      /* create array */
      std::vector<llvm::Type *> members;
      /* ptr to array */
      members.push_back(llvm::PointerType::getUnqual(
        // getPointerElementType is used here since array elem will always of type ref
        getLLVMType(arrayType->getType())->getPointerElementType()
      ));
      /* dimensions number of array */
      members.push_back(i32);

      for (int i = 0; i < arrayType->getStars(); i++) members.push_back(i32); 

      /* create the struct */
      llvm::StructType *arrayStruct = llvm::StructType::create(TheContext, typeName);
      arrayStruct->setBody(members);

      return arrayStruct->getPointerTo();
    }
    case TypeClassType::REF: 
      return getLLVMType(((RefType *)t)->getType())->getPointerTo();
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
  llvm::Type *stringType = getLLVMType(new ArrayType(new SimpleType(BaseType::CHAR)))->getPointerElementType();

  /* writeString - lib.a */
  llvm::FunctionType *writeString_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext),
                      {llvm::PointerType::get(i8, 0)}, false);
  TheWriteString =
    llvm::Function::Create(writeString_type, llvm::Function::ExternalLinkage,
                      "writeString", TheModule.get());

  /* print_string */
  llvm::FunctionType *printString_type = 
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { stringType->getPointerTo() }, false);
  ThePrintStringInternal =
    llvm::Function::Create(printString_type, llvm::Function::InternalLinkage,
                  "print_string", TheModule.get());
  llvm::BasicBlock *ThePrintStringBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", ThePrintStringInternal);
  Builder.SetInsertPoint(ThePrintStringBB);
  llvm::Value *printstr_strPtr = Builder.CreateLoad(Builder.CreateGEP(stringType, ThePrintStringInternal->getArg(0), { c32(0), c32(0) }, "stringPtr"));
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

  /* writeChar - lib.a */
  llvm::FunctionType *writeChar_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), { i8 }, false);
  TheWriteChar =
    llvm::Function::Create(writeChar_type, llvm::Function::ExternalLinkage,
                  "writeChar", TheModule.get());
  /* print_char */
  llvm::FunctionType *printChar_type = 
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { i8 }, false);
  ThePrintCharInternal =
    llvm::Function::Create(printChar_type, llvm::Function::InternalLinkage,
                  "print_char", TheModule.get());
  llvm::BasicBlock *ThePrintCharBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", ThePrintCharInternal);
  Builder.SetInsertPoint(ThePrintCharBB);
  Builder.CreateCall(TheWriteChar, { ThePrintCharInternal->getArg(0) });
  Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  TheFPM->run(*ThePrintCharInternal);
  LLVMValueStore->newLLVMValue("print_char", ThePrintCharInternal);

  /* writeBoolean - lib.a */
  llvm::FunctionType *writeBoolean_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), { i1 }, false);
  TheWriteBoolean =
    llvm::Function::Create(writeBoolean_type, llvm::Function::ExternalLinkage,
                  "writeBoolean", TheModule.get());
  /* print_bool */
  llvm::FunctionType *printBool_type = 
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { i1 }, false);
  ThePrintBoolInternal =
    llvm::Function::Create(printBool_type, llvm::Function::InternalLinkage,
                  "print_bool", TheModule.get());
  llvm::BasicBlock *ThePrintBoolBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", ThePrintBoolInternal);
  Builder.SetInsertPoint(ThePrintBoolBB);
  Builder.CreateCall(TheWriteBoolean, { ThePrintBoolInternal->getArg(0) });
  Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  TheFPM->run(*ThePrintBoolInternal);
  LLVMValueStore->newLLVMValue("print_bool", ThePrintBoolInternal);

  /* readInteger - lib.a */
  llvm::FunctionType *readInteger_type =
    llvm::FunctionType::get(i32, false);
  TheReadInteger =
    llvm::Function::Create(readInteger_type, llvm::Function::ExternalLinkage,
                  "readInteger", TheModule.get());
  /* read_int */
  llvm::FunctionType *readInt_type = 
    llvm::FunctionType::get(i32, { TheModule->getTypeByName("unit") }, false);
  TheReadIntInternal =
    llvm::Function::Create(readInt_type, llvm::Function::InternalLinkage,
                  "read_int", TheModule.get());
  llvm::BasicBlock *TheReadIntBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheReadIntInternal);
  Builder.SetInsertPoint(TheReadIntBB);
  Builder.CreateRet(Builder.CreateCall(TheReadInteger));
  TheFPM->run(*TheReadIntInternal);
  LLVMValueStore->newLLVMValue("read_int", TheReadIntInternal);
  
  /* readBoolean - lib.a */
  llvm::FunctionType *readBoolean_type =
    llvm::FunctionType::get(i1, false);
  TheReadBoolean =
    llvm::Function::Create(readBoolean_type, llvm::Function::ExternalLinkage,
                  "readBoolean", TheModule.get());
  /* read_bool */
  llvm::FunctionType *readBool_type = 
    llvm::FunctionType::get(i1, { TheModule->getTypeByName("unit") }, false);
  TheReadBoolInternal =
    llvm::Function::Create(readBool_type, llvm::Function::InternalLinkage,
                  "read_bool", TheModule.get());
  llvm::BasicBlock *TheReadBoolBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheReadBoolInternal);
  Builder.SetInsertPoint(TheReadBoolBB);
  Builder.CreateRet(Builder.CreateCall(TheReadBoolean));
  TheFPM->run(*TheReadBoolInternal);
  LLVMValueStore->newLLVMValue("read_bool", TheReadBoolInternal);
  
  /* readChar - lib.a */
  llvm::FunctionType *readCharacter_type =
    llvm::FunctionType::get(i8, false);
  TheReadChar =
    llvm::Function::Create(readCharacter_type, llvm::Function::ExternalLinkage,
                  "readChar", TheModule.get());
  /* read_char */
  llvm::FunctionType *readChar_type = 
    llvm::FunctionType::get(i8, { TheModule->getTypeByName("unit") }, false);
  TheReadCharInternal =
    llvm::Function::Create(readChar_type, llvm::Function::InternalLinkage,
                  "read_char", TheModule.get());
  llvm::BasicBlock *TheReadCharBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheReadCharInternal);
  Builder.SetInsertPoint(TheReadCharBB);
  Builder.CreateRet(Builder.CreateCall(TheReadChar));
  TheFPM->run(*TheReadCharInternal);
  LLVMValueStore->newLLVMValue("read_char", TheReadCharInternal);

  /* readString - lib.a */
  llvm::FunctionType *readString_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext),
                      { i32, llvm::PointerType::get(i8, 0) }, false);
  TheReadString =
    llvm::Function::Create(readString_type, llvm::Function::ExternalLinkage,
                  "readString", TheModule.get());
  /* read_string */
  llvm::FunctionType *readStringInternal_type =
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { stringType->getPointerTo() }, false);
  TheReadStringInternal =
    llvm::Function::Create(readStringInternal_type, llvm::Function::InternalLinkage,
                  "read_string", TheModule.get());
  llvm::BasicBlock *TheReadStringInternalBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheReadStringInternal);
  Builder.SetInsertPoint(TheReadStringInternalBB);
  llvm::Value *strPtr = Builder.CreateLoad(Builder.CreateGEP(stringType, TheReadStringInternal->getArg(0), { c32(0), c32(0) }, "stringPtr"));
  llvm::Value *sizePtr = Builder.CreateLoad(Builder.CreateGEP(stringType, TheReadStringInternal->getArg(0), { c32(0), c32(2) }, "sizePtr"));
  Builder.CreateCall(TheReadString, { sizePtr, strPtr });
  Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  TheFPM->run(*TheReadStringInternal);
  LLVMValueStore->newLLVMValue("read_string", TheReadStringInternal);

  /* abs */
  llvm::FunctionType *abs_type =
    llvm::FunctionType::get(i32, std::vector<llvm::Type *> { i32 }, false);
  TheAbs =
    llvm::Function::Create(abs_type, llvm::Function::ExternalLinkage,
                  "abs", TheModule.get());
  LLVMValueStore->newLLVMValue("abs", TheAbs);

  /* incr */
  llvm::FunctionType *incr_type =
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { llvm::PointerType::get(i32, 0) }, false);
  TheIncr =
    llvm::Function::Create(incr_type, llvm::Function::InternalLinkage,
                  "incr", TheModule.get());
  llvm::BasicBlock *TheIncrBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheIncr);
  Builder.SetInsertPoint(TheIncrBB);
  llvm::Value *valToIncr = Builder.CreateLoad(TheIncr->getArg(0));
  llvm::Value *valIncreased = Builder.CreateAdd(valToIncr, c32(1));
  Builder.CreateStore(valIncreased, TheIncr->getArg(0));
  Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  TheFPM->run(*TheIncr);
  LLVMValueStore->newLLVMValue("incr", TheIncr);
  
  /* decr */
  llvm::FunctionType *decr_type =
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { llvm::PointerType::get(i32, 0) }, false);
  TheDecr =
    llvm::Function::Create(decr_type, llvm::Function::InternalLinkage,
                  "decr", TheModule.get());
  llvm::BasicBlock *TheDecrBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheDecr);
  Builder.SetInsertPoint(TheDecrBB);
  llvm::Value *valToDecr = Builder.CreateLoad(TheDecr->getArg(0));
  llvm::Value *valDecreased = Builder.CreateSub(valToDecr, c32(1));
  Builder.CreateStore(valDecreased, TheDecr->getArg(0));
  Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  TheFPM->run(*TheDecr);
  LLVMValueStore->newLLVMValue("decr", TheDecr);

  /* ord - lib.a */
  llvm::FunctionType *ord_type =
    llvm::FunctionType::get(i32, std::vector<llvm::Type *> { i8 }, false);
  TheOrd =
    llvm::Function::Create(ord_type, llvm::Function::ExternalLinkage,
                  "ord", TheModule.get());
  /* int_of_char */
  llvm::FunctionType *intOfChar_type = 
    llvm::FunctionType::get(i32, { i8 }, false);
  TheIntOfChar =
    llvm::Function::Create(intOfChar_type, llvm::Function::InternalLinkage,
                  "int_of_char", TheModule.get());
  llvm::BasicBlock *TheIntOfCharBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheIntOfChar);
  Builder.SetInsertPoint(TheIntOfCharBB);
  Builder.CreateRet(Builder.CreateCall(TheOrd, { TheIntOfChar->getArg(0) }));
  TheFPM->run(*TheIntOfChar);
  LLVMValueStore->newLLVMValue("int_of_char", TheIntOfChar);

  /* TheChr */
  llvm::FunctionType *chr_type =
    llvm::FunctionType::get(i8, std::vector<llvm::Type *> { i32 }, false);
  TheChr =
    llvm::Function::Create(chr_type, llvm::Function::ExternalLinkage,
                  "chr", TheModule.get());
  /* char_of_int */
  llvm::FunctionType *charOfInt_type = 
  llvm::FunctionType::get(i8, { i32 }, false);
  TheCharOfInt =
    llvm::Function::Create(charOfInt_type, llvm::Function::InternalLinkage,
                  "char_of_int", TheModule.get());
  llvm::BasicBlock *TheCharOfIntBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheCharOfInt);
  Builder.SetInsertPoint(TheCharOfIntBB);
  Builder.CreateRet(Builder.CreateCall(TheChr, { TheCharOfInt->getArg(0) }));
  TheFPM->run(*TheCharOfInt);
  LLVMValueStore->newLLVMValue("char_of_int", TheCharOfInt);

  /* strlen - lib.a */
  llvm::FunctionType *stringLength_type =
    llvm::FunctionType::get(i32, { llvm::PointerType::get(i8, 0) }, false);
  TheStringLength =
    llvm::Function::Create(stringLength_type, llvm::Function::ExternalLinkage,
                  "strlen", TheModule.get());
  /* strlen */
  llvm::FunctionType *strlen_type = 
    llvm::FunctionType::get(i32, { stringType->getPointerTo() }, false);
  TheStringLengthInternal =
    llvm::Function::Create(strlen_type, llvm::Function::InternalLinkage,
                  "strlen", TheModule.get());
  llvm::BasicBlock *TheStringLengthInternalBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheStringLengthInternal);
  Builder.SetInsertPoint(TheStringLengthInternalBB);
  llvm::Value *strlen_strPtr = Builder.CreateLoad(Builder.CreateGEP(stringType, TheStringLengthInternal->getArg(0), { c32(0), c32(0) }, "stringPtr"));
  Builder.CreateRet(Builder.CreateCall(TheStringLength, { strlen_strPtr }));
  TheFPM->run(*TheStringLengthInternal);
  LLVMValueStore->newLLVMValue("strlen", TheStringLengthInternal);
  
  /* strcmp - lib.a */
  llvm::FunctionType *stringCompare_type =
    llvm::FunctionType::get(i32, { llvm::PointerType::get(i8, 0), llvm::PointerType::get(i8, 0) }, false);
  TheStringCompare =
    llvm::Function::Create(stringCompare_type, llvm::Function::ExternalLinkage,
                  "strcmp", TheModule.get());
  /* strcmp */
  llvm::FunctionType *strcmp_type = 
    llvm::FunctionType::get(i32, { stringType->getPointerTo(), stringType->getPointerTo() }, false);
  TheStringCompareInternal =
    llvm::Function::Create(strcmp_type, llvm::Function::InternalLinkage,
                  "strcmp", TheModule.get());
  llvm::BasicBlock *TheStringCompareInternalBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheStringCompareInternal);
  Builder.SetInsertPoint(TheStringCompareInternalBB);
  llvm::Value *strcmp_strPtr1 = Builder.CreateLoad(Builder.CreateGEP(stringType, TheStringCompareInternal->getArg(0), { c32(0), c32(0) }, "stringPtr1"));
  llvm::Value *strcmp_strPtr2 = Builder.CreateLoad(Builder.CreateGEP(stringType, TheStringCompareInternal->getArg(1), { c32(0), c32(0) }, "stringPtr2"));
  Builder.CreateRet(Builder.CreateCall(TheStringCompare, { strcmp_strPtr1, strcmp_strPtr2 }));
  TheFPM->run(*TheStringCompareInternal);
  LLVMValueStore->newLLVMValue("strcmp", TheStringCompareInternal);  
  
  /* strcpy - lib.a */
  llvm::FunctionType *stringCopy_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), { llvm::PointerType::get(i8, 0), llvm::PointerType::get(i8, 0) }, false);
  TheStringCopy =
    llvm::Function::Create(stringCopy_type, llvm::Function::ExternalLinkage,
                  "strcpy", TheModule.get());
  /* strcpy */
  llvm::FunctionType *strcpy_type = 
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { stringType->getPointerTo(), stringType->getPointerTo() }, false);
  TheStringCopyInternal =
    llvm::Function::Create(strcpy_type, llvm::Function::InternalLinkage,
                  "strcpy", TheModule.get());
  llvm::BasicBlock *TheStringCopyInternalBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheStringCopyInternal);
  Builder.SetInsertPoint(TheStringCopyInternalBB);
  llvm::Value *strcpy_strPtr1 = Builder.CreateLoad(Builder.CreateGEP(stringType, TheStringCopyInternal->getArg(0), { c32(0), c32(0) }, "stringPtr"));
  llvm::Value *strcpy_strPtr2 = Builder.CreateLoad(Builder.CreateGEP(stringType, TheStringCopyInternal->getArg(1), { c32(0), c32(0) }, "stringPtr"));
  Builder.CreateCall(TheStringCopy, { strcpy_strPtr1, strcpy_strPtr2 });
  Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  TheFPM->run(*TheStringCopyInternal);
  LLVMValueStore->newLLVMValue("strcpy", TheStringCopyInternal); 
  
  /* strcat - lib.a */
  llvm::FunctionType *stringConcat_type =
    llvm::FunctionType::get(llvm::Type::getVoidTy(TheContext), { llvm::PointerType::get(i8, 0), llvm::PointerType::get(i8, 0) }, false);
  TheStringConcat =
    llvm::Function::Create(stringConcat_type, llvm::Function::ExternalLinkage,
                  "strcat", TheModule.get());
  /* strcat */
  llvm::FunctionType *strcat_type = 
    llvm::FunctionType::get(TheModule->getTypeByName("unit"), { stringType->getPointerTo(), stringType->getPointerTo() }, false);
  TheStringConcatInternal =
    llvm::Function::Create(strcat_type, llvm::Function::InternalLinkage,
                  "strcat", TheModule.get());
  llvm::BasicBlock *TheStringConcatInternalBB = llvm::BasicBlock::Create(TheModule->getContext(), "entry", TheStringConcatInternal);
  Builder.SetInsertPoint(TheStringConcatInternalBB);
  llvm::Value *strcat_strPtr1 = Builder.CreateLoad(Builder.CreateGEP(stringType, TheStringConcatInternal->getArg(0), { c32(0), c32(0) }, "stringPtr1"));
  llvm::Value *strcat_strPtr2 = Builder.CreateLoad(Builder.CreateGEP(stringType, TheStringConcatInternal->getArg(1), { c32(0), c32(0) }, "stringPtr2"));
  Builder.CreateCall(TheStringConcat, { strcat_strPtr1, strcat_strPtr2 });
  Builder.CreateRet(llvm::ConstantAggregateZero::get(TheModule->getTypeByName("unit")));
  TheFPM->run(*TheStringConcatInternal);
  LLVMValueStore->newLLVMValue("strcat", TheStringConcatInternal); 
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
