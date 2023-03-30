#ifndef __Def_HPP__
#define __Def_HPP__

#include "../Stmt/Stmt.hpp"
#include "../../types/types.hpp"

class Def: public Stmt {
public:
  Def();
  ~Def();
  virtual DefType getDefType() const = 0;
  // virtual class llvm::Value *codegen is overridden 
};



#endif
