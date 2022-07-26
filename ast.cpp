#include "ast.hpp"

extern Type *intType;
extern Type *unitType;
extern Type *charType;
extern Type *boolType;
extern Type *stringType;

AST::~AST() {}
void AST::setLineno(unsigned l) { this->lineno = l; }

Def::Def() {}
Def::~Def() {}

Type *Expr::getType() { return type; }

ExprBlock::ExprBlock(): block() {}
ExprBlock::~ExprBlock() { for (Expr *b : block) delete b; }
void ExprBlock::append(Expr *c) { block.push_back(c); }
void ExprBlock::printOn(std::ostream &out) const {
  out << "ExprBlock(";
  bool first = true;
  for (Expr *c : block) {
    if (!first) out << ",";
    first = false;
    out << *c;
  }
  out << ")";
}

NumPattern::NumPattern(int n, bool neg): num(n), negative(neg) {}
void NumPattern::printOn(std::ostream &out) const {
  out << "NumPattern(" << (negative ? "-" : "+") << "," << num << ")";
}

CharPattern::CharPattern(char c): var(c) {}
void CharPattern::printOn(std::ostream &out) const {
  out << "CharPattern(" << var << ")";
}

BoolPattern::BoolPattern(bool v): val(v) {}
void BoolPattern::printOn(std::ostream &out) const {
  out << "BoolPattern(" << (val ? "true" : "false") << ")";
}

VarPattern::VarPattern(std::string v): var(v) {}
void VarPattern::printOn(std::ostream &out) const {
  out << "VarPattern(" << var << ")";
}


Clause::Clause(Pattern *p, Expr *e): pattern(p), expr(e) {}
Clause::~Clause() { delete pattern; delete expr; }
void Clause::printOn(std::ostream &out) const {
  out << "Clause(" << *pattern << "," << *expr << ")";
}

Pattern *Clause::getPattern() { return pattern; }
Expr *Clause::getExpr() { return expr; }

ClauseBlock::ClauseBlock(): clauses() {}
ClauseBlock::~ClauseBlock() { for (Clause *c : clauses) delete c; }
void ClauseBlock::append(Clause *c) { clauses.push_back(c); }
void ClauseBlock::printOn(std::ostream &out) const {
  out << "ClauseBlock(";
  bool first = true;
  for (Clause *c : clauses) {
    if (!first) out << ",";
    first = false;
    out << *c;
  }
  out << ")";
}

BinOpExpr::BinOpExpr(Expr *l, BinOp p, Expr *r): lhs(l), rhs(r), op(p) {}
BinOpExpr::~BinOpExpr() { delete lhs; delete rhs; }
void BinOpExpr::printOn(std::ostream &out) const {
  out << "BinOp(" << op << "," << *lhs << "," << *rhs << ")";
}

SigOpExpr::SigOpExpr(SigOp p, Expr *e): expr(e), op(p) {}
SigOpExpr::~SigOpExpr() { delete expr; }
void SigOpExpr::printOn(std::ostream &out) const {
  out << "SigOp(" << op << "," << *expr << ")";
}

DefBlock::DefBlock(): block() {}
DefBlock::~DefBlock() { for (Def *b : block) delete b; }
void DefBlock::append(Def *d) { block.push_back(d); }
void DefBlock::printOn(std::ostream &out) const {
  out << "DefBlock(";
  bool first = true;
  for (Stmt *s : block) {
    if (!first) out << ",";
    first = false;
    out << *s;
  }
  out << ")";
}

LetDef::LetDef(DefBlock *b, bool rec): defBlock(b), rec(rec) {}
LetDef::~LetDef() { delete defBlock; }
void LetDef::printOn(std::ostream &out) const {
  out << "LetDef";
  if (rec) out << "Rec";
  out << "(";
  out << *defBlock;
  out << ")";
}

