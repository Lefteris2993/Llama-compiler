#ifndef __AST_HPP__
#define __AST_HPP__

#include <iostream>

#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LegacyPassManager.h>

#include "../../symbol/symbol.hpp"

class AST {
public:
  virtual ~AST();
  virtual void printOn(std::ostream &out) const = 0;
  virtual void sem();
  virtual void setLineno(unsigned l);
  virtual llvm::Value* codegen();
  void llvm_compile_and_dump(bool optimize = true);

  unsigned lineno;

protected:
  static std::unique_ptr<SymbolTable> LLVMValueStore;

  static llvm::LLVMContext TheContext;
  static llvm::IRBuilder<> Builder;
  static std::unique_ptr<llvm::Module> TheModule;
  static std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;

  static llvm::Type *i1;
  static llvm::Type *i8;
  static llvm::Type *i32;
  static llvm::Type *i64;

  static llvm::ConstantInt* c1(int c);
  static llvm::ConstantInt* c8(char c);
  static llvm::ConstantInt* c32(int n);
  static llvm::ConstantInt* c64(int n);

  static llvm::Function *TheWriteString;
  static llvm::Function *TheWriteInteger;
  static llvm::Function *ThePrintIntInternal;
private:
  void codegenLibs();
};

inline std::ostream& operator<< (std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}

#endif