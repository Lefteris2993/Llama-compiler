#ifndef __AST_HPP__
#define __AST_HPP__

#include <iostream>
#include <vector>
#include <string>
#include "types.hpp"
#include "lexer.hpp"
#include "error.hpp"
#include "symbol.hpp"

enum BinOp { BIN_PLUS, BIN_MINUS, STAR, DIV, MOD, STRUCT_EQ, STRUCT_NE, L, G, LE, GE, EQ, NE, AND, OR, ASS, PAR };

enum SigOp { NOT, SIG_PLUS, SIG_MINUS, EXCL };

enum DefType { DEF_MUTABLE, DEF_IMMUTABLE, DEF_ARRAY };

extern Type *intType;
extern Type *unitType;
extern Type *charType;
extern Type *boolType;

extern SymbolTable *symbolaTable;

class AST {
public:
  virtual ~AST();
  virtual void printOn(std::ostream &out) const = 0;
  virtual void sem();
  virtual void setLineno(unsigned l);

  unsigned lineno;
};

inline std::ostream& operator<< (std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}

class Stmt: public AST {};

class Def: public Stmt {
public:
  Def();
  ~Def();
  virtual DefType getDefType() const = 0;
};

class Expr: public AST {
public:
  Type *getType();

protected:
  Type *type;
};

class Pattern: public Expr {};

class ExprBlock: public AST {
public:
  ExprBlock();
  ~ExprBlock();
  void append(Expr *c);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override ;

private:
  std::vector<Expr *> block;
};

class NumPattern: public Pattern {
public:
  NumPattern(int n, bool neg = false);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  int num;
  bool negative;
};

class CharPattern: public Pattern {
public:
  CharPattern(char c);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  char var;
};

class BoolPattern: public Pattern {
public:
  BoolPattern(bool v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  bool val;
};

class VarPattern: public Pattern {
public:
  VarPattern(std::string v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  std::string var;
};

class Clause: public AST {
public:
  Clause(Pattern *p, Expr *e);
  ~Clause();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

  Pattern *getPattern();
  Expr *getExpr();
private:
  Pattern *pattern;
  Expr *expr;
};

class ClauseBlock: public AST {
public:
  ClauseBlock();
  ~ClauseBlock();
  void append(Clause *c);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  std::vector<Clause*> clauses;
};

class BinOpExpr: public Expr {
public:
  BinOpExpr(Expr *l, BinOp p, Expr *r);
  ~BinOpExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  Expr *lhs;
  Expr *rhs;
  BinOp op;
};

class SigOpExpr: public Expr {
public:
  SigOpExpr(SigOp p, Expr *e);
  ~SigOpExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
  
private:
  Expr *expr;
  SigOp op;
};

class DefBlock: public Stmt {
public:
  DefBlock();
  ~DefBlock();
  void append(Def *d);
  virtual void sem() override;
  void recSem();
  virtual void printOn(std::ostream &out) const override;

private:
  std::vector<Def *> block;
};

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

class LetInExpr: public Expr {
public:
  LetInExpr(LetDef *l, Expr *e);
  ~LetInExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  LetDef *let;
  Expr *expr;

};

class WhileExpr: public Expr {
public:
  WhileExpr(Expr *c, Expr *b);
  ~WhileExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  Expr *cond;
  Expr *body;
};

class ForExpr: public Expr {
public:
  ForExpr(bool u, std::string i, Expr *q, Expr *t, Expr *b);
  ~ForExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  bool up;
  std::string id;
  Expr *eq;
  Expr *to;
  Expr *body;
};

class DimExpr: public Expr {
public:
  DimExpr(std::string i, unsigned c = -1);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  std::string id;
  unsigned dimention;
};

class NewExpr: public Expr {
public:
  NewExpr(Type *t);
  ~NewExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  Type *type;
};

class DeleteExpr: public Expr {
public:
  DeleteExpr(Expr *e);
  ~DeleteExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  Expr *expr;
};

class IfThenElseExpr: public Expr {
public:
  IfThenElseExpr(Expr *c, Expr *e1, Expr *e2 = nullptr);
  ~IfThenElseExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  Expr *cond;
  Expr *expr1;
  Expr *expr2;
};

class MatchExpr: public Expr {
public:
  MatchExpr(Expr *e, ClauseBlock *c);
  ~MatchExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  Expr *expr;
  ClauseBlock *block;
};

typedef Expr HighPrioExpr;

class DeRefHighPrioExpr: public HighPrioExpr {
public:
  DeRefHighPrioExpr(HighPrioExpr *e);
  ~DeRefHighPrioExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  HighPrioExpr *expr;
};

class UnitHighPrioExpr: public HighPrioExpr {
public:
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:

};

class IntHighPrioExpr: public HighPrioExpr {
public:
  IntHighPrioExpr(int v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  int val;
};

class CharHighPrioExpr: public HighPrioExpr {
public:
  CharHighPrioExpr(char v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  char val;
};

class StringHighPrioExpr: public HighPrioExpr {
public:
  StringHighPrioExpr(std::string s);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  std::string val;
};

class BoolHighPrioExpr: public HighPrioExpr {
public:
  BoolHighPrioExpr(bool v);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  bool val;
};

class IdHighPrioExpr: public HighPrioExpr {
public:
  IdHighPrioExpr(std::string i, ExprBlock *c = nullptr);
  ~IdHighPrioExpr();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  std::string id;
  ExprBlock *commaExprList;
};

class HighPrioExprBlock: public Expr {
public:
  HighPrioExprBlock();
  ~HighPrioExprBlock();
  void append(HighPrioExpr *d);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  std::vector<HighPrioExpr *> block;
  std::string id;
};

class StmtBlock: public Stmt {
public:
  StmtBlock();
  ~StmtBlock();
  void append(Stmt *s);
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  std::vector<Stmt *> stmt_list;
};

class MutableDef: public Def {
public:
  MutableDef(std::string i, Type *t = nullptr);
  ~MutableDef();
  virtual DefType getDefType() const override;
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  std::string id;
  Type *type;
};

class MutableArrayDef: public Def {
public:
  MutableArrayDef(std::string i, ExprBlock *b, Type *t = nullptr);
  ~MutableArrayDef();
  virtual DefType getDefType() const override;
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;

private:
  std::string id;
  ExprBlock *block;
  Type *type;
};

class Par: public AST {
public:
  Par(std::string i, Type *t = nullptr);
  ~Par();
  virtual void sem() override;
  void insertParam(FunSymbolEntry *f);
  virtual void printOn(std::ostream &out) const override;
private:
  std::string id;
  Type *type;
};

class ParBlock: public AST {
public:
  ParBlock();
  ~ParBlock();
  void append(Par *p);
  virtual void sem() override;
  void insertParams(FunSymbolEntry *f);
  virtual void printOn(std::ostream &out) const override;
private:
  std::vector<Par *> block;
};

class ImmutableDef: public Def {
public:
  ImmutableDef(std::string i, ParBlock *p, Expr *e, Type *t = nullptr);
  ~ImmutableDef();
  virtual DefType getDefType() const override;
  virtual void sem() override;
  void decl();
  virtual void printOn(std::ostream &out) const override;

private:
  std::string id;
  Expr *expr;
  ParBlock *block;
  Type *type;
};

class FunctionCall: public Expr {
public:
  FunctionCall(std::string id, HighPrioExprBlock *b);
  ~FunctionCall();
  virtual void sem() override;
  virtual void printOn(std::ostream &out) const override;
private:
  std::string id;
  HighPrioExprBlock *block;
};

#endif
