
#ifndef __TOY_TREE__
#define __TOY_TREE__

#include <new>
#include <stdlib.h>
#include <iostream>

#include <ext/aligned_buffer.h>
#include <toy_alloc.h>
#include <toy_iterator.h>

namespace toy {

enum rbtree_node_color { _rbtree_red = false, _rbtree_black = true };

/*
 * 将node分成两级，是为了rbtree_node_base可定义为普通类，而不需要与rbtree_node混在一起
 * 成为模板类，模板类将在编译时期为每一种类型参数具化为一种类型，这样使得rbtree_node_base
 * 部分的代码重复；所以可以将模板类中与类型数T无关的函数和成员分离出来，成为一个普通的基类；
 */
struct rbtree_node_base
{
    typedef rbtree_node_base* base_ptr;

    rbtree_node_color _color;
    base_ptr _parent;
    base_ptr _left;
    base_ptr _right;

    static base_ptr minimum(base_ptr __x) {
        while(__x->_left != nullptr) __x = __x->_left;
        return __x;
    }

    static base_ptr maximum(base_ptr __x) {
        while(__x->_right  != nullptr) __x = __x->_right;
        return __x;
    }

    static base_ptr increment(base_ptr __x) {
        if(__x->_right != nullptr) {
            __x = __x->_right;
            __x = minimum(__x);
        }
        else {
            base_ptr __y = __x->_parent;
            while(__x == __y->_right) {
                __x = __y;
                __y = __y->_parent;
            }
            
            if(__x->_right != __y)
                __x = __y;
        }
        
        return __x;
    }

    static base_ptr decrement(base_ptr __x) {
        if(__x->_color == _rbtree_red && 
           __x->_parent->_parent == __x) {
            __x = __x->_right;
        }
        else if(__x->_left != nullptr) {
            __x = __x->_left;
            __x = maximum(__x);
        }
        else {
            base_ptr __y = __x->_parent;
            while(__x == __y->_left) {
                __x = __y;
                __y = __y->_parent;
            }
            __x = __y;
        }

        return __x;
    }
};

template<typename T>
struct rbtree_node: public rbtree_node_base {
    T _storage;

    // node分层的一个缺点：迭代器既需要访问value值（以rbtree_node类型存储），
    // 又需要以rbtree_node_base类型进行移动，所以需要一种机制在两种形式之间切换;
    // 此成员函数用于返回存储value的地址
    T* valptr()
    { return &_storage; }

    const T* valptr() const
    { return &_storage; }
};

template<typename T>
struct rbtree_iterator:
public iterator<bidirectional_iterator_tag, T> {
    typedef rbtree_node_base::base_ptr node_base_ptr;
    typedef rbtree_node<T>* node_ptr;

    node_base_ptr _pnode;

    rbtree_iterator(): _pnode() {}
    explicit rbtree_iterator(node_base_ptr __x): _pnode(__x) {}

    T& operator*() const {
        // 此处将_node_base_ptr类型的指针强制转换成_node_ptr类型，
        // 基类指针转换为派生类属于向下转型，并不安全
        return *(static_cast<node_ptr>(_pnode)->valptr());
    }

    T* operator->() const {
        return static_cast<node_ptr>(_pnode)->valptr();
    }

    rbtree_iterator& operator++() {
        _pnode = rbtree_node_base::increment(_pnode);
        return *this;
    }

    rbtree_iterator operator++(int) {
        rbtree_iterator __temp = *this;
        _pnode = rbtree_node_base::increment(_pnode);
        return __temp;
    }

    rbtree_iterator& operator--() {
        _pnode = rbtree_node_base::decrement(_pnode);
        return *this;
    }

    rbtree_iterator operator--(int) {
        rbtree_iterator __temp = *this;
        _pnode = rbtree_node_base::decrement(_pnode);
        return __temp;
    }

    bool operator==(const rbtree_iterator& __x) const {
        return _pnode == __x._pnode;
    }

    bool operator!=(const rbtree_iterator& __x) const {
        return _pnode != __x._pnode;
    }
};

/*
 * Key:如果适配器为map，那么此类型为pair中提取出来的（比如第一个参数）；
 * Value:如果适配器为map，那么此类型为pair；
 * KeyOfValue:从实值中提取键值的方法；
 */
template<class Key, class Value, class KeyOfValue, class Compare, 
         class Alloc = toyalloc<rbtree_node<Value>>>
class rbtree {
protected:
    typedef rbtree_node_base* node_base_ptr;
    typedef rbtree_node<Value> rbtree_node_type;
    typedef Alloc node_alloc;
    typedef rbtree_node_color color_type;

public:
    typedef Value value_type;
    typedef value_type* pointer;
    typedef rbtree_iterator<value_type> iterator;
    typedef value_type& reference;
    typedef size_t size_type;
    typedef ptrdiff_t defference_type;

