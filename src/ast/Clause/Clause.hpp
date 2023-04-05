#ifndef __Clause_HPP__
#define __Clause_HPP__

#include "../AST/AST.hpp"
#include "../Expr/Expr.hpp"
#include "../Expr/Expr.hpp"

class Clause: public AST {
public:
  Clause(Pattern *p, Expr *e);
  ~Clause();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  Pattern *getPattern();
  Expr *getExpr();
private:
  Pattern *pattern;
  Expr *expr;
};

#endif