#include "oneWayPersistent.hpp"

namespace persistent {

template<is_string_class T>
void Set<T>::commit() {    
    if(root_ == newRoot_) {
        oldRoot_ = newRoot_;
        for(auto n: changesOld_)
            delete n;
    } else {
        for(auto n: changesNew_)
            delete n;
    }
    newRoot_ = nullptr;
    oldRoot_ = root_;
    changesOld_.clear();
    changesNew_.clear();
}

template<is_string_class T>
bool Set<T>::contains(const T &val) const {
    Node *traverseNode = root_;
    size_t elementsFound = 0;
    size_t len = val.length();
    string_view v = string_view(val);
    while (traverseNode && elementsFound < len) {
        string_view substr = v.substr(elementsFound, T::npos);

        auto it = std::find_if(traverseNode->children_.begin(), traverseNode->children_.end(), 
            [&substr](Edge &edge){ return substr.starts_with(edge.label_);});

        if (it != traverseNode->children_.end()) {
            traverseNode = it->target_;
            elementsFound += it->label_.length();
        }
        else {
            traverseNode = nullptr;
        }
    }

    return (traverseNode && traverseNode->isTerminal && (elementsFound == len));
}


template<is_string_class T>
void Set<T>::insert(const T &val) {
    if(contains(val)) {
        return;
    }

    commit();

    Node *traverseNode = root_;
    size_t elementsFound = 0;
    size_t len = val.length();
    string_view v = string_view(val);

    auto commonPrefix = [](auto &fst, auto &snd){
        auto impl = [](auto &a, auto &b){
            return string_view(a.begin(), std::mismatch(a.begin(), a.end(), b.begin()).first);
        };
        if(fst.size() > snd.size()) return impl(snd, fst);
        return impl(fst, snd);
    };

    Edge *copyTarget = nullptr;

    while (traverseNode && elementsFound < len) {
        changesOld_.push_back(traverseNode);
        changesNew_.push_back(new Node(*traverseNode));
        auto &tmp = changesNew_.back();
        if(copyTarget) {
            copyTarget->target_ = tmp;
        }

        string_view substr = v.substr(elementsFound, T::npos);

        auto it = std::find_if(tmp->children_.begin(), tmp->children_.end(), 
            [&substr](Edge &edge){return substr.starts_with(edge.label_[0]);});

        if (it != tmp->children_.end()) {
            string_view com = commonPrefix(substr, it->label_);
            elementsFound += com.size();
            if(elementsFound == len) {
                traverseNode = it->target_;
                changesNew_.push_back(new Node(*traverseNode));
                it->target_ = changesNew_.back();
                break;
            }
            if(com.size() == it->label_.size()) {
                traverseNode = it->target_;
                copyTarget = &*it;
                continue;
            }
            auto tmpNode = new Node;
            Edge e{.label_ = T(it->label_.substr(com.size(), T::npos)), .target_ = it->target_};
            tmpNode->children_.push_back(e);
            it->label_ = com;
            it->target_ = tmpNode;
            changesNew_.push_back(tmpNode);
            break;
        }
        else {
            break;
        }
    }
    newRoot_ = changesNew_[0];
    root_ = newRoot_;
    traverseNode = changesNew_.back();
    if(elementsFound == len) {
        traverseNode->isTerminal = true;
        return;
    }
    Edge e{.label_ = T(v.substr(elementsFound, T::npos)), .target_ = new Node};
    e.target_->isTerminal = true;
    traverseNode->children_.push_back(e);
    changesNew_.push_back(e.target_);
}

template<is_string_class T>
void Set<T>::remove(const T &val) {
    if(root_->children_.empty())
        return;

    commit();

    Node *traverseNode = root_;
    Node *parent = root_;
    typename std::list<Edge>::const_iterator e;
    size_t elementsFound = 0;
    size_t len = val.length();
    string_view v = string_view(val);
    Edge *copyTarget = nullptr;

    while (traverseNode && elementsFound < len) {
        changesOld_.push_back(traverseNode);
        if(!changesNew_.empty())
            parent = changesNew_.back();

        changesNew_.push_back(new Node(*traverseNode));
        auto &tmp = changesNew_.back();
        if(copyTarget) {
            copyTarget->target_ = tmp;
        }

        string_view substr = v.substr(elementsFound, T::npos);

        auto it = std::find_if(tmp->children_.begin(), tmp->children_.end(), 
            [&substr](Edge &edge){ return substr.starts_with(edge.label_);});

        if (it != tmp->children_.end()) {
            traverseNode = it->target_;
            elementsFound += it->label_.length();
            copyTarget = &*it;
            e = it;
        }
        else {
            traverseNode = nullptr;
        }
    }
    if(!traverseNode) {
        throw std::runtime_error("unreachable");
    }

    newRoot_ = changesNew_[0];
    root_ = newRoot_;
    
    if(traverseNode->children_.empty()) {
        parent->children_.erase(e);
        return;
    }
    
    auto b = changesNew_.back();
    for(auto &edge: traverseNode->children_) {
        Edge e_{.label_ = e->label_ + edge.label_, .target_ = edge.target_};
        b->children_.push_back(e_);
    }
    parent->children_.erase(e);
    return;
}

template<is_string_class T>
bool Set<T>::undo() {
    if(root_ == oldRoot_)
        return false;
    root_ = oldRoot_?oldRoot_:root_;
    return true;
}

template<is_string_class T>
bool Set<T>::redo() {
    if(root_ == newRoot_)
        return false;

    root_ = newRoot_?newRoot_: root_;
    return true;
}

template<is_string_class T>
void Set<T>::cleanup(Node *curNode) {
    for (auto it = curNode->children_.begin(); it != curNode->children_.end(); ++it) {
        Node *curChild = it->target_;
        if (curChild)
            cleanup(curChild);
    }
    delete curNode;
}

}