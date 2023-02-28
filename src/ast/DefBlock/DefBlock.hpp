#ifndef __DefBlock_HPP__
#define __DefBlock_HPP__

#include <iostream>
#include <vector>

#include "../Stmt/Stmt.hpp"
#include "../Def/Def.hpp"

class DefBlock: public Stmt {
public:
  DefBlock();
  ~DefBlock();
  void append(Def *d);
  virtual void sem() override;
  void recSem();
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  std::vector<Def *> block;
};



#endif
