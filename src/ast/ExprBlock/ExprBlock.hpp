#ifndef __ExprBlock_HPP__
#define __ExprBlock_HPP__

#include <iostream>
#include <vector>

#include "../AST/AST.hpp"
#include "../Expr/Expr.hpp"

class ExprBlock: public AST {
public:
  ExprBlock();
  ~ExprBlock();
  void append(Expr *c);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  std::vector<llvm::Value *> codegenValues();

private:
  std::vector<Expr *> block;
};



#endif
