#ifndef RBTREE_H_
#define RBTREE_H_

#include <memory>
#include <iterator>

namespace Tree {

enum RBTreeColor {Red = true, Black = false};

// basenode - 不包含数据
struct RBTreeNodeBase {
    typedef RBTreeNodeBase*        NodePtr;
    typedef const RBTreeNodeBase*  ConstNodePtr;

    RBTreeColor color_;
    NodePtr parent_;
    NodePtr left_;
    NodePtr right_;
};

// node - 包含数据
template<typename T>
struct RBTreeNode : public RBTreeNodeBase {
    T val_;

    T* ValPtr() {
        return std::addressof(val_);
    }

    const T* ValPtr() const {
        return std::addressof(val_);
    }
};

// 特殊header节点
struct RBTreeHeader {
    RBTreeNodeBase headerNode_;
    size_t nodeCount_;

    RBTreeHeader() noexcept {
        headerNode_.color_ = Red;
        reset();
    };

    RBTreeHeader(RBTreeHeader &&val) noexcept {
        if(val.headerNode_.parent_ != nullptr) {
            move(val);
        }else {
            headerNode_.color_ = Red;
            reset();
        }
    };
    
    void move(RBTreeHeader &from) {
        headerNode_.color_ = from.headerNode_.color_;
        headerNode_.parent_ = from.headerNode_.parent_;
        headerNode_.left_ = from.headerNode_.left_;
        headerNode_.right_ = from.headerNode_.right_;
        headerNode_.parent_->parent_ = &headerNode_;

        nodeCount_ = from.nodeCount_;
        from.reset();
    }

    void reset() {
        headerNode_.parent_ = nullptr;
        headerNode_.left_ = &headerNode_;
        headerNode_.right_ = &headerNode_;
        nodeCount_ = 0;
    }
};

// 节点操作
RBTreeNodeBase* increment(RBTreeNodeBase *) noexcept;
const RBTreeNodeBase* increment(const RBTreeNodeBase *) noexcept;
RBTreeNodeBase* decrement(RBTreeNodeBase *) noexcept;
const RBTreeNodeBase* decrement(const RBTreeNodeBase *) noexcept;

struct RBTreeIteratorBase {
    typedef RBTreeNodeBase* BasePtr;

    typedef std::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t                       difference_type;
    typedef size_t                          size_type;

    BasePtr node_;
};

// 迭代器
template<typename T, typename Ptr = T*, typename Ref = T&>
struct RBTreeIterator : public RBTreeIteratorBase {
    typedef T   value_type;
    typedef Ptr pointer;
    typedef Ref reference;

    typedef RBTreeNode<T>* NodePtr;

    typedef RBTreeIterator<T, Ptr, Ref> Self;

    typedef RBTreeIterator<T, const T*, const T&> const_iterator;
    typedef RBTreeIterator<T, T*, T&> iterator;
    

    RBTreeIterator() {}
    RBTreeIterator(const iterator& it) { node_ = it.node_; }
    explicit RBTreeIterator(BasePtr node) { node_ = node; }

    reference operator*() const {
        return *static_cast<NodePtr>(node_)->ValPtr();
    }

    pointer operator->() const {
        return static_cast<NodePtr>(node_)->ValPtr();
    }

    Self& operator++() {
        node_ = increment(node_);
        return *this;
    }

    Self operator++(int) {
        Self tmp = *this;
        node_ = increment(node_);
        return tmp;
    }

    Self& operator--() {
        node_ = decrement(node_);
        return *this;
    }

    Self operator--(int) {
        Self tmp = *this;
        node_ = decrement(node_);
        return tmp;
    }
};

inline bool operator==(const RBTreeIteratorBase& l,
                       const RBTreeIteratorBase& r) {
    return l.node_ == r.node_;
}

inline bool operator!=(const RBTreeIteratorBase& l,
                       const RBTreeIteratorBase& r) {
    return l.node_ != r.node_;
}

} // end of namespace Tree

#endif
