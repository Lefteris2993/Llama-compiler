#ifndef __LetInExpr_HPP__
#define __LetInExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"
#include "../LetDef/LetDef.hpp"

class LetInExpr: public Expr {
public:
  LetInExpr(LetDef *l, Expr *e);
  ~LetInExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  LetDef *let;
  Expr *expr;
};



#endif
