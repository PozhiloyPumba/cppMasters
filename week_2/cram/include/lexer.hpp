#ifndef __LEXER_HPP__
#define __LEXER_HPP__

#include <istream>
#include <memory>
#include <vector>
#include "lexem.hpp"
#include "token.hpp"

#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer CRAM_FlexLexer // the trick with prefix; no namespace here :(
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL CRAM::Lexem CRAM::Lexer::get_next_token()

namespace CRAM {

class Lexer : public yyFlexLexer {
	virtual Lexem get_next_token();
	bool readContent = false;
	std::vector<std::vector<std::unique_ptr<Token>>> statements_;
	std::vector<std::unique_ptr<Token>> get_next_statement();

public:
    Lexer(std::istream &input): yyFlexLexer(input, std::cout) {}
	virtual ~Lexer() = default;
	void read();
};

}

#endif