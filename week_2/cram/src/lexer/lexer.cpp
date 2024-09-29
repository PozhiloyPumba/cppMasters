#include "lexer.hpp"

namespace CRAM {

const std::vector<Lexem> Lexer::get_next_statement() {
    std::vector<Lexem> tokens;
    CRAM::Lexem token;
    while(((token = get_next_token()) != Lexem::END_OF_INPUT) &&
            (token != Lexem::SEMICOLON)) {
        tokens.push_back(token);
    }
    return tokens;
}

}
