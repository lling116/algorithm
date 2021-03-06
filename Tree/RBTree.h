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

inline void RBTreeRotateLeft(RBTreeNodeBase* x, RBTreeNodeBase* &root) {
    RBTreeNodeBase* const y = x->right_;

    x->right_ = y->left_;
    if(y->left_ != nullptr) {
        y->left_->parent_ = x;
    }

    y->parent_ = x->parent_;
    if(x == root) {
        root = y;
    }else if(x == x->parent_->left_) {
        x->parent_->left_ = y;
    }else {
        x->parent_->right_ = y;
    }

    x->parent_ = y;
    y->left_ = x;
}

inline void RBTreeRotateRight(RBTreeNodeBase* x, RBTreeNodeBase* &root) {
    RBTreeNodeBase* const y = x->left_;
    x->left_ = y->right_;
    if(y->right_ != nullptr) {
        y->right_->parent_ = x;
    }

    y->parent_ = x->parent_;
    if(x == root) {
        root = x;
    }else if(x == x->parent_->left_) {
        x->parent_->left_ = y;
    }else {
        x->parent_->right_ = y;
    }

    x->parent_ = y;
    y->right_ = x;
}

/* 插入节点x后重新调整平衡 */
inline void RBTreeRebalanceForInsert(RBTreeNodeBase* x,
                                     RBTreeNodeBase &headerNode) {
    x->color_ = Red;
    RBTreeNodeBase* &root = headerNode.parent_;

    while(x != root && x->parent_->color_ == Red) {
        if(x->parent_ == x->parent_->parent_->left_) {
            RBTreeNodeBase* y = x->parent_->parent_->right_;
            if(y && y->color_ == Red) {
                x->parent_->color_ = Black;
                y->color_ = Black;
                x->parent_->parent_->color_ = Red;
                x = x->parent_->parent_;
            }else {
                if(x == x->parent_->right_) {
                    x = x->parent_;
                    RBTreeRotateLeft(x, root);
                }
                x->parent_->color_ = Black;
                x->parent_->parent_->color_ = Red;
                RBTreeRotateRight(x->parent_->parent_, root);
            }
        }else {
            RBTreeNodeBase* y = x->parent_->parent_->left_;
            if(y && y->color_ == Red) {
                x->parent_->color_ = Black;
                y->color_ = Black;
                x->parent_->parent_->color_ = Red;
                x = x->parent_->parent_;
            }else {
                if(x == x->parent_->left_) {
                    x = x->parent_;
                    RBTreeRotateRight(x, root);
                }
                x->parent_->color_ = Black;
                x->parent_->parent_->color_ = Red;
                RBTreeRotateLeft(x->parent_->parent_, root);
            }
        }
    }
    
    root->color_ = Black;
}

/* 删除节点x后重新调整平衡 */
inline RBTreeNodeBase* RBTreeRebalanceForErase(RBTreeNodeBase* const x, 
                                               RBTreeNodeBase &headerNode) {
    RBTreeNodeBase* &root = headerNode.parent_;
    RBTreeNodeBase* &leftmost = headerNode.left_;
    RBTreeNodeBase* &rightmost = headerNode.right_;
    return nullptr;
}

struct RBTreeIteratorBase {
    typedef RBTreeNodeBase* BasePtr;

    typedef std::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t                       difference_type;
    typedef size_t                          size_type;

    BasePtr node_;

    void increment() {
        if(node_->right_ != nullptr) {
            node_ = node_->right_;
            while(node_->left_ != nullptr) {
                node_ = node_->left_;
            }
        }else {
            BasePtr y = node_->parent_;
            while(node_ == y->right_) {
                node_ = y;
                y = y->parent_;
            }
            if(node_->right_ != y) {
                node_ = y;
            }
        }
    }

    void decrement() {
        if(node_->color_ == Red && 
           node_->parent_->parent_ == node_) {
            node_ = node_->right_;
        }else if(node_->left_ != nullptr) {
            node_ = node_->left_;
            while(node_->right_ != nullptr) {
                node_ = node_->right_;
            }
        }else {
            BasePtr y = node_->parent_;
            while(node_ == y->left_) {
                node_ = y;
                y = y->parent_;
            }
            node_ = y;
        }
    }
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
        increment();
        return *this;
    }

    Self operator++(int) {
        Self tmp = *this;
        increment();
        return tmp;
    }

    Self& operator--() {
        decrement();
        return *this;
    }

    Self operator--(int) {
        Self tmp = *this;
        decrement();
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

template<typename Key, typename Value, typename KeyOfValue, 
         typename Compare, typename Alloc = std::allocator<Value>>
class RBTree {
    typedef RBTreeNode<Value>* NodePtr;
    typedef RBTreeNodeBase*    BasePtr;

    struct ReuseOrAllocNode {
        RBTree &t_;
        BasePtr root_;
        BasePtr nodes_;

        ReuseOrAllocNode(RBTree& t) : 
            t_(t),
            root_(t.Root()),
            nodes_(t.RightMost()) {
            if(root_) {
                
            }else {
                nodes_ = nullptr;
            }
        }

        ReuseOrAllocNode(const ReuseOrAllocNode &) = delete;
    };


protected:
    NodePtr& Root() { return header_.headerNode_.parent_; }
    NodePtr& LeftMost() { return header_.headerNode_.left_; }
    NodePtr& RightMost() { return header_.headerNode_.right_; }

    RBTreeHeader header_;
};

} // end of namespace Tree

#endif
