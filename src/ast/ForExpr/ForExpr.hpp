#ifndef __ForExpr_HPP__
#define __ForExpr_HPP__

#include <iostream>

#include "../Expr/Expr.hpp"

class ForExpr: public Expr {
public:
  ForExpr(bool u, std::string i, Expr *q, Expr *t, Expr *b);
  ~ForExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  bool up;
  std::string id;
  Expr *eq;
  Expr *to;
  Expr *body;
};


#endif
