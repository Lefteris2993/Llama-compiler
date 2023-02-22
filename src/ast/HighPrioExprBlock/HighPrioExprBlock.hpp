#ifndef __HighPrioExprBlock_HPP__
#define __HighPrioExprBlock_HPP__

#include <iostream>
#include <vector>

#include "../Expr/Expr.hpp"
#include "../../symbol/symbol.hpp"

class HighPrioExprBlock: public Expr {
public:
  HighPrioExprBlock();
  ~HighPrioExprBlock();
  void append(HighPrioExpr *d);
  virtual void sem() override;
  void parCheck(FunSymbolEntry *f);
  virtual void printOn(std::ostream &out) const override;
  unsigned getBlockLength();

private:
  std::vector<HighPrioExpr *> block;
};


#endif