LetInExpr::LetInExpr(LetDef *l, Expr *e): let(l), expr(e) {}
LetInExpr::~LetInExpr() { delete let; delete expr; }
void LetInExpr::printOn(std::ostream &out) const {
  out << "LetInExpr(" << *let << "," << *expr << ")";
}

WhileExpr::WhileExpr(Expr *c, Expr *b): cond(c), body(b) {}
WhileExpr::~WhileExpr() { delete cond; delete body; }
void WhileExpr::printOn(std::ostream &out) const {
  out << "While(" << *cond << "," << *body << ")";
}

ForExpr::ForExpr(
  bool u, 
  std::string i, 
  Expr *q, 
  Expr *t, 
  Expr *b
): up(u), id(i), eq(q), to(t), body(b) {}
ForExpr::~ForExpr() { delete eq; delete to; delete body; }
void ForExpr::printOn(std::ostream &out) const {
  out << "For(" << (up ? "true" : "false") << "," << id << "," << *eq << "," << *to << "," << *body << ")";  
}

DimExpr::DimExpr(std::string i, unsigned c): id(i), dimention(c) {}
void DimExpr::printOn(std::ostream &out) const {
  out << "Dim(" << dimention << "," << id << ")";
}

NewExpr::NewExpr(Type *t): type(t) {}
NewExpr::~NewExpr() { delete type; }
void NewExpr::printOn(std::ostream &out) const {
  out << "New(" << *type << ")"; 
}

DeleteExpr::DeleteExpr(Expr *e): expr(e) {}
DeleteExpr::~DeleteExpr() { delete expr; }
void DeleteExpr::printOn(std::ostream &out) const {
  out << "Delete(" << *expr << ")";
}

IfThenElseExpr::IfThenElseExpr(
  Expr *c, 
  Expr *e1, 
  Expr *e2
): cond(c), expr1(e1), expr2(e2) {}
IfThenElseExpr::~IfThenElseExpr() { delete cond; delete expr1; delete expr2; }
void IfThenElseExpr::printOn(std::ostream &out) const {
  out << "IfThenElse(" << *cond << "," << *expr1;
  if (expr2 != nullptr) out << "," << *expr2;
  out << ")";
}

MatchExpr::MatchExpr(Expr *e, ClauseBlock *c): expr(e), block(c) {}
MatchExpr::~MatchExpr() { delete expr; delete block; }
void MatchExpr::printOn(std::ostream &out) const {
  out << "MatchExpr(" << *expr << "," << *block << ")";
}

DeRefHighPrioExpr::DeRefHighPrioExpr(HighPrioExpr *e): expr(e) {}
DeRefHighPrioExpr::~DeRefHighPrioExpr() { delete expr; }
void DeRefHighPrioExpr::printOn(std::ostream &out) const {
  out << "DeRefHighPrioExpr(" << *expr << ")";
}

void UnitHighPrioExpr::printOn(std::ostream &out) const {
  out << "UnitHighPrioExpr( unit )";
}

IntHighPrioExpr::IntHighPrioExpr(int v): val(v) {}
void IntHighPrioExpr::printOn(std::ostream &out) const {
  out << "IntHighPrioExpr(" << val << ")";
}


CharHighPrioExpr::CharHighPrioExpr(char v): val(v) {}
void CharHighPrioExpr::printOn(std::ostream &out) const {
  out << "CharHighPrioExpr(" << val << ")";
}

StringHighPrioExpr::StringHighPrioExpr(std::string s): val(s) {}
void StringHighPrioExpr::printOn(std::ostream &out) const {
  out << "StringHighPrioExpr(" << val << ")";
}

BoolHighPrioExpr::BoolHighPrioExpr(bool v): val(v) {}
void BoolHighPrioExpr::printOn(std::ostream &out) const {
  out << "BoolHighPrioExpr(" << (val ? "true" : "false") << ")";
}

