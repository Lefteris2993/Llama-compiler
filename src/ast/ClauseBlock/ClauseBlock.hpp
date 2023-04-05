#ifndef __ClauseBlock_HPP__
#define __ClauseBlock_HPP__

#include <vector>

#include "../AST/AST.hpp"
#include "../Clause/Clause.hpp"

class ClauseBlock: public AST {
public:
  ClauseBlock();
  ~ClauseBlock();
  void append(Clause *c);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  std::vector<Clause*> getClauses();
  Type * getClausesExprsType();
  // llvm::Value* codegen() is indirectly implemented in MatchExpr
private:
  std::vector<Clause*> clauses;
  Type *clausesExprsType;
};



#endif
