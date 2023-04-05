#ifndef __SigOpExpr_HPP__
#define __SigOpExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class SigOpExpr: public Expr {
public:
  SigOpExpr(SigOp p, Expr *e);
  ~SigOpExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  Expr *expr;
  SigOp op;
};



#endif
