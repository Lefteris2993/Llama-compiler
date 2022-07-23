#ifndef __AST_HPP__
#define __AST_HPP__

#include <iostream>
#include <vector>
#include <string>
#include "types.hpp"
#include "lexer.hpp"
#include "error.hpp"

enum BinOp { BIN_PLUS, BIN_MINUS, STAR, DIV, MOD, STRUCT_EQ, STRUCT_NE, L, G, LE, GE, EQ, NE, AND, OR, ASS, PAR };

enum SigOp { NOT, SIG_PLUS, SIG_MINUS, EXCL };

extern const Type *INT_TYPE;
extern const Type *UNIT_TYPE;
extern const Type *CHAR_TYPE;
extern const Type *BOOL_TYPE;

class AST {
public:
  virtual ~AST() {}
  virtual void printOn(std::ostream &out) const = 0;
  virtual void sem() { std::cout << "not implemented\n"; } // TODO: change this to virtual so all classes have to implement this.
  virtual void setLineno(unsigned l) { this->lineno = l; }

  unsigned lineno;
};

inline std::ostream& operator<< (std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}

class Stmt: public AST {};

class Def: public Stmt {};

class Expr: public AST {
public:
  Type *getType() {
    return type;
  }

protected:
  Type *type;
};

class Pattern: public Expr {};

class ExprBlock: public AST {
public:
  ExprBlock(): block() {}
  ~ExprBlock() { for (Expr *b : block) delete b; }
  void append(Expr *c) { block.push_back(c); }
  virtual void sem() override {
    for (Expr *c : block) {
      c->sem();
      if (!Type::equal_types(c->getType(), INT_TYPE)) {
        Logger::error(c->lineno, "Array dimentions must be of type INT");
      }
    }
  }
  virtual void printOn(std::ostream &out) const override {
    out << "ExprBlock(";
    bool first = true;
    for (Expr *c : block) {
      if (!first) out << ", ";
      first = false;
      out << *c;
    }
    out << ")";
  }

private:
  std::vector<Expr *> block;
};

class NumPattern: public Pattern {
public:
  NumPattern(int n, bool neg = false): num(n), negative(neg) {}
  virtual void sem() override {
    this->type = new SimpleType(INT);
  }
  virtual void printOn(std::ostream &out) const override {
    out << "NumPattern(" << (negative ? "-" : "+") << "," << num << ")";
  }
private:
  int num;
  bool negative;
};

class CharPattern: public Pattern {
public:
  CharPattern(char c): var(c) {}
  virtual void sem() override {
    this->type = new SimpleType(CHAR);
  }
  virtual void printOn(std::ostream &out) const override {
    out << "CharPattern(" << var << ")";
  }
private:
  char var;
};

class BoolPattern: public Pattern {
public:
  BoolPattern(bool v): val(v) {}
  virtual void sem() override {
    this->type = new SimpleType(BOOL);
  }
  virtual void printOn(std::ostream &out) const override {
    out << "BoolPattern(" << (val ? "true" : "false") << ")";
  }

private:
  bool val;
};

class VarPattern: public Pattern {
public:
  VarPattern(std::string v): var(v) {}
  virtual void printOn(std::ostream &out) const override {
    out << "VarPattern(" << var << ")";
  }

private:
  std::string var;
};

class Clause: public AST {
public:
  Clause(Pattern *p, Expr *e): pattern(p), expr(e) {}
  ~Clause() { delete pattern; delete expr; }
  virtual void sem() override {
    pattern->sem();
    expr->sem();
  }
  virtual void printOn(std::ostream &out) const override {
    out << "Clause(" << *pattern << "," << *expr << ")";
  }

  Pattern *getPattern() { return pattern; }
  Expr *getExpr() { return expr; }
private:
  Pattern *pattern;
  Expr *expr;
};

