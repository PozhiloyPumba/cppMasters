#ifndef __PERSISTENT_BST_HPP__
#define __PERSISTENT_BST_HPP__

#include <array>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <list>
#include <optional>
#include <functional>
#include <type_traits>

namespace persistent {

template<typename T>
class Set final {
    bool isUndo = false;
    std::optional<T> backlog_;
    std::set<T> tree_;

    enum {
        INSERT = 0,
        REMOVE = 1,
        INVALID = 2
    };

    const std::array<std::function<void(const T&)>, 3> lastOpFunc {
        [this](const T&val){ tree_.insert(val); },
        [this](const T&val){ tree_.erase(val); },
        []([[maybe_unused]]const T&val){}
    };

    std::function<void(const T&)> lastOp = lastOpFunc[INVALID];
    std::function<void(const T&)> prev = lastOpFunc[INVALID];

public:
    bool contains(const T &val) const;
    void remove(const T &val);
    void insert(const T &val);
    bool undo();
    bool redo();
    void dump() const;
};

#include "oneWayPersistent_impl.cpp"

template<>
class Set<std::string> {
    using strType = std::string;
    struct Node;
    struct Edge {
        std::string label_;
        Node *target_ = nullptr;
    };
    struct Node {
        // list because of erasing
        std::list<Edge> children_;
        bool isTerminal = false;
    };

    std::vector<Node *> changesOld_;
    std::vector<Node *> changesNew_;

    Node *newRoot_ = nullptr;
    Node *oldRoot_ = nullptr;
    Node *root_ = new Node;

    void dump(std::ostream &out, Node *root) const {
        if (!root)
            return;

        out << "digraph tree {\n"
                "rankdir = \"LR\"\n"
                "node [fontsize=10, shape=box, height=0.5]\n"
                "edge [fontsize=10]\n";

        PrintNodeIntoGraphviz(root, out);
        BuildConnectionsInGraphviz(root, out);

        out << "}\n";
    }

    void PrintNodeIntoGraphviz(Node *curNode, std::ostream &out) const {
        out << "\"" << curNode << "\" [label = \"";
        out << curNode->isTerminal;
        out << "\"]\n";

        for (auto it = curNode->children_.begin(); it != curNode->children_.end(); ++it) {
            Node *curChild = it->target_;
            if (curChild)
                PrintNodeIntoGraphviz(curChild, out);
        }
    }

    void BuildConnectionsInGraphviz(Node *curNode, std::ostream &out) const {        
        for (auto it = curNode->children_.begin(); it != curNode->children_.end(); ++it) {
            Node *curChild = it->target_;
            if (curChild)
                out << "\"" << curNode << "\" -> \"" << curChild << "\" [label=\"" << it->label_ << "\"]\n";
        }

        for (auto it = curNode->children_.begin(); it != curNode->children_.end(); ++it) {
            Node *curChild = it->target_;
            if (curChild)
                BuildConnectionsInGraphviz(curChild, out);
        }
    }
    void commit();
    void cleanup(Node *curNode);
 
public:
    bool contains(const strType &val) const;
    void remove(const strType &val);
    void insert(const strType &val);
    bool undo();
    bool redo();
    inline void dumpTree(std::ostream &out) const {dump(out, root_);}

    ~Set(){
        commit();
        cleanup(root_);
    }
};

}


#endif