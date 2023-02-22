#ifndef __Par_HPP__
#define __Par_HPP__

#include <iostream>

#include "../AST/AST.hpp"
#include "../../symbol/symbol.hpp"

class Par: public AST {
public:
  Par(std::string i, Type *t = nullptr);
  ~Par();
  virtual void sem() override;
  void insertParam(FunSymbolEntry *f);
  Type *getType();
  virtual void printOn(std::ostream &out) const override;
private:
  std::string id;
  Type *type;
};



#endif
