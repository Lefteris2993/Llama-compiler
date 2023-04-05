#ifndef __CharPattern_HPP__
#define __CharPattern_HPP__

#include <iostream>
#include "../Expr/Expr.hpp"

class CharPattern: public Pattern {
public:
  CharPattern(char c);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  char var;
};



#endif
