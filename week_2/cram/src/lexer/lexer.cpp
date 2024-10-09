#include "lexer.hpp"

namespace CRAM {

std::vector<std::unique_ptr<Token>> Lexer::get_next_statement() {
    std::vector<std::unique_ptr<Token>> tokens;
    CRAM::Lexem token;
    while(((token = get_next_token()) != Lexem::END_OF_INPUT) &&
            (token != Lexem::SEMICOLON)) {
		switch(token) {
			case Lexem::VAR:
				tokens.push_back(std::make_unique<Var>(yytext));
				break;
			case Lexem::NUMBER:
				tokens.push_back(std::make_unique<Number>(std::atoi(yytext)));
				break;
			default:
				tokens.push_back(std::make_unique<Token>(token));
		}
    }
    return tokens;
}

void Lexer::read() {
	if(readContent)
		return;

    std::vector<std::unique_ptr<Token>> tokens;

    while((tokens = get_next_statement()), tokens.size())
		statements_.push_back(std::move(tokens));

#ifndef NDEBUG
	for(const auto &st: statements_) {
		std::cout << "next" << std::endl;
		for(const auto &n: st) {
			std::cout << toString(n->getType());
			if(n->getType() == Lexem::VAR) {
				std::cout << " " << static_cast<const Var *>(n.get())->getName();
			}
			else if(n->getType() == Lexem::NUMBER) {
				std::cout << " " << static_cast<const Number *>(n.get())->getValue();
			}
			std::cout << std::endl;
		}
	}
#endif
	readContent = true;
}

}
