#ifndef __LEXER_HPP__
#define __LEXER_HPP__

#include <sstream>
#include "tokens.hpp"

#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer TestQual_FlexLexer // the trick with prefix; no namespace here :(
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL TestQual::Lexem TestQual::Lexer::get_next_token()


namespace TestQual {

class Lexer : public yyFlexLexer {
public:
    Lexer(std::istringstream &input): yyFlexLexer(input, std::cout) {}
	virtual ~Lexer() {}
	virtual Lexem get_next_token();
};
}

#endif