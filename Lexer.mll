{
  open Printf

  let create_hashtable size init =
    let tbl = Hashtbl.create size in
    List.iter (fun (key, data) -> Hashtbl.add tbl key data) init;
    tbl

  type token =
    | AND
    | ARRAY
    | BEGIN
    | BOOL
    | CHAR
    | DELETE
    | DIM
    | DO 
    | DONE
    | DOWNTO
    | ELSE
    | END
    | FALSE
    | FLOAT
    | FOR
    | IF
    | IN
    | INT
    | LET
    | MATCH
    | MOD
    | MUTABLE
    | NEW
    | NOT
    | OF 
    | REC
    | REF
    | THEN
    | TO
    | TRUE
    | TYPE
    | UNIT
    | WHILE
    | WITH
    | ARROW
    | ASSIGN
    | VERTICAL_LINE
    | PLUS
    | MINUS
    | STAR
    | SLASH
    | PLUS_DOT
    | MINUS_DOT
    | STAR_DOT
    | SLASH_DOT
    | DOUBLE_STAR
    | EXCLAMATION_MARK
    | SEMI_COLON
    | DOUBLE_AMPERSAND
    | DOUBLE_VERTICAL_LINE
    | DIFF
    | LESS
    | GREATER
    | LESS_EQUAL
    | GREATER_EQUAL
    | EQUAL
    | NOT_EQUAL
    | SET
    | PAR_OPEN
    | PAR_CLOSE
    | BRACKET_OPEN
    | BRACKET_CLOSE
    | COLON
    | COMMA
    | NUMBER of int
    | STRING of string
    | ID of string
    | EOF

  let keyword_table = 
    create_hashtable 35 [
      ("and", AND);
      ("array", ARRAY);
      ("begin", BEGIN);
      ("bool", BOOL);
      ("char", CHAR);
      ("delete", DELETE);
      ("dim", DIM);
      ("do", DO);
      ("done", DONE);
      ("downto", DOWNTO);
      ("else", ELSE);
      ("end", END);
      ("false", FALSE);
      ("float", FLOAT);
      ("for", FOR);
      ("if", IF);
      ("in", IN);
      ("int", INT);
      ("let", LET);
      ("match", MATCH);
      ("mod", MOD);
      ("mutable", MUTABLE);
      ("new", NEW);
      ("not", NOT);
      ("of", OF);
      ("rec", REC);
      ("ref", REF);
      ("then", THEN);
      ("to", TO);
      ("true", TRUE);
      ("type", TYPE);
      ("unit", UNIT);
      ("while", WHILE);
      ("with", WITH);
    ]

  let operators_table =
    create_hashtable 23 [
      ("->", ARROW);
      ("=", ASSIGN);
      ("|", VERTICAL_LINE);
      ("+", PLUS);
      ("-", MINUS);
      ("*", STAR);
      ("/", SLASH);
      ("+.", PLUS_DOT);
      ("-.", MINUS_DOT);
      ("*.", STAR_DOT);
      ("/.", SLASH_DOT);
      ("**", DOUBLE_STAR);
      ("!", EXCLAMATION_MARK);
      (";", SEMI_COLON);
      ("&&", DOUBLE_AMPERSAND);
      ("||", DOUBLE_VERTICAL_LINE);
      ("<>", DIFF);
      ("<", LESS);
      (">", GREATER);
      ("<=", LESS_EQUAL);
      (">=", GREATER_EQUAL);
      ("==", EQUAL);
      ("!=", NOT_EQUAL);
      (":=", SET);
    ]

    let separators_table =
    create_hashtable 6 [
      ('(', PAR_OPEN);
      (')', PAR_CLOSE);
      ('[', BRACKET_OPEN);
      (']', BRACKET_CLOSE);
      (':', COLON);
      (',', COMMA)
    ]
}

let digit = ['0'-'9']
let id = ['a'-'z' 'A'-'Z'] ['a'-'z' 'A'-'Z' '0'-'9' '_']*
let white  = [' ' '\t' '\n']

rule lexer = parse 
  | digit+ as inum 
    {
      printf "integer: %s\n" inum;
      NUMBER (int_of_string inum)
    }
  | '\"'[^'\n' '\"']*'\"' as str
    { 
      let str = String.sub str 1 ((String.length str )- 2) in 
      let str = Scanf.unescaped (str) in
      printf "string: %s\n" str;
      STRING (str)
    }
  | '\'' '\\' ['n' 'r' '0' 't' '\\' '\'' '\"'] '\''
  | id as word
    {
      try
        let token = Hashtbl.find keyword_table word in
        printf "keyword: %s\n" word;
        token
      with Not_found ->
        printf "identifier: %s\n" word;
        ID word
    }
  | "->" | "=" | "|" | "+" | "-" | "*" | "/" | "+." | "-." | "*." | "/." | "**" | "!" | ";" | "&&" | "||" | "<>" | "<" | ">" | "<=" | ">=" | "==" | "!=" | ":=" as op
    {
      printf "operator: %s\n" op;
      let token = Hashtbl.find operators_table op in
      token
    }
  | '(' | ')' | '[' | ']' | ',' | ':' as sep
    {
      printf "separator: %c\n" sep;
      let token = Hashtbl.find separators_table sep in
      token
    }
  | "--" [^'\n']*
    {
      (* Ignore comments *)
      lexer lexbuf
    }
  | '\n'
    {
      (* Ignore end of line *)
      lexer lexbuf
    }
  | white+ as ws
    {
      (* Ignore white spaces, tabs and return characters *)
      lexer lexbuf
    }
  | _ as c
    { 
      printf "Unrecognized character: %c\n" c;
      CHAR
    }
  | eof
    {
      raise End_of_file
    }

and comments level = parse
  | "(*"
    {
      if level = 0 then lexer lexbuf
      else comments (level - 1) lexbuf
    }
  | "*)"
    {
      comments (level + 1) lexbuf
    }
  | '\n'
    {
      comments level lexbuf
    }
  | _
    {
      comments level lexbuf
    }
  | eof 
    {
      printf "Unexpected EOF";
      raise End_of_file
    }

{
  let rec parse lexbuf = 
    let token = lexer lexbuf in 
    parse lexbuf
  
  let main () = 
    let cin = 
      if Array.length Sys.argv > 1
      then open_in Sys.argv.(1)
      else stdin
    in
    let lexbuf = Lexing.from_channel cin in 
    try parse lexbuf
    with End_of_file -> ()

  let _ = Printexc.print main ()
}