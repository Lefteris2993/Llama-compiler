#ifndef __MutableDef_HPP__
#define __MutableDef_HPP__

#include <iostream>

#include "../Def/Def.hpp"

class MutableDef: public Def {
public:
  MutableDef(std::string i, Type *t = nullptr);
  ~MutableDef();
  virtual DefType getDefType() const override;
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  std::string id;
  Type *type;
};


#endif
