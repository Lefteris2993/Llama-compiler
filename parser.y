%{
#include "lexer.hpp"
#include <cstdio>
#include <cstdlib>
%}

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
%token  T_float   	            "float"
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
%token  T_type    	            "type"
%token  T_unit   	              "unit"
%token  T_while                 "while"
%token  T_with	                "with"

%token  T_arrow                 "->"
%token  T_plus_dot              "+." 
%token  T_minus_dot             "-."
%token  T_star_dot              "*."
%token  T_slash_dot             "/."
%token  T_double_star           "**"
%token  T_double_ampersand      "&&"
%token  T_double_vertical_line  "||"
%token  T_diff                  "<>"
%token  T_less_equal            "<="
%token  T_greater_equal         ">="
%token  T_equal                 "=="
%token  T_not_equal             "!="
%token  T_set                   ":="

%token  T_const_int
%token  T_const_string
%token  T_const_char
%token  T_constructor
%token  T_id


%nonassoc "let" "in"
%left ';'
%nonassoc "if" "then"
%nonassoc "else"
%nonassoc ":="
%left "||"
%left "&&"
%nonassoc '=' "<>" '>' '<' "<=" ">=" "==" "!="
%left '+' '-' "+." "-."
%left '*' '/' "*." "/." "mod"
%right "**"
%nonassoc "not" "delete"
%nonassoc '!'
%nonassoc "new"

%right "->"
%nonassoc "of"
%nonassoc "ref"

%%

program:
  stmt_list
;

stmt_list:
  /* nothing */
| letdef stmt_list
| typedef stmt_list
;

letdef:
  "let" def_list
| "let" "rec" def_list
;

def_list:
  def
| def "and" def_list
;

def:
  T_id par_list '=' expr
| T_id par_list ':' type '=' expr
| "mutable" T_id
| "mutable" T_id ':' type
| "mutable" T_id '[' comma_expr_list ']'
| "mutable" T_id '[' comma_expr_list ']' ':' type
;

par_list:
  /* nothing */
| par par_list
;

comma_expr_list:
  expr
| expr ',' comma_expr_list
;

typedef:
  "type" tdef typedef_list
;

typedef_list:
  tdef
| "and" tdef typedef_list
;

tdef:
  T_id '=' constr_list
;

constr_list:
  constr
| '|' constr constr_list
;

constr:
  T_constructor
| T_constructor "of" type_list
;

type_list:
  type
| type type_list
;

par:
  T_id
| '(' T_id ':' type ')'
;

type:
  "unit"
| "int"
| "char"
| "bool"
| "float"
| '(' type ')'
| type "->" type
| type "ref"
| "array" '[' star_list ']' "of" type
| T_id
;

star_list:
  '*'
| '*' ',' star_list
;

expr:
  T_const_int
| T_const_char
| T_const_string
| "true"
| "false"
| '(' ')'
| '(' expr ')'
| unop expr
| expr binop expr
| T_id expr_list
| T_constructor expr_list
| T_id '[' comma_expr_list ']'
| "dim" T_const_int T_id
| "dim" T_id
| "new" type
| "delete" expr
| letdef "in" expr
| "begin" expr "end"
| "if" expr "then" expr
| "if" expr "then" expr "else" expr
| "while" expr "do" expr "done"
| "for" T_id '=' expr "to" expr "do" expr "done"
| "for" T_id '=' expr "downto" expr "do" expr "done"
| "match" expr "with" clause_list "end"
;

expr_list:
  /* nothing */
| expr expr_list
;

clause_list:
  clause
| clause '|' clause_list
;

unop:
  '+'
| '-'
| "+."
| "-."
| '!'
| "not"
;

binop:
  '+'
| '-'
| '*'
| '/'
| "+."
| "-."
| "*."
| "/."
| "mod"
| "**"
| '='
| "<>"
| '<'
| '>'
| "<="
| ">="
| "=="
| "!="
| "&&"
| "||"
| ';'
| ":="
;

clause:
  pattern "->" expr
;

pattern:
  '+' T_const_int
| '-' T_const_int
| T_const_char
| "true"
| "false"
| T_id
| '(' pattern ')'
| T_constructor pattern_list
;

pattern_list:
  /* nothing */
| pattern_list pattern
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
