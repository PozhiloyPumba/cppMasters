#include "lexer.hpp"
#include <fstream>
#include <vector>
#include "tokens.hpp"

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "Give me code file" << std::endl;
        return 0;
    }

    std::ifstream f(argv[1]);
    if(!f) {
        std::cerr << "file not opened: " << argv[1] << std::endl;
        return 0;
    }
    
    CRAM::Lexer l(f);
    std::vector<CRAM::Lexem> tokens;

    while((tokens = l.get_next_statement()), tokens.size()) {
        std::cout << "next statement" << std::endl;
        for(const auto token: tokens) {
            std::cout << CRAM::toString(token) << std::endl;
        }
    }

    f.close();
    return 0;
}