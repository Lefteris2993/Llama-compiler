#ifndef __LetDef_HPP__
#define __LetDef_HPP__

#include <iostream>

#include "../DefBlock/DefBlock.hpp"

class LetDef: public Stmt {
public:
  LetDef(DefBlock *b, bool rec);
  ~LetDef();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  DefBlock *defBlock;
  bool rec;
};



#endif
