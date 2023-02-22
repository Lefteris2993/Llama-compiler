#ifndef __NewExpr_HPP__
#define __NewExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"
#include "../../types/types.hpp"

class NewExpr: public Expr {
public:
  NewExpr(Type *t);
  ~NewExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
};


#endif
