#ifndef __TOKENS_HPP__
#define __TOKENS_HPP__

namespace CRAM {

#define ENUM_MEMBERS(c) \
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

#define GEN_ENUM(e) e,

enum class Lexem {
    ENUM_MEMBERS(GEN_ENUM)
};

static const char *toString(Lexem l) {
    switch(l) {

#define ENUM_TO_STRING(e) \
    case Lexem::e: return #e;

    ENUM_MEMBERS(ENUM_TO_STRING)
    default:;
    }

#undef ENUM_TO_STRING
}

}

#endif