class ClauseBlock: public AST {
public:
  ClauseBlock(): clauses() {}
  ~ClauseBlock() { for (Clause *c : clauses) delete c; }
  void append(Clause *c) { clauses.push_back(c); }
  virtual void sem() override {
    for (Clause *c : clauses) c->sem();
    if (clauses.size() < 1) Logger::error(this->lineno, "Match expretion must have at least one clause");
    Type *patternType = clauses[0]->getPattern()->getType();
    Type *exprType = clauses[0]->getExpr()->getType();
    for (Clause *c : clauses) { 
      if (!Type::equal_types(c->getExpr()->getType(), exprType)) {
        Logger::error(c->lineno, "Match expretion clauses must return the same type");
      }  
    }
    for (Clause *c : clauses) {
      if (!Type::equal_types(c->getPattern()->getType(), patternType)) {
        Logger::error(c->lineno, "Match expretion patterns must be the same type");
      }
    }
  }
  virtual void printOn(std::ostream &out) const override {
    out << "ClauseBlock(";
    bool first = true;
    for (Clause *c : clauses) {
      if (!first) out << ", ";
      first = false;
      out << *c;
    }
    out << ")";
  }

private:
  std::vector<Clause*> clauses;
};

class BinOpExpr: public Expr {
public:
  BinOpExpr(Expr *l, BinOp p, Expr *r): lhs(l), rhs(r), op(p) {}
  ~BinOpExpr() { delete lhs; delete rhs; }
  virtual void sem() override {
    lhs->sem();
    rhs->sem();
    switch (op)
    {
    case BIN_PLUS:
    case BIN_MINUS:
    case STAR:
    case DIV:
    case MOD:
      if (
        !Type::equal_types(lhs->getType(), INT_TYPE) 
        || !Type::equal_types(rhs->getType(), INT_TYPE)
      ) {
        Logger::error(this->lineno, "Operands must be of type INT");
      }
      this->type = new SimpleType(INT);
      break; 
    
    default:
      Logger::error(this->lineno, "Not implemented");
      break;
    }
  }
  virtual void printOn(std::ostream &out) const override {
    out << "BinOp(" << op << "," << *lhs << "," << *rhs << ")";
  }

private:
  Expr *lhs;
  Expr *rhs;
  BinOp op;
};

class SigOpExpr: public Expr {
public:
  SigOpExpr(SigOp p, Expr *e): expr(e), op(p) {}
  ~SigOpExpr() { delete expr; }
  virtual void printOn(std::ostream &out) const override {
    out << "SigOp(" << op << "," << *expr << ")";
  }
  
private:
  Expr *expr;
  SigOp op;
};

class DefBlock: public Stmt {
public:
  DefBlock(): block() {}
  ~DefBlock() { for (Def *b : block) delete b; }
  void append(Def *d) { block.push_back(d); }
  virtual void sem() override {
    for (Stmt *s : block) s->sem(); 
  }
  virtual void printOn(std::ostream &out) const override {
    out << "DefBlock(";
    bool first = true;
    for (Stmt *s : block) {
      if (!first) out << ", ";
      first = false;
      out << *s;
    }
    out << ")";
  }

private:
  std::vector<Def *> block;
};

class LetDef: public Stmt {
public:
  LetDef(DefBlock *b, bool rec): defBlock(b), rec(rec) {}
  ~LetDef() { delete defBlock; }
  virtual void sem() override { 
    defBlock->sem(); 
  }
  virtual void printOn(std::ostream &out) const override {
    out << "LetDef";
    if (rec) out << "Rec";
    out << "(";
    out << *defBlock;
    out << ")";
  }

private:
  DefBlock *defBlock;
  bool rec;
};

class LetInExpr: public Expr {
public:
  LetInExpr(LetDef *l, Expr *e): let(l), expr(e) {}
  ~LetInExpr() { delete let; delete expr; }
  virtual void printOn(std::ostream &out) const override {
    out << "LetInExpr(" << *let << "," << *expr << ")";
  }
private:
  LetDef *let;
  Expr *expr;

};

class WhileExpr: public Expr {
public:
  WhileExpr(Expr *c, Expr *b): cond(c), body(b) {}
  ~WhileExpr() { delete cond; delete body; }
  virtual void printOn(std::ostream &out) const override {
    out << "While(" << *cond << "," << *body << ")";
  }
private:
  Expr *cond;
  Expr *body;
};

class ForExpr: public Expr {
public:
  ForExpr(
    bool u, 
    std::string i, 
    Expr *q, 
    Expr *t, 
    Expr *b
  ): up(u), id(i), eq(q), to(t), body(b) {}
  ~ForExpr() { delete eq; delete to; delete body; }
  virtual void printOn(std::ostream &out) const override {
    out << "For(" << (up ? "true" : "false") << "," << id << "," << *eq << "," << *to << "," << *body << ")";  
  }
private:
  bool up;
  std::string id;
  Expr *eq;
  Expr *to;
  Expr *body;
};

