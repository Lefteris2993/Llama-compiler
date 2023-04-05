#ifndef __StmtBlock_HPP__
#define __StmtBlock_HPP__

#include <iostream>
#include <vector>

#include "../AST/AST.hpp"

class StmtBlock: public Stmt {
public:
  StmtBlock();
  ~StmtBlock();
  void append(Stmt *s);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  virtual llvm::Value* codegen() override;
private:
  std::vector<Stmt *> stmt_list;
};


#endif
