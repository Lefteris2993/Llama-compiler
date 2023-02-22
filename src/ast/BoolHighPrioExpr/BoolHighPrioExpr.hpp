#ifndef __BoolHighPrioExpr_HPP__
#define __BoolHighPrioExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class BoolHighPrioExpr: public HighPrioExpr {
public:
  BoolHighPrioExpr(bool v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  bool val;
};

#endif