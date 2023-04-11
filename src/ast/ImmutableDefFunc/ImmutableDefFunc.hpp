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
  virtual std::string getId() const override;
  virtual void sem() override;
  void decl();
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
  void funGen(llvm::Function *fi = nullptr);
  llvm::Function* defCodeGen();

private:
  Expr *expr;
  ParBlock *block;
  Type *type;
};



#endif
