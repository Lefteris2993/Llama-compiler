#ifndef __Expr_HPP__
#define __Expr_HPP__

#include "../AST/AST.hpp"
#include "../../types/types.hpp"

class Expr: public AST {
public:
  Type *getType();

protected:
  Type *type;
};

typedef Expr HighPrioExpr;



#endif
