%option noyywrap c++ nodefault
%option yyclass="Lexer"
%option prefix="TestQual_"

%{
    #include <iostream>
    #include <string>
    #include "lexer.hpp"
    #include "tokens.hpp"
%}

%%

[ \t\r\n]+ { /* ignore trash */ }

"const" { return Lexem::CONST; }
"char"  { return Lexem::CHAR; }
"*"     { return Lexem::POINTER; }
"[]"    { return Lexem::ARRAY; }
<<EOF>> { return Lexem::END_OF_INPUT; }
.       { throw std::runtime_error("Unexpected lexem:" + std::string(yytext)); return Lexem::ERROR;}

%%