%{
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include "ast.hpp"
#include "lexer.hpp"
%}

%define parse.error verbose

%token  T_and   	              "and"
%token  T_array   	            "array"
%token  T_begin   	            "begin"
%token  T_bool                  "bool"
%token  T_char   	              "char"
%token  T_delete                "delete"
%token  T_dim   	              "dim"
%token  T_do                    "do"
%token  T_done   	              "done"
%token  T_downto                "downto"
%token  T_else   	              "else"
%token  T_end   	              "end"
%token  T_false   	            "false"
%token  T_for   	              "for"
%token  T_if   	                "if"
%token  T_in   	                "in"
%token  T_int   	              "int"
%token  T_let   	              "let"
%token  T_match   	            "match"
%token  T_mod   	              "mod"
%token  T_mutable               "mutable"
%token  T_new   	              "new"
%token  T_not   	              "not"
%token  T_of   	                "of"
%token  T_rec    	              "rec"
%token  T_ref                   "ref"
%token  T_then                  "then"
%token  T_to   	                "to"
%token  T_true   	              "true"
%token  T_unit   	              "unit"
%token  T_while                 "while"
%token  T_with	                "with"

%token  T_arrow                 "->"
%token  T_double_ampersand      "&&"
%token  T_double_vertical_line  "||"
%token  T_diff                  "<>"
%token  T_less_equal            "<="
%token  T_greater_equal         ">="
%token  T_equal                 "=="
%token  T_not_equal             "!="
%token  T_set                   ":="

%token<num>  T_const_int
%token<var>  T_const_string
%token<c>  T_const_char
%token<var>  T_id


%nonassoc "let" "in"
%left ';'
%nonassoc "if" "then"
%nonassoc "else"
%nonassoc ":="
%left "||"
%left "&&"
%nonassoc '=' "<>" '>' '<' "<=" ">=" "==" "!="
%left '+' '-'
%left '*' '/' "mod"
%nonassoc "not" "delete" INT_POS_SIGN INT_NEG_SIGN
%nonassoc '!'
%nonassoc "new"

%right "->"
%nonassoc "of"
%nonassoc "ref"

%union {
  Stmt *stmt;
  Expr *expr;
  LetDef *letdef;
  DefBlock *defBlock;
  StmtBlock *stmtBlock;
  Def *def;
  HighPrioExpr *exprHigh;
  HighPrioExprBlock *exprHighBlock;
  Pattern *pattern;
  Clause *clause;
  ClauseBlock *clauseBlock;
  ExprBlock *exprBlock;
  Type *type;
  ParBlock *parBlock;
  Par *par;
  std::string *var;
  int num;
  char c;
  unsigned un;
}

%type<expr> expr
%type<stmtBlock> stmt_list
%type<def> def
%type<letdef> letdef
%type<defBlock> def_list
%type<exprHigh> expr_high
%type<exprHighBlock> expr_high_list
%type<pattern> pattern
%type<clause> clause
%type<clauseBlock> clause_list
%type<exprBlock> comma_expr_list
%type<type> type
%type<un> star_list
%type<parBlock> par_list
%type<par> par

%%

program:
  stmt_list { 
    std::cout << "AST:" << *$1 << std::endl;
   }
;

stmt_list:
  /* nothing */     { $$ = new StmtBlock(); }
| stmt_list letdef  { $1->append($2); $$ = $1; }
;

letdef:
  "let" def_list        { $$ = new LetDef($2, false); }
| "let" "rec" def_list  { $$ = new LetDef($3, true); }
;

def_list:
  def                 { DefBlock *d = new DefBlock(); d->append($1); $$ = d; }
| def_list "and" def  { $1->append($3); $$ = $1; }
;

def:
  T_id par_list '=' expr                            { $$ = new ImmutableDef(*$1, $2, $4); }
| T_id par_list ':' type '=' expr                   { $$ = new ImmutableDef(*$1, $2, $6, $4); }
| "mutable" T_id                                    { $$ = new MutableDef(*$2); }
| "mutable" T_id ':' type                           { $$ = new MutableDef(*$2, $4); }
| "mutable" T_id '[' comma_expr_list ']'            { $$ = new MutableArrayDef(*$2, $4); }
| "mutable" T_id '[' comma_expr_list ']' ':' type   { $$ = new MutableArrayDef(*$2, $4, $7); }
;

par_list:
  /* nothing */ { $$ = new ParBlock(); }
| par_list par  { $1->append($2); $$ = $1; }
;

par:
  T_id                  { $$ = new Par(*$1); }
| '(' T_id ':' type ')' { $$ = new Par(*$2, $4); }
;

comma_expr_list:
  expr                      { ExprBlock *b = new ExprBlock(); b->append($1); $$ = b; }
| comma_expr_list ',' expr  { $1->append($3); $$ = $1; }
;

type:
  "unit"                                { $$ = new SimpleType(BaseType::UNIT); }
| "int"                                 { $$ = new SimpleType(BaseType::INT); }
| "char"                                { $$ = new SimpleType(BaseType::CHAR); }
| "bool"                                { $$ = new SimpleType(BaseType::BOOL); }
| '(' type ')'                          { $$ = $2; }
| type "->" type                        { $$ = new FunctionType($1, $3); }
| type "ref"                            { $$ = new RefType($1); }                    
| "array" '[' star_list ']' "of" type   { $$ = new ArrayType($3, $6); }
;

star_list:
  '*'               { $$ = 1; }
