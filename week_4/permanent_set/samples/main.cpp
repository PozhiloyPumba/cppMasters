#include "oneWayPersistent.hpp"
#include <iostream>

int main() {
    persistent::Set<int> t;
    t.undo();
    t.insert(5);
    t.dump();
    t.undo();
    t.dump();
    std::cout << t.contains(5) << std::endl;
    t.insert(3);
    t.dump();
    t.insert(2);
    t.dump();
    std::cout << t.contains(2) << std::endl;
    t.redo();
    t.dump();
    std::cout << t.contains(2) << std::endl;
    t.insert(4);
    t.dump();
    t.undo();
    t.dump();
    t.insert(9);
    t.dump();
    t.insert(7);
    t.dump();
    std::cout << t.contains(4) << std::endl;
    return 0;
}