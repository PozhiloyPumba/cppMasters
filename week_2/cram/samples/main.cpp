#include "interpreter.hpp"
#include <fstream>
#include <vector>

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "Give me code file" << std::endl;
        return 0;
    }
    
	std::vector<int> memory(4096, 0);
    CRAM::Interpreter interpeter(argv[1], memory);
	interpeter.run();
    return 0;
}