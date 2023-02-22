#ifndef __CharHighPrioExpr_HPP__
#define __CharHighPrioExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class CharHighPrioExpr: public HighPrioExpr {
public:
  CharHighPrioExpr(char v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  char val;
};


#endif
