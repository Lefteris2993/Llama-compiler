%{
#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <string>
#include <cstdlib>

#define   T_eof                     0

int comment_level = 0;
int line_number = 0;

%}

D    [0-9]
W     [ \t\r]
LO    [a-z]
UP    [A-Z]
L     {LO}|{UP}
E    \\[ntr0\\\'\"]

%option noyywrap

%x COMMENT 
%%

"and"           { return T_and; }
"array"         { return T_array; }
"begin"         { return T_begin; }
"bool"          { return T_bool; }  
"char"          { return T_char; }
"delete"        { return T_delete; }
"dim"           { return T_dim; }
"do"            { return T_do; }
"done"          { return T_done; }
"downto"        { return T_downto; }
"else"          { return T_else; }
"end"           { return T_end; }
"false"         { return T_false; }
"for"           { return T_for; }
"if"            { return T_if; }
"in"            { return T_in; }
"int"           { return T_int; }
"let"           { return T_let; }
"match"         { return T_match; }
"mod"           { return T_mod; }
"mutable"       { return T_mutable; }
"new"           { return T_new; }
"not"           { return T_not; }
"of"            { return T_of; }
"rec"           { return T_rec; }
"ref"           { return T_ref; }
"then"          { return T_then; }
"to"            { return T_to; }
"true"          { return T_true; }
"unit"          { return T_unit; }
"while"         { return T_while; }
"with"          { return T_with; }

"->"            { return T_arrow; }
"&&"            { return T_double_ampersand; }
"||"            { return T_double_vertical_line; }
"<>"            { return T_diff; }
"<="            { return T_less_equal; }
">="            { return T_greater_equal; }
"=="            { return T_equal; }
"!="            { return T_not_equal; }
":="            { return T_set; }

[\=\|\+\-\*\/\!\;\<\>\(\)\[\]\,\:]  { return yytext[0]; }
{D}+                { return T_const_int; }
\"([^\n"])*\"        { std::string str(yytext); yylval.var = str; return T_const_string; } 
\'([^\'\\\"]|{E})\' { return T_const_char; }
{LO}(_|{L}|{D})*    { std::string str(yytext); yylval.var = str; return T_id; }
%[^\n]*\n           { line_number = line_number + 1 ;}

\-\-.*              { /* ignore the rest of the line */ }

"(*"                { comment_level++; BEGIN(COMMENT); }
<COMMENT>"(*"       { comment_level++; }
<COMMENT>"*)"       { comment_level--; if(!comment_level) BEGIN(INITIAL); }
<COMMENT>\n         { line_number++; }
<COMMENT>\*         { }
<COMMENT>\(          { }
<COMMENT>[^(*\n]+   { }

{W}                 { }
\n                  { line_number = line_number + 1; }
<<EOF>>             { return T_eof; }
.                   { yyerror("illegal character"); }

%%
