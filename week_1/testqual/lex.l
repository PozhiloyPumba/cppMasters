%option noyywrap
%option noinput

%{
#include "tokens.hpp"
#include <iostream>
#include <string>

#define YY_DECL Lexem yylex(void)
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

void scan_string(const char* str)
{
    yy_switch_to_buffer(yy_scan_string(str));
}