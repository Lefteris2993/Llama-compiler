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

%token  T_const_int
%token<var>  T_const_string
%token  T_const_char
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

  std::string var;

}

%type<expr> expr
%type<stmtBlock> stmt_list
%type<def> def
%type<letdef> letdef
%type<defBlock> def_list
%type<exprHigh> expr_high
%type<exprHighBlock> expr_high_list

%%

program:
  stmt_list { 
    std::cout << "AST:" << *$1 << std::endl;
   }
;

stmt_list:
  /* nothing */ { $$ = new StmtBlock(); }
| stmt_list letdef { $1->append($2); $$ = $1; }
;

letdef:
  "let" def_list { $$ = new LetDef($2, false); }
| "let" "rec" def_list { $$ = new LetDef($3, true); }
;

def_list:
  def { DefBlock d = new DefBlock(); d.append($1); $$ = d; }
| def_list "and" def { $1->append($3); $$ = $1; }
;

def:
  T_id par_list '=' expr { $$ = new ImmutableDef($1, $4);  }
| T_id par_list ':' type '=' expr
| "mutable" T_id { $$ = new MutableDef($2); }
| "mutable" T_id ':' type
| "mutable" T_id '[' comma_expr_list ']'
| "mutable" T_id '[' comma_expr_list ']' ':' type
;

par_list:
  /* nothing */
| par par_list
;

par:
  T_id
| '(' T_id ':' type ')'
;

comma_expr_list:
  expr
| expr ',' comma_expr_list
;

type:
  "unit"
| "int"
| "char"
| "bool"
| '(' type ')'
| type "->" type
| type "ref"
| "array" '[' star_list ']' "of" type
;

star_list:
  '*'
| '*' ',' star_list
;

expr: 
  "not" expr
| '+' expr %prec INT_POS_SIGN
| '-' expr %prec INT_NEG_SIGN
| expr '+' expr
| expr '-' expr
| expr '*' expr
| expr '/' expr
| expr "mod" expr
| expr '=' expr
| expr "<>" expr
| expr '<' expr
| expr '>' expr
| expr "<=" expr
| expr ">=" expr
| expr "==" expr
| expr "!=" expr
| expr "&&" expr
| expr "||" expr
| expr ":=" expr
| expr ';' expr
| letdef "in" expr
| "while" expr "do" expr "done"
| "for" T_id '=' expr "to" expr "do" expr "done"
| "for" T_id '=' expr "downto" expr "do" expr "done"
| "dim" T_id
| "dim" T_const_int T_id
| "new" type
| "delete" expr
| T_id expr_high_list
| "if" expr "then" expr
| "if" expr "then" expr "else" expr
| "begin" expr "end"
| "match" expr "with" clause_list "end"
| expr_high
;

expr_high: 
  '!' expr_high
| '(' expr ')'
| '(' ')'
| T_const_int
| T_const_char
| T_const_string { $$ = $1 }
| "true"
| "false"
| T_id
| T_id '[' comma_expr_list ']'
;

expr_high_list: 
  expr_high { HighPrioExprBlock d = new HighPrioExprBlock(); d.append($1); $$ = d; }
| expr_high_list expr_high { $1->append($2); $$ = $1; }
;

clause_list:
  clause
| clause '|' clause_list
;

clause:
  pattern "->" expr
;

pattern:
  '+' T_const_int %prec INT_POS_SIGN
| '-' T_const_int %prec INT_NEG_SIGN
| T_const_int
| T_const_char
| "true"
| "false"
| T_id
| '(' pattern ')'
;

%%

void yyerror (const char * msg)
{
	fprintf(stderr,"error :  %s\n",msg);
	exit(1);
}

int main () {
  int result = yyparse();
  if (result == 0) printf("Success.\n");
  return result;
}
