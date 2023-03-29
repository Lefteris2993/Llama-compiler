#ifndef __MutableArrayDef_HPP__
#define __MutableArrayDef_HPP__

#include <iostream>

#include "../Def/Def.hpp"
#include "../ExprBlock/ExprBlock.hpp"

class MutableArrayDef: public Def {
public:
  MutableArrayDef(std::string i, ExprBlock *b, Type *t = nullptr);
  ~MutableArrayDef();
  virtual DefType getDefType() const override;
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  std::string id;
  ExprBlock *block;
  Type *type;
};


#endif
