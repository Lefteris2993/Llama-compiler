#ifndef __Def_HPP__
#define __Def_HPP__

#include "../AST/AST.hpp"
#include "../../types/types.hpp"

class Def: public Stmt {
public:
  Def();
  ~Def();
  virtual DefType getDefType() const = 0;
  virtual std::string getId() const = 0;
  // virtual class llvm::Value *codegen is overridden 
protected:
  std::string id;
};



#endif
