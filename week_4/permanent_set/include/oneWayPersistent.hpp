#ifndef __PERSISTENT_BST_HPP__
#define __PERSISTENT_BST_HPP__

#include <array>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <optional>
#include <functional>

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
    inline void dump() const {
        const char *sep = "";
        const auto &borderIt = tree_.upper_bound(*backlog_);
        
        auto curIt = tree_.begin();
        for(;curIt != borderIt; ++curIt) {
            std::cout << sep << *curIt;
            sep = ", ";
        }

        if(!isUndo && backlog_.has_value()) {
            std::cout << sep << *backlog_;
            sep = ", ";
        }

        for(const auto &end = tree_.end(); curIt != end; ++curIt) {
            std::cout << sep << *curIt;
            sep = ", ";
        }
        std::cout << std::endl;
    }
};

template<typename T>
bool Set<T>::contains(const T &val) const {
    bool treeSearch = tree_.find(val) != tree_.end();
    if(!isUndo && backlog_.has_value()) {
        return treeSearch || ((*backlog_) == val);
    }

    return treeSearch;
}

template<typename T>
void Set<T>::remove(const T &val) {
    if(!contains(val))
        return;

    if(backlog_.has_value())
        lastOp(backlog_.value());
    backlog_ = val;
    lastOp = lastOpFunc[REMOVE];
    isUndo = false;
}

template<typename T>
void Set<T>::insert(const T &val) {
    if(contains(val))
        return;

    if(backlog_.has_value())
        lastOp(backlog_.value());
    backlog_ = val;
    lastOp = lastOpFunc[INSERT];
    isUndo = false;
}

template<typename T>
bool Set<T>::undo() {
    if(isUndo)
        return false;

    prev = lastOp;
    lastOp = lastOpFunc[INVALID];
    isUndo = true;
    return true; 
}

template<typename T>
bool Set<T>::redo() {
    if(!isUndo)
        return false;

    lastOp = prev;

    isUndo = false;
    return true; 
}
}

#endif