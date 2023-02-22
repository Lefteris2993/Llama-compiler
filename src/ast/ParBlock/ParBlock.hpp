#ifndef __ParBlock_HPP__
#define __ParBlock_HPP__

#include <iostream>
#include <vector>

#include "../AST/AST.hpp"
#include "../Par/Par.hpp"

class ParBlock: public AST {
public:
  ParBlock();
  ~ParBlock();
  void append(Par *p);
  virtual void sem() override;
  void insertParams(FunSymbolEntry *f);
  virtual void printOn(std::ostream &out) const override;
  std::vector<Par *> block;
};



#endif
