#ifndef __ImmutableDefVar_HPP__
#define __ImmutableDefVar_HPP__

#include <iostream>

#include "../Def/Def.hpp"
#include "../Expr/Expr.hpp"

class ImmutableDefVar: public Def {
public:
  ImmutableDefVar(std::string i, Expr *e, Type *t = nullptr);
  ~ImmutableDefVar();
  virtual DefType getDefType() const override;
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  std::string id;
  Expr *expr;
  Type *type;
};



#endif
