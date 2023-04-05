#ifndef __NumPattern_HPP__
#define __NumPattern_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class NumPattern: public Pattern {
public:
  NumPattern(int n, bool neg = false);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  int num;
  bool negative;
};



#endif
