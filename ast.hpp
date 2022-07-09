#ifndef __AST_HPP__
#define __AST_HPP__

#include <iostream>
#include <vector>
#include <string>

enum BaseType { UNIT, INT, CHAR, BOOL };


class AST {
public:
  // virtual ~AST() {}
  virtual void printOn(std::ostream &out) const = 0;
};

inline std::ostream& operator<< (std::ostream &out, const AST &t) {
  t.printOn(out);
  return out;
}

class Stmt : public AST {
  public:
    virtual void run() const = 0;
};

class Expr: public AST {
  public:
    virtual void eval() const = 0;
};

class BinOpExpr: public Expr {

};

class SigOpExpr: public Expr {

};

class HighPrioExpr: public Expr {
public:
  HighPrioExpr(std::string s): val(s) {}
  virtual void printOn(std::ostream &out) const override {
    out << "expr_high(" << val << ")";
  }
private:
  std::string val;
};

class Def: public Stmt {
public:
  Def() {}
private:
};

class HighPrioExprBlock: public Expr {
public:
  HighPrioExprBlock(): block() {}
  void append(Def *d) { block.push_back(d); }
  virtual void printOn(std::ostream &out) const override {
    out << "expr_high_block(";
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
  std::string id;
};

class StmtBlock: public Stmt {
public:
  StmtBlock(): stmt_list() {}
  ~StmtBlock() { for (Stmt *s : stmt_list) delete s; }
  void append(Stmt *s) { stmt_list.push_back(s); }
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
  // virtual void run() const override {
  //   for (Stmt *s : stmt_list)
  //     s->run();
  // }
private:
  std::vector<Stmt *> stmt_list;
};

class MutableDef: public Def {
public:
  MutableDef(std::string i): id(i) {}
  virtual void printOn(std::ostream &out) const override {
    out << "mut_def(" << id << ")";
  }

private:
  std::string id;
};

class ImmutableDef: public Def {
public:
  ImmutableDef(std::string i, Expr *e): id(i), exp(e) {}
  virtual void printOn(std::ostream &out) const override {
    out << "im_def(" << id << *exp << ")";
  }

private:
  std::string id;
  Expr *exp;
};

class DefBlock: public Stmt {
public:
  DefBlock(): block() {}
  void append(Def *d) { block.push_back(d); }
  virtual void printOn(std::ostream &out) const override {
    out << "def_bock(";
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
  LetDef(std::vector<DefBlock *> b, bool rec): defBlock(b), rec(rec) {}
  virtual void printOn(std::ostream &out) const override {
    out << "def_bock_";
    if (rec) out << "rec";
    out << "(";
    bool first = true;
    for (Stmt *s : defBlock) {
      if (!first) out << ", ";
      first = false;
      out << *s;
    }
    out << ")";
  }

private:
  std::vector<DefBlock *> defBlock;
  bool rec;
};


#endif
