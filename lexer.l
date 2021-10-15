%{

#define		T_eof			                0
#define		T_and			                1001
#define		T_array			              1002
#define		T_begin			              1003
#define		T_bool		                1004
#define		T_char			              1005
#define		T_delete		              1006
#define		T_dim			                1007
#define		T_do		                  1008
#define		T_done			              1009
#define		T_downto		              1010
#define		T_else			              1011
#define		T_end			                1012
#define		T_false			              1013
#define		T_float			              1014
#define		T_for			                1015
#define		T_if			                1016
#define		T_in			                1017
#define		T_int			                1018
#define 	T_let			                1019
#define		T_match			              1020
#define		T_mod			                1021
#define		T_mutable		              1022
#define		T_new			                1023
#define		T_not			                1024
#define		T_of			                1025
#define		T_rec 			              1026
#define		T_ref		                  1027
#define  	T_then		                1028
#define		T_to			                1029
#define		T_true			              1030
#define		T_type 			              1031
#define		T_unit			              1032
#define		T_while		                1033
#define		T_with	                  1034

#define   T_arrow                   1035
#define   T_plus_dot                1036 
#define   T_minus_dot               1037
#define   T_star_dot                1038
#define   T_slash_dot               1039
#define   T_double_star             1040
#define   T_double_ampersand        1041
#define   T_double_vertical_line    1042
#define   T_diff                    1043
#define   T_less_equal              1044
#define   T_greater_equal           1045
#define   T_equal                   1046
#define   T_not_equal               1047
#define   T_set                     1048

#define   T_const_int               1049
#define   T_const_string            1050
#define   T_const_char              1051
#define   T_constructor             1052
#define   T_id                      1053

int comment_level = 0;
int line_number = 0;

%}

D 		[0-9]
W		  [ \t\r]
LO    [a-z]
UP    [A-Z]
L		  {LO}|{UP}
E 		\\[ntr0\\\'\"]


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
"float"         { return T_float; }
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
"type"          { return T_type; }
"unit"          { return T_unit; }
"while"         { return T_while; }
"with"          { return T_with; }

"->"            { return T_arrow; }
"+."            { return T_plus_dot; }
"-."            { return T_minus_dot; }
"*."            { return T_star_dot; }
"/."            { return T_slash_dot; }
"**"            { return T_double_star; }
"&&"            { return T_double_ampersand; }
"||"            { return T_double_vertical_line; }
"<>"            { return T_diff; }
"<="            { return T_less_equal; }
">="            { return T_greater_equal; }
"=="            { return T_equal; }
"!="            { return T_not_equal; }
":="            { return T_set; }

[\=\|\+\-\*\/\!\;\<\>\(\)\[\]\,\:]					{ return yytext[0]; }
{D}+                { return T_const_int; }
\"([^\n"])*\"	      { return T_const_string; } 
\'([^\'\\\"]|{E})\' { return T_const_char; }
{LO}(_|{L}|{D})*    { return T_id; }
{UP}(_|{L}|{D})*    { return T_constructor; }
%[^\n]*\n           { line_number = line_number + 1 ;}

\-\-.*              { /* ignore the rest of the line */ }

"(*"                { comment_level = comment_level + 1; BEGIN(COMMENT); }
<COMMENT>"(*"       { comment_level = comment_level + 1; }
<COMMENT>"*)"       { comment_level = comment_level - 1; if(!comment_level) BEGIN(INITIAL); }
<COMMENT>\*         { }
<COMMENT><          { }
<COMMENT>[^(*\n]+   { }

{W}                 { }
\n                  { line_number = line_number + 1; }
<<EOF>>             { return T_eof; }
.                   { fprintf(stderr, "Error: Found character %c with code %d in line %d.\n", yytext[0]>=32 ? yytext[0]:'?', yytext[0], line_number); exit(1); }

%%


int main () {
	int token;
	do {
		token = yylex();
		printf("token=%d, lexeme=\"%s\"\n", token, yytext);
	} while (token != T_eof );
}
