#include "oneWayPersistent.hpp"
#include <iostream>
#include <fstream>

enum class string_code {
    UNDO,
    REDO,
    INSERT,
    DELETE,
    CONTAINS,

    INVALID
};

using enum string_code;

static string_code hashit (const std::string & inString) {
    if (inString == "u") return UNDO;
    if (inString == "r") return REDO;
    if (inString == "i") return INSERT;
    if (inString == "d") return DELETE;
    if (inString == "c") return CONTAINS;
    return INVALID;
}
int main() {
    persistent::Set<int> t;

    std::string opt;
    std::cin >> opt;
    while(opt != "e") {
        switch(hashit(opt)) {
            case UNDO: {
                t.undo();
                break;
            }
            case REDO: {
                t.redo();
                break;
            }
            case INSERT: {
                int tmp;
                std::cin >> tmp;
                t.insert(tmp);
                break;
            }
            case DELETE: {
                int tmp;
                std::cin >> tmp;
                t.remove(tmp);
                break;
            }
            case CONTAINS: {
                int tmp;
                std::cin >> tmp;
                std::cout << t.contains(tmp) << std::endl;
                break;
            }
            default:;
        }
        std::cin >> opt;
    }
    return 0;
}