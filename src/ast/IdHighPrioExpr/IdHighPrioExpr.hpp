#ifndef __IdHighPrioExpr_HPP__
#define __IdHighPrioExpr_HPP__

#include <iostream>

#include "../ExprBlock/ExprBlock.hpp"

class IdHighPrioExpr: public HighPrioExpr {
public:
  IdHighPrioExpr(std::string i, ExprBlock *c = nullptr);
  ~IdHighPrioExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  std::string id;
  ExprBlock *commaExprList;
};


#endif
