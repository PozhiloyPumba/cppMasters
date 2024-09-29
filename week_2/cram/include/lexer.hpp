#ifndef __LEXER_HPP__
#define __LEXER_HPP__

#include <istream>
#include <vector>
#include "tokens.hpp"

#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer CRAM_FlexLexer // the trick with prefix; no namespace here :(
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL CRAM::Lexem CRAM::Lexer::get_next_token()

namespace CRAM {

class Lexer : public yyFlexLexer {
public:
    Lexer(std::istream &input): yyFlexLexer(input, std::cout) {}
	virtual ~Lexer() {}
	virtual Lexem get_next_token();
	const std::vector<Lexem> get_next_statement();
};

}

#endif