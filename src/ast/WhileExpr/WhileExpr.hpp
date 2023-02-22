#ifndef __WhileExpr_HPP__
#define __WhileExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class WhileExpr: public Expr {
public:
  WhileExpr(Expr *c, Expr *b);
  ~WhileExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  Expr *cond;
  Expr *body;
};



#endif