    typedef Key key_type;
    typedef rbtree_node_type* node_ptr;
    

protected:
    node_ptr create_one_node(const value_type& __x) {
        node_ptr __np = node_alloc::allocate(1);
        construct<value_type>(&(__np->_storage), __x);
        return __np;
    }

    void destory_one_node(node_ptr __np) {
        destory(&(__np->_storage));
        node_alloc::deallocate(__np, 1);
    }

    node_ptr clone_one_node(node_ptr __np) {
        node_ptr __temp = create_one_node(__np->_storage);
        __temp->_color = __np->_color;
        __temp->_left = nullptr;
        __temp->_right = nullptr;
        return __temp;
    }

    size_type node_count;
    node_ptr header;
    Compare key_compare;

    // 返回指针的引用就像返回指针的指针一样，可以根据此返回值对指针变量进行修改；
    // 如果只是返回指针，那么调用者仅仅得到一个指针的拷贝值，对这个拷贝值进行修改
    // 无法影响“原指针变量”；
    node_base_ptr& root() const {
        // return static_cast<node_ptr&>(header->_parent);
        return header->_parent;
    }
    node_base_ptr& leftmost() const {
        // return static_cast<node_ptr&>(header->_left);
        return header->_left;
    }
    node_base_ptr& rightmost() const {
        // return static_cast<node_ptr&>(header->_right);
        return header->_right;
    }

    // static node_base_ptr left(node_base_ptr __x) { 
    //     return __x->_left;
    // }
    // static node_base_ptr right(node_base_ptr __x) { 
    //     return __x->_right;
    // }
    // static node_base_ptr parent(node_base_ptr __x) { 
    //     return __x->_parent;
    // }
    static reference value(node_ptr __x) { 
        return *__x->valptr();
    }
    static const Key& key(node_ptr __x) { 
        return KeyOfValue()(value(__x)); 
    }
    static color_type& color(node_ptr __x) { 
        return __x->_color; 
    }

    static node_ptr left(node_base_ptr __x) { 
        return static_cast<node_ptr>(__x->_left);
    }
    static node_ptr right(node_base_ptr __x) { 
        return static_cast<node_ptr>(__x->_right);
    }
    static node_ptr parent(node_base_ptr __x) { 
        return static_cast<node_ptr>(__x->_parent);
    }
    static reference value(node_base_ptr __x) { 
        return *static_cast<node_ptr>(__x)->valptr();
    }
    static const Key& key(node_base_ptr __x) { 
        return KeyOfValue()(value(__x));
    }
    static color_type& color(node_base_ptr __x) { 
        return __x->_color;
    }

    // 子类向基类的向上转型比较容易，直接赋值即可
    static node_base_ptr minimum(node_base_ptr __np) {
        return rbtree_node_base::minimum(__np);
    }
    static node_base_ptr maximum(node_base_ptr __np) {
        return rbtree_node_base::maximum(__np);
    }

private:
    iterator insert(node_base_ptr, node_base_ptr, node_base_ptr);

    void reset()
    {
        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
        node_count = 0;
    }

    void init() {
        header = create_one_node(value_type());
        color(header) = _rbtree_red;
        reset();
    }

