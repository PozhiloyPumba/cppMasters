#include "oneWayPersistent.hpp"

namespace persistent {

template<is_not_string_class T>
bool Set<T>::contains(const T &val) const {
    bool treeSearch = tree_.find(val) != tree_.end();
    if(!isUndo && backlog_.has_value()) {
        return treeSearch || ((*backlog_) == val);
    }

    return treeSearch;
}

template<is_not_string_class T>
void Set<T>::remove(const T &val) {
    if(!contains(val))
        return;

    if(backlog_.has_value())
        lastOp(backlog_.value());
    backlog_ = val;
    lastOp = lastOpFunc[REMOVE];
    isUndo = false;
}

template<is_not_string_class T>
void Set<T>::insert(const T &val) {
    if(contains(val))
        return;

    if(backlog_.has_value())
        lastOp(backlog_.value());
    backlog_ = val;
    lastOp = lastOpFunc[INSERT];
    isUndo = false;
}

template<is_not_string_class T>
bool Set<T>::undo() {
    if(isUndo)
        return false;

    prev = lastOp;
    lastOp = lastOpFunc[INVALID];
    isUndo = true;
    return true; 
}

template<is_not_string_class T>
bool Set<T>::redo() {
    if(!isUndo)
        return false;

    lastOp = prev;

    isUndo = false;
    return true; 
}

template<is_not_string_class T>
void Set<T>::dump() const {
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

}