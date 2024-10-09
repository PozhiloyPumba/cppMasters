#ifndef __TOKENS_HPP__
#define __TOKENS_HPP__

#include "genEnum.hpp"

namespace CRAM {

#define ENUM_LEXEMS(c) \
    c(OP_BRACK) \
    c(CL_BRACK) \
    c(SEMICOLON) \
    c(INPUT) \
    c(PRINT) \
    c(VAR) \
    c(NUMBER) \
    c(ASSIGN) \
    c(ADD) \
    c(MINUS) \
    c(MULT) \
    c(DIV) \
    c(END_OF_INPUT) \
    c(ERROR)

CREATE_ENUM(Lexem, ENUM_LEXEMS);

}


#endif