| star_list ',' '*' { $$ = $$ + 1; }
;

expr: 
  "not" expr                                            { $$ = new SigOpExpr(SigOp::NOT, $2); }
| '+' expr %prec INT_POS_SIGN                           { $$ = new SigOpExpr(SigOp::SIG_PLUS, $2); }
| '-' expr %prec INT_NEG_SIGN                           { $$ = new SigOpExpr(SigOp::SIG_MINUS, $2); }
| expr '+' expr                                         { $$ = new BinOpExpr($1, BinOp::BIN_PLUS, $3); }
| expr '-' expr                                         { $$ = new BinOpExpr($1, BinOp::BIN_MINUS, $3); }
| expr '*' expr                                         { $$ = new BinOpExpr($1, BinOp::STAR, $3); }
| expr '/' expr                                         { $$ = new BinOpExpr($1, BinOp::DIV, $3); }
| expr "mod" expr                                       { $$ = new BinOpExpr($1, BinOp::MOD, $3); }
| expr '=' expr                                         { $$ = new BinOpExpr($1, BinOp::STRUCT_EQ, $3); }
| expr "<>" expr                                        { $$ = new BinOpExpr($1, BinOp::STRUCT_NE, $3); }
| expr '<' expr                                         { $$ = new BinOpExpr($1, BinOp::L, $3); }
| expr '>' expr                                         { $$ = new BinOpExpr($1, BinOp::G, $3); }
| expr "<=" expr                                        { $$ = new BinOpExpr($1, BinOp::LE, $3); }
| expr ">=" expr                                        { $$ = new BinOpExpr($1, BinOp::GE, $3); }
| expr "==" expr                                        { $$ = new BinOpExpr($1, BinOp::EQ, $3); }
| expr "!=" expr                                        { $$ = new BinOpExpr($1, BinOp::NE, $3); }
| expr "&&" expr                                        { $$ = new BinOpExpr($1, BinOp::AND, $3); }
| expr "||" expr                                        { $$ = new BinOpExpr($1, BinOp::OR, $3); }
| expr ":=" expr                                        { $$ = new BinOpExpr($1, BinOp::ASS, $3); }
| expr ';' expr                                         { $$ = new BinOpExpr($1, BinOp::PAR, $3); }
| letdef "in" expr                                      { $$ = new LetInExpr($1, $3); }
| "while" expr "do" expr "done"                         { $$ = new WhileExpr($2, $4); }
| "for" T_id '=' expr "to" expr "do" expr "done"        { $$ = new ForExpr(true, *$2, $4, $6, $8); }
| "for" T_id '=' expr "downto" expr "do" expr "done"    { $$ = new ForExpr(false, *$2, $4, $6, $8); }
| "dim" T_id                                            { $$ = new DimExpr(*$2); }
| "dim" T_const_int T_id                                { $$ = new DimExpr(*$3, $2); }
| "new" type                                            { $$ = new NewExpr($2); }
| "delete" expr                                         { $$ = new DeleteExpr($2); }
| T_id expr_high_list                                   { $$ = new FunctionCall(*$1, $2); }
| "if" expr "then" expr                                 { $$ = new IfThenElseExpr($2, $4); }
| "if" expr "then" expr "else" expr                     { $$ = new IfThenElseExpr($2, $4, $6); }
| "begin" expr "end"                                    { $$ = $2; }
| "match" expr "with" clause_list "end"                 { $$ = new MatchExpr($2, $4); }
| expr_high                                             { $$ = $1; }
;

expr_high: 
  '!' expr_high                 { $$ = new DeRefHighPrioExpr($2); }            
| '(' expr ')'                  { $$ = $2; }
| '(' ')'                       { $$ = new UnitHighPrioExpr(); }
| T_const_int                   { $$ = new IntHighPrioExpr($1); }
| T_const_char                  { $$ = new CharHighPrioExpr($1); }
| T_const_string                { $$ = new StringHighPrioExpr(*$1); }
| "true"                        { $$ = new BoolHighPrioExpr(true); }
| "false"                       { $$ = new BoolHighPrioExpr(false); }
| T_id                          { $$ = new IdHighPrioExpr(*$1); }
| T_id '[' comma_expr_list ']'  { $$ = new IdHighPrioExpr(*$1, $3); }
;

expr_high_list: 
  expr_high                 { HighPrioExprBlock *d = new HighPrioExprBlock(); d->append($1); $$ = d; }
| expr_high_list expr_high  { $1->append($2); $$ = $1; }
;

clause_list:
  clause                  { ClauseBlock *b = new ClauseBlock(); b->append($1); $$ = b; }
| clause_list '|' clause  { $1->append($3); $$ = $1; }
;

clause:
  pattern "->" expr { $$ = new Clause($1, $3); }
;

pattern:
  '+' T_const_int %prec INT_POS_SIGN  { $$ = new NumPattern($2); }
| '-' T_const_int %prec INT_NEG_SIGN  { $$ = new NumPattern($2, true); }
| T_const_int                         { $$ = new NumPattern($1); }
| T_const_char                        { $$ = new NumPattern($1); }
| "true"                              { $$ = new BoolPattern(true); }
| "false"                             { $$ = new BoolPattern(false); }
| T_id                                { $$ = new VarPattern(*$1); }
| '(' pattern ')'                     { $$ = $2; }
;

%%

int main () {
  int result = yyparse();
  if (result == 0) printf("Success.\n");
  return result;
}
