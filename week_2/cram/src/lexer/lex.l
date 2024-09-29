%option noyywrap c++ nodefault nounistd
%option yyclass="Lexer"
%option prefix="CRAM_"

%{
    #include <iostream>
    #include <string>
    #include "lexer.hpp"
    #include "tokens.hpp"
%}

%%

[ \t\r\n]+ { /* ignore trash */ }

"["                         { return Lexem::OP_BRACK; }
"]"                         { return Lexem::CL_BRACK; }
";"                         { return Lexem::SEMICOLON; }
"+"                         { return Lexem::ADD; }
"-"                         { return Lexem::MINUS; }
"*"                         { return Lexem::MULT; }
"/"                         { return Lexem::DIV; }
"="                         { return Lexem::ASSIGN; }
"input"                     { return Lexem::INPUT; }
"print"                     { return Lexem::PRINT; }
[A-Za-z][A-Za-z0-9_,.-]*    { return Lexem::VAR; }
[0-9]+                      { return Lexem::NUMBER; }
<<EOF>>                     { return Lexem::END_OF_INPUT; }
.       { throw std::runtime_error("Unexpected lexem:" + std::string(yytext)); return Lexem::ERROR;}

%%