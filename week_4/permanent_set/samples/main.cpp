#include "oneWayPersistent.hpp"
#include <iostream>
#include <fstream>

int main() {
    // std::ofstream file;
    // file.open("test.dot");
    persistent::Set<std::string> t2;
    t2.insert("abc");
    t2.undo();
    t2.insert("ab");
    t2.insert("abd");
    t2.insert("aaa");
    t2.undo();
    t2.insert("bim");
    t2.insert("bam");
    t2.insert("bimbim");
    t2.insert("bambam");
    t2.undo();

    // t2.dumpTree(file);
    std::cout << t2.contains("bim") << std::endl;
    // file.close();

    persistent::Set<int> t;
    t.insert(5);
    t.undo();
    std::cout << t.contains(5) << std::endl;
    t.insert(3);
    t.insert(2);
    std::cout << t.contains(2) << std::endl;
    t.redo();
    std::cout << t.contains(2) << std::endl;
    t.insert(4);
    t.undo();
    t.insert(9);
    t.insert(7);
    std::cout << t.contains(4) << std::endl;
    return 0;
}