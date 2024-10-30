#include "lexer.hpp"

namespace PM {

std::vector<std::unique_ptr<Token>> Lexer::read_next_statement() {
    std::vector<std::unique_ptr<Token>> tokens;
    Lexem token;
    while(((token = get_next_token()) != Lexem::END_OF_INPUT) &&
            (token != Lexem::SEMICOLON)) {
		switch(token) {
			case Lexem::VAR:
				tokens.push_back(std::make_unique<Var>(yytext));
				break;
			case Lexem::NUMBER:
				tokens.push_back(std::make_unique<Number>(std::atoi(yytext)));
				break;
			case Lexem::CL_BRACK:
				if(	(tokens.size() >= 2) && 
					(tokens[tokens.size() - 1]->getType() == Lexem::OP_BRACK) &&
					((tokens[tokens.size() - 2]->getType() == Lexem::OP_BRACK) ||
					 (tokens[tokens.size() - 2]->getType() == Lexem::VAR))) {
					tokens.push_back(std::make_unique<Token>(Lexem::EMPTY));
				}
			default:
				tokens.push_back(std::make_unique<Token>(token));
		}
    }
	if (token == Lexem::SEMICOLON) {
		tokens.push_back(std::make_unique<Token>(Lexem::SEMICOLON));
		// don't ask me why? but this very important
		tokens.push_back(std::make_unique<Token>(Lexem::SEMICOLON));
	}
    return tokens;
}

void Lexer::read() {
	if(readContent)
		return;

    std::vector<std::unique_ptr<Token>> tokens;

    while((tokens = read_next_statement()), tokens.size()) {
		if(tokens.back()->getType() != Lexem::SEMICOLON)
			throw std::runtime_error("no semicolon at the end");
		statements_.push_back(std::move(tokens));
	}

#ifndef NDEBUG
	std::cout << "Lexer output:" << std::endl;
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
