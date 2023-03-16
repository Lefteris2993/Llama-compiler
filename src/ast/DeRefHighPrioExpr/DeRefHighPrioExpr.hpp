#ifndef __DeRefHighPrioExpr_HPP__
#define __DeRefHighPrioExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class DeRefHighPrioExpr: public HighPrioExpr {
public:
  DeRefHighPrioExpr(HighPrioExpr *e);
  ~DeRefHighPrioExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  HighPrioExpr *expr;
};


#endif
