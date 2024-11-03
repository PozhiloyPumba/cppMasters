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

namespace impl
{
    template<typename T>
    inline constexpr bool is_string_class_decayed = false;

    template<typename... T>
    inline constexpr bool is_string_class_decayed<std::basic_string<T...>> = true;
}

template<typename T>
concept is_string_class = impl::is_string_class_decayed<std::decay_t<T>>;
template<typename T>
concept is_not_string_class = !impl::is_string_class_decayed<std::decay_t<T>>;

template<typename T>
class Set final {
};

template<is_not_string_class T>
class Set<T> final {
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

template<is_string_class T>
class Set<T> {
    template<class>
    struct Extract;

    template<template<class...> class V, class First, class... Rest>
    struct Extract<V<First, Rest...>> {
        using type = First;
    };

    template<class T_>
    struct TestWrapper {
        typename Extract<T_>::type extradata;
    };

    using string_view = std::basic_string_view<typename Extract<T>::type>;

    struct Node;
    struct Edge {
        T label_;
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

    template<typename Out>
    void dump(Out &out, Node *root) const {
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

    template<typename Out>
    void PrintNodeIntoGraphviz(Node *curNode, Out &out) const {
        out << "\"" << curNode << "\" [label = \"";
        out << curNode->isTerminal;
        out << "\"]\n";

        for (auto it = curNode->children_.begin(); it != curNode->children_.end(); ++it) {
            Node *curChild = it->target_;
            if (curChild)
                PrintNodeIntoGraphviz(curChild, out);
        }
    }

    template<typename Out>
    void BuildConnectionsInGraphviz(Node *curNode, Out &out) const {        
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
    bool contains(const T &val) const;
    void remove(const T &val);
    void insert(const T &val);
    bool undo();
    bool redo();

    template<typename Out>
    inline void dumpTree(Out &out) const {dump(out, root_);}

    ~Set(){
        commit();
        cleanup(root_);
    }
};

}

#include "oneWayPersistent_notString.cpp"
#include "oneWayPersistent_string.cpp"

#endif