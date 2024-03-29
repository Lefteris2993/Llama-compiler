#ifndef __FunctionCall_HPP__
#define __FunctionCall_HPP__

#include <iostream>
#include <llvm/IR/Value.h>

#include "../HighPrioExprBlock/HighPrioExprBlock.hpp"

class FunctionCall: public Expr {
public:
  FunctionCall(std::string id, HighPrioExprBlock *b);
  ~FunctionCall();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  std::string id;
  HighPrioExprBlock *block;
};


#endif
