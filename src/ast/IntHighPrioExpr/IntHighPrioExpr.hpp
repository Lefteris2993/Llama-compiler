#ifndef __IntHighPrioExpr_HPP__
#define __IntHighPrioExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class IntHighPrioExpr: public HighPrioExpr {
public:
  IntHighPrioExpr(int v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  int val;
};


#endif