    void erase(node_ptr __x)
    {
        // Erase without rebalancing.
        while (__x != nullptr)
        {   
            erase(right(__x));
            node_ptr __y = left(__x);
            destory_one_node(__x);
            __x = __y;
        }
    }
public:
    rbtree(const Compare& comp = Compare()) 
        : key_compare(comp) {
        init();
    }

    ~rbtree() {
        clear();
        destory_one_node(header);
    }

    Compare key_comp() const { return key_compare; }
    iterator begin() const { return iterator(leftmost()); }
    iterator end() const { return iterator(header); }
    bool empty() const { return node_count == 0; }
    size_type size() { return node_count; }

    iterator insert_multi(const value_type&);
    std::pair<iterator, bool> insert_unique(const value_type&);
    iterator find(const Key&);


    void clear() {
        node_ptr __begin = static_cast<node_ptr>(root());
        erase(__begin);
        reset();
    }
};

/**
 * @description: 找到待插入点的父节点，并调用insert()函数将其插入
 * @param {type} 
 * @return: 
 */
template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rbtree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rbtree<Key, Value, KeyOfValue, Compare, Alloc>::insert_multi(const Value& __val) {
    node_ptr __new = create_one_node(__val);

    node_ptr __x = root();
    node_ptr __y = header;

    while(__x != nullptr)
    {
        __y = __x;
        __x = key_compare(key(__new), key(__x)) ?
              left(__x) : right(__x);
    }
    
    return insert(__x, __y, __new);
}

/**
 * @description: 如果待插入值已经存在返回错误，否则找到待插入点的父节点，并调用insert()函数
 *               将其插入；
 * @param {type} 
 * @return: 
 */
template<class Key, class Value, class KeyOfValue, class Compare, class Alloc> 
std::pair<typename rbtree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool> 
rbtree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value& __val) {
    // 这里可能造成内存泄露
    node_ptr __new = create_one_node(__val);

    node_base_ptr __x = root();
    node_base_ptr __y = header;
    bool __comp = true;

    while(__x != nullptr)
    {
        __y = __x;
        // 此时比较返回的是__new < __x的真假，与下面不同
        __comp = key_compare(key(__new), key(__x));
        __x = __comp ? left(__x) : right(__x);
    }

    iterator __j = iterator(__y);
    if(__comp) {
        if(__j == begin())
        {
            // 待插入节点的父节点为整个树的最左节点，可直接插入
            return std::pair<iterator, bool>(insert(__x, __y, __new), true);
        }
        else {

            // 否则，迭代器减一，注意__j为迭代器，其减一操作将向上遍历，直至找到
            // 整个树中key值小于其key值的最大值，其节点并不一定与__j相邻；
            --__j;
        }
    }
    
    // 此时比较返回的是__j < __new的真假，与上面不同
    if(key_compare(key(__j._pnode), key(__new))) {
        return std::pair<iterator, bool>(insert(__x, __y, __new), true);
    }
    
    destory_one_node(__new);
    return std::pair<iterator, bool>(__j, false);
}

void rbtree_rebalance(rbtree_node_base* __x, rbtree_node_base*& __root);

/**
 * @description: 插入节点，并在需要的时候修改leftmost()与rightmost()，然后调用
 *               rbtree_rebalance()使红黑树重归平衡；
 * @param {type} 
 * @return: 
 */
template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rbtree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rbtree<Key, Value, KeyOfValue, Compare, Alloc>::
insert(node_base_ptr __x, node_base_ptr __y, node_base_ptr __new) {
    node_ptr __px = static_cast<node_ptr>(__x);
    node_ptr __py = static_cast<node_ptr>(__y);
    node_ptr __pn = static_cast<node_ptr>(__new);

    if(__py == header || __px != nullptr 
    || key_compare(key(__pn), key(__py))) {
        __py->_left = __pn;
        if(__py == header) {
            root() = __pn;
            leftmost() = __pn;
        }
        else if(__y == leftmost()) {
            leftmost() =  __pn;
        }
    }
    else {
        __py->_right = __pn;
        if(__py == rightmost()) {
            rightmost() = __pn;
        }
    }

    __pn->_parent = __py;
    __pn->_left = nullptr;
    __pn->_right = nullptr;

    // while(1) {
    //     volatile int k;
    //     k++;
    //     __x->_parent->_color = _rbtree_red;
    // }

    rbtree_rebalance(__pn, header->_parent);
    ++node_count;
    return iterator(__pn);
}