IdHighPrioExpr::IdHighPrioExpr(std::string i, ExprBlock *c): id(i), commaExprList(c) {}
IdHighPrioExpr::~IdHighPrioExpr() { delete commaExprList; }
void IdHighPrioExpr::printOn(std::ostream &out) const {
  out << "IdHighPrioExpr(" << id;
  if (commaExprList != nullptr) out << "," << *commaExprList;
  out << ")";
}

HighPrioExprBlock::HighPrioExprBlock(): block() {}
HighPrioExprBlock::~HighPrioExprBlock() { for (HighPrioExpr *b : block) delete b; }
void HighPrioExprBlock::append(HighPrioExpr *d) { block.push_back(d); }
unsigned HighPrioExprBlock::getBlockLength() { return block.size(); } 
void HighPrioExprBlock::printOn(std::ostream &out) const {
  out << "HighPrioExprBlock(";
  bool first = true;
  for (HighPrioExpr *s : block) {
    if (!first) out << ",";
    first = false;
    out << *s;
  }
  out << ")";
}


StmtBlock::StmtBlock(): stmt_list() {}
StmtBlock::~StmtBlock() { for (Stmt *s : stmt_list) delete s; }
void StmtBlock::append(Stmt *s) { stmt_list.push_back(s); }
void StmtBlock::printOn(std::ostream &out) const {
  out << "StmtBlock(";
  bool first = true;
  for (Stmt *s : stmt_list) {
    if (!first) out << ",";
    first = false;
    out << *s;
  }
  out << ")";
}

MutableDef::MutableDef(std::string i, Type *t): id(i), type(t) {}
MutableDef::~MutableDef() { delete type; }
DefType MutableDef::getDefType() const { return DefType::DEF_MUTABLE; }
void MutableDef::printOn(std::ostream &out) const {
  out << "MutableDef(" << id;
  if (type != nullptr) out << "," << *type;
  out << ")";
}

MutableArrayDef::MutableArrayDef(
  std::string i, 
  ExprBlock *b, 
  Type *t
): id(i), block(b), type(t) {}
MutableArrayDef::~MutableArrayDef() { delete block; delete type; }
DefType MutableArrayDef::getDefType() const { return DefType::DEF_ARRAY; }
void MutableArrayDef::printOn(std::ostream &out) const {
  out << "MutableArrayDef(" << id << "," << *block;
  if (type != nullptr) out << "," << *type;
  out << ")";
}

Par::Par(std::string i, Type *t): id(i), type(t) {}
Par::~Par() { delete type; }
Type *Par::getType() { return type; }
void Par::printOn(std::ostream &out) const {
  out << "Par(" << id;
  if (type != nullptr) out << "," << *type;
  out << ")";
}

ParBlock::ParBlock(): block() {}
ParBlock::~ParBlock() { for (Par *b : block) delete b; }
void ParBlock::append(Par *p) { block.push_back(p); }
void ParBlock::printOn(std::ostream &out) const {
  out << "ParBlock(";
  bool first = true;
  for (Par *p : block) {
    if (!first) out << ",";
    first = false;
    out << *p;
  }
  out << ")";
}

ImmutableDef::ImmutableDef(
  std::string i,
  ParBlock *p,  
  Expr *e, 
  Type *t
): id(i), expr(e), block(p), type(t) {}
ImmutableDef::~ImmutableDef() { delete expr; delete block; delete type; }
DefType ImmutableDef::getDefType() const { return DefType::DEF_IMMUTABLE; }
void ImmutableDef::printOn(std::ostream &out) const {
  out << "ImmutableDef(" << id << "," << *expr << "," << *block;
  if (type != nullptr) out << "," << *type; 
  out << ")";
}

FunctionCall::FunctionCall(std::string id, HighPrioExprBlock *b): id(id), block(b) {}
FunctionCall::~FunctionCall() { delete block; }
void FunctionCall::printOn(std::ostream &out) const {
  out << "FunctionCall(" << id << "," << *block << ")";
}
