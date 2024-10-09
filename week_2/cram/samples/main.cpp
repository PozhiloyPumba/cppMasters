#include "lexer.hpp"
#include <fstream>
#include <vector>
#include "lexem.hpp"

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
	l.read();
    f.close();

	

    return 0;
}