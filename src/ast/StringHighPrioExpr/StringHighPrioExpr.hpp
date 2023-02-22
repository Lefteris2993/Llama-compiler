#ifndef __StringHighPrioExpr_HPP__
#define __StringHighPrioExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class StringHighPrioExpr: public HighPrioExpr {
public:
  StringHighPrioExpr(std::string s);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  std::string val;
};


#endif