class DimExpr: public Expr {
public:
  DimExpr(std::string i, unsigned c = -1): id(i), dimention(c) {}
  virtual void printOn(std::ostream &out) const override {
    out << "Dim(" << dimention << "," << id << ")";
  }
private:
  std::string id;
  unsigned dimention;
};

class NewExpr: public Expr {
public:
  NewExpr(Type *t): type(t) {}
  ~NewExpr() { delete type; }
  virtual void printOn(std::ostream &out) const override {
    out << "New(" << *type << ")"; 
  }
private:
  Type *type;
};

class DeleteExpr: public Expr {
public:
  DeleteExpr(Expr *e): expr(e) {}
  ~DeleteExpr() { delete expr; }
  virtual void printOn(std::ostream &out) const override {
    out << "Delete(" << *expr << ")";
  }
private:
  Expr *expr;
};

class IfThenElseExpr: public Expr {
public:
  IfThenElseExpr(
    Expr *c, 
    Expr *e1, 
    Expr *e2 = nullptr
  ): cond(c), expr1(e1), expr2(e2) {}
  ~IfThenElseExpr() { delete cond; delete expr1; delete expr2; }
  virtual void printOn(std::ostream &out) const override {
    out << "IfThenElse(" << *cond << "," << *expr1;
    if (expr2 != nullptr) out << "," << *expr2;
    out << ")";
  }
private:
  Expr *cond;
  Expr *expr1;
  Expr *expr2;
};

class MatchExpr: public Expr {
public:
  MatchExpr(Expr *e, ClauseBlock *c): expr(e), block(c) {}
  ~MatchExpr() { delete expr; delete block; }
  virtual void sem() override {
    expr->sem();
    block->sem();
  }
  virtual void printOn(std::ostream &out) const override {
    out << "MatchExpr(" << *expr << "," << *block << ")";
  }
private:
  Expr *expr;
  ClauseBlock *block;
};

typedef Expr HighPrioExpr;

class DeRefHighPrioExpr: public HighPrioExpr {
public:
  DeRefHighPrioExpr(HighPrioExpr *e): expr(e) {}
  ~DeRefHighPrioExpr() { delete expr; }
  virtual void printOn(std::ostream &out) const override {
    out << "DeRefHighPrioExpr(" << *expr << ")";
  }
private:
  HighPrioExpr *expr;
};

class UnitHighPrioExpr: public HighPrioExpr {
public:
  UnitHighPrioExpr() {}
  virtual void printOn(std::ostream &out) const override {
    out << "UnitHighPrioExpr( unit )";
  }
private:

};

class IntHighPrioExpr: public HighPrioExpr {
public:
  IntHighPrioExpr(int v): val(v) {}
  virtual void sem() override {
    this->type = new SimpleType(INT);
  }
  virtual void printOn(std::ostream &out) const override {
    out << "IntHighPrioExpr(" << val << ")";
  }
private:
  int val;
};

class CharHighPrioExpr: public HighPrioExpr {
public:
  CharHighPrioExpr(char v): val(v) {}
  virtual void sem() override {
    this->type = new SimpleType(CHAR);
  }
  virtual void printOn(std::ostream &out) const override {
    out << "CharHighPrioExpr(" << val << ")";
  }
private:
  char val;
};

class StringHighPrioExpr: public HighPrioExpr {
public:
  StringHighPrioExpr(std::string s): val(s) {}
  virtual void printOn(std::ostream &out) const override {
    out << "StringHighPrioExpr(" << val << ")";
  }
private:
  std::string val;
};

class BoolHighPrioExpr: public HighPrioExpr {
public:
  BoolHighPrioExpr(bool v): val(v) {}
  virtual void printOn(std::ostream &out) const override {
    out << "BoolHighPrioExpr(" << (val ? "true" : "false") << ")";
  }
private:
  bool val;
};

