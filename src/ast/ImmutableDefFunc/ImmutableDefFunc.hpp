#ifndef __ImmutableDefFunc_HPP__
#define __ImmutableDefFunc_HPP__

#include <iostream>

#include "../Def/Def.hpp"
#include "../ParBlock/ParBlock.hpp"
#include "../Expr/Expr.hpp"

class ImmutableDefFunc: public Def {
public:
  ImmutableDefFunc(std::string i, ParBlock *p, Expr *e, Type *t = nullptr);
  ~ImmutableDefFunc();
  virtual DefType getDefType() const override;
  virtual void sem() override;
  void decl();
  virtual void printOn(std::ostream &out) const override;

private:
  std::string id;
  Expr *expr;
  ParBlock *block;
  Type *type;
};



#endif
