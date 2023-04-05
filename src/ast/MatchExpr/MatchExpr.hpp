#ifndef __MatchExpr_HPP__
#define __MatchExpr_HPP__

#include <iostream>

#include "../ClauseBlock/ClauseBlock.hpp"

class MatchExpr: public Expr {
public:
  MatchExpr(Expr *e, ClauseBlock *c);
  ~MatchExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  Expr *expr;
  ClauseBlock *block;
};


#endif