template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
typename rbtree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
rbtree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& __k) {
    node_base_ptr __y = header;
    node_base_ptr __x = root();

    while(__x != nullptr) {
        if(!key_compare(key(__x), __k)) {
            // 当__x 大于等于 __k时，向左走，同时父节点__y也要跟着走；
            // 因为其中包含等于的情况，有可能是目标节点；
            __y = __x;
            __x = left(__x);
        }
        else {
            // 当__x 小于 __k时，向右走，但是父节点__y不会移动；
            __x = right(__x);
        }
    }

    iterator __j = iterator(__y);
    return (__j == end() || key_compare(__k, key(__j._pnode))) ? end() : __j;
}

void rbtree_rotate_left(rbtree_node_base* __x, rbtree_node_base*& __root) {
    rbtree_node_base* __y = __x->_right;
    __x->_right = __y->_left;
    if(__y->_left != nullptr) {
        __y->_left->_parent = __x;
    }
    __y->_parent = __x->_parent;

    if(__x == __root) __root = __y;
    else if(__x == __x->_parent->_left) __x->_parent->_left = __y;
    else __x->_parent->_right = __y;

    __y->_left = __x;
    __x->_parent = __y;
}

void rbtree_rotate_right(rbtree_node_base* __x, rbtree_node_base*& __root) {
    rbtree_node_base* __y = __x->_left;
    __x->_left = __y->_right;
    if(__y->_right != nullptr) {
        __y->_right->_parent = __x;
    }
    __y->_parent = __x->_parent;

    if(__x == __root) __root = __y;
    else if(__x == __x->_parent->_right) __x->_parent->_right = __y;
    else __x->_parent->_left = __y;

    __y->_right = __x;
    __x->_parent = __y;
}

void rbtree_rebalance(rbtree_node_base* __x, rbtree_node_base*& __root) {
    __x->_color = _rbtree_red;
    // while(1) {
    //     volatile int k;
    //     k++;
    //     // __x->_parent->_color = _rbtree_red;
    // }

    while(__x != __root && __x->_parent->_color == _rbtree_red) {

        if(__x->_parent == __x->_parent->_parent->_left) {
            rbtree_node_base* __y = __x->_parent->_parent->_right;
            if(__y && __y->_color == _rbtree_red) {
                __x->_parent->_color = _rbtree_black;
                __y->_color = _rbtree_black;
                __x->_parent->_parent->_color = _rbtree_red;
                __x = __x->_parent->_parent;
    
            }
            else {
                if(__x == __x->_parent->_right) {
                    __x = __x->_parent;
                    rbtree_rotate_left(__x, __root);
                }
                __x->_parent->_color = _rbtree_black;
                __x->_parent->_parent->_color = _rbtree_red;
                rbtree_rotate_right(__x->_parent->_parent, __root);
            }
        }
        else {
            rbtree_node_base* __y = __x->_parent->_parent->_left;
            if(__y && __y->_color == _rbtree_red) {
                __x->_parent->_color = _rbtree_black;
                __y->_color = _rbtree_black;
                __x->_parent->_parent->_color = _rbtree_red;
                __x = __x->_parent->_parent;
            }
            else {
                if(__x == __x->_parent->_left) {
                    __x = __x->_parent;
                    rbtree_rotate_right(__x, __root);
                }
                __x->_parent->_color = _rbtree_black;
                __x->_parent->_parent->_color = _rbtree_red;
                rbtree_rotate_left(__x->_parent->_parent, __root);
            }
        }
    }
    __root->_color = _rbtree_black;
}

}

#endif