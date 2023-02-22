#ifndef __DeleteExpr_HPP__
#define __DeleteExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class DeleteExpr: public Expr {
public:
  DeleteExpr(Expr *e);
  ~DeleteExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  Expr *expr;
};


#endif
