#ifndef __UnitHighPrioExpr_HPP__
#define __UnitHighPrioExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class UnitHighPrioExpr: public HighPrioExpr {
public:
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
};


#endif