class IdHighPrioExpr: public HighPrioExpr {
public:
  IdHighPrioExpr(std::string i, ExprBlock *c = nullptr): id(i), commaExprList(c) {}
  ~IdHighPrioExpr() { delete commaExprList; }
  virtual void printOn(std::ostream &out) const override {
    out << "IdHighPrioExpr(" << id;
    if (commaExprList != nullptr) out << "," << *commaExprList;
    out << ")";
  }

private:
  std::string id;
  ExprBlock *commaExprList;
};

class HighPrioExprBlock: public Expr {
public:
  HighPrioExprBlock(): block() {}
  ~HighPrioExprBlock() { for (HighPrioExpr *b : block) delete b; }
  void append(HighPrioExpr *d) { block.push_back(d); }
  virtual void printOn(std::ostream &out) const override {
    out << "ExprHighBlock(";
    bool first = true;
    for (HighPrioExpr *s : block) {
      if (!first) out << ", ";
      first = false;
      out << *s;
    }
    out << ")";
  }

private:
  std::vector<HighPrioExpr *> block;
  std::string id;
};

class StmtBlock: public Stmt {
public:
  StmtBlock(): stmt_list() {}
  ~StmtBlock() { for (Stmt *s : stmt_list) delete s; }
  void append(Stmt *s) { stmt_list.push_back(s); }
  virtual void sem() override {
    for (Stmt *s : stmt_list) s->sem();
  }
  virtual void printOn(std::ostream &out) const override {
    out << "StmtBlock(";
    bool first = true;
    for (Stmt *s : stmt_list) {
      if (!first) out << ", ";
      first = false;
      out << *s;
    }
    out << ")";
  }
private:
  std::vector<Stmt *> stmt_list;
};

class MutableDef: public Def {
public:
  MutableDef(std::string i, Type *t = nullptr): id(i), type(t) {}
  ~MutableDef() { delete type; }
  virtual void printOn(std::ostream &out) const override {
    out << "MutableDef(" << id;
    if (type != nullptr) out << "," << *type;
    out << ")";
  }

private:
  std::string id;
  Type *type;
};

class MutableArrayDef: public Def {
public:
  MutableArrayDef(
    std::string i, 
    ExprBlock *b, 
    Type *t = nullptr
  ): id(i), block(b), type(t) {}
  ~MutableArrayDef() { delete block; delete type; }
  virtual void printOn(std::ostream &out) const override {
    out << "MutableArrayDef(" << id << "," << *block;
    if (type != nullptr) out << "," << *type;
    out << ")";
  }

private:
  std::string id;
  ExprBlock *block;
  Type *type;
};

class Par: public AST {
public:
  Par(std::string i, Type *t = nullptr): id(i), type(t) {}
  ~Par() { delete type; }
  virtual void printOn(std::ostream &out) const override {
    out << "Par(" << id;
    if (type != nullptr) out << "," << *type;
    out << ")";
  }
private:
  std::string id;
  Type *type;
};

class ParBlock: public AST {
public:
  ParBlock(): block() {}
  ~ParBlock() { for (Par *b : block) delete b; }
  void append(Par *p) { block.push_back(p); }
  virtual void printOn(std::ostream &out) const override {
    out << "ParBlock(";
    bool first = true;
    for (Par *p : block) {
      if (!first) out << ", ";
      first = false;
      out << *p;
    }
    out << ")";
  }
private:
  std::vector<Par *> block;
};

class ImmutableDef: public Def {
public:
  ImmutableDef(
    std::string i,
    ParBlock *p,  
    Expr *e, 
    Type *t = nullptr
  ): id(i), exp(e), block(p), type(t) {}
  ~ImmutableDef() { delete exp; delete block; delete type; }
  virtual void sem() override {
    exp->sem();
    block->sem();
    // TODO: its not ready
  }
  virtual void printOn(std::ostream &out) const override {
    out << "ImmutableDef(" << id << "," << *exp << "," << *block;
    if (type != nullptr) out << "," << *type; 
    out << ")";
  }

private:
  std::string id;
  Expr *exp;
  ParBlock *block;
  Type *type;
};

class FunctionCall: public Expr {
public:
  FunctionCall(std::string id, HighPrioExprBlock *b): id(id), block(b) {}
  ~FunctionCall() { delete block; }
  virtual void printOn(std::ostream &out) const override {
    out << "FunctionCall(" << id << "," << *block << ")";
  }
private:
  std::string id;
  HighPrioExprBlock *block;
};

#endif
