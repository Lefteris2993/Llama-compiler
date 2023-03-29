#ifndef __DimExpr_HPP__
#define __DimExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class DimExpr: public Expr {
public:
  DimExpr(std::string i, unsigned c = 1);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  std::string id;
  unsigned dimension;
};


#endif
