#ifndef __BinOpExpr_HPP__
#define __BinOpExpr_HPP__

#include "../Expr/Expr.hpp"

class BinOpExpr: public Expr {
public:
  BinOpExpr(Expr *l, BinOp p, Expr *r);
  ~BinOpExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;

private:
  Expr *lhs;
  Expr *rhs;
  BinOp op;
};

#endif