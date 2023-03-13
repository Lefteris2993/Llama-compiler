#ifndef __IfThenElseExpr_HPP__
#define __IfThenElseExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class IfThenElseExpr: public Expr {
public:
  IfThenElseExpr(Expr *c, Expr *e1, Expr *e2 = nullptr);
  ~IfThenElseExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  Expr *cond;
  Expr *expr1;
  Expr *expr2;
};


#endif
