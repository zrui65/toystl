/*
 * @Description:
 * 
 */

#ifndef __TOY_LIST__
#define __TOY_LIST__

#include <toy_iterator.h>
#include <toy_alloc.h>

#include <iostream>

namespace toy {

template<class T>
struct list_node {
    typedef list_node<T>* node_pointer;
    node_pointer prev;
    node_pointer next;
    T data;
};

/*
 * 需要注意所有操作符重载函数的返回值，有的返回所管理的数据；
 * 有的返回迭代器；
 */
template<class T>
struct list_iterator : 
public iterator<bidirectional_iterator_tag, T> {
    typedef list_node<T>* node_pointer;
    
    // 整个list迭代器的对象仅仅是一个指向list_node的指针
    node_pointer _ipnode;

    list_iterator() {}
    list_iterator(node_pointer __np): _ipnode(__np) {}
    list_iterator(const list_iterator& __iter): _ipnode(__iter._ipnode) {}
    ~list_iterator() {}

    bool operator==(const list_iterator __iter) const {
        return _ipnode == __iter._ipnode;
    }

    bool operator!=(const list_iterator __iter) const {
        return _ipnode != __iter._ipnode;
    }

    // 返回数据的引用，说明可写可读
    T& operator*() const {
        return (*_ipnode).data;
    }

    T* operator->() const {
        return &(operator*());
    }

    list_iterator& operator=(const list_iterator& __iter) {
        if(this == &__iter) return *this;

        _ipnode = __iter._ipnode;
        return *this;
    }

    list_iterator& operator++() {
        _ipnode = _ipnode->next;
        return *this;
    }

    // 以值返回，因为不能返回局部变量的引用
    list_iterator operator++(int) {
        list_iterator __old_iter(*this);
        _ipnode = _ipnode->next;
        return __old_iter;
    }

    list_iterator& operator--() {
        _ipnode = _ipnode->prev;
        return *this;
    }

    list_iterator operator--(int) {
        list_iterator __old_iter(*this);
        _ipnode = _ipnode->prev;
        return __old_iter;
    }
};

template<class T, class alloc = toyalloc<list_node<T>>>
class list {
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef list_iterator<value_type> iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    typedef list_node<T>* node_pointer;

    typedef alloc node_alloc;

    // 整个链表模板中，其实只有这个node指针（栈中）以及在构造函数中创建的空白node（堆中）
    node_pointer _pnode;

    node_pointer create_one_node(const value_type& __data) const {
        node_pointer __np = node_alloc::allocate(1);
        construct(&(__np->data), __data);
        return __np;
    }

    void destory_one_node(node_pointer __np) const {
        destory(&(__np->data));
        node_alloc::deallocate(__np, size_type(1));
    }

    void null_node() {
        _pnode = create_one_node(value_type());
        _pnode->prev = _pnode;
        _pnode->next = _pnode;
    }

    /*
    * 作用：将__first到__last的链表移到__position之前；
    */
    void transfer(iterator __position, iterator __first, 
                                iterator __last) {
        if(__position != __last) {
            __last._ipnode->prev->next = __position._ipnode;
            __first._ipnode->prev->next = __last._ipnode;
            __position._ipnode->prev->next = __first._ipnode;

            node_pointer __temp = __position._ipnode->prev;
            __position._ipnode->prev = __last._ipnode->prev;
            __last._ipnode->prev = __first._ipnode->prev;
            __first._ipnode->prev = __temp;
        }
    }

public:
    list() {
        null_node();
    }

    list(const list& __lst) {
        null_node();
        iterator iter = __lst.begin();
        while(iter != __lst.end()) {
            push_back(*iter);
            ++iter;
        }
    }

    ~list() {
        clear();
        destory_one_node(_pnode);
    }

    iterator begin() const {
        return iterator(_pnode->next);
    }

    iterator end() const {
        return iterator(_pnode);
    }

    // 遍历整个链表统计元素个数，所以尽量使用empty()来判断链表是否为空
    size_type size() const {
        size_type __n = 0;
        iterator __iter = begin();
        while(++__iter != end()) {
            ++__n;
            ++__iter;
        } 
        return __n;
    }

    bool empty() {
        return begin() == end();
    }

    void insert(iterator __position, const value_type& __data) const {
        node_pointer __np = create_one_node(__data);
        __position._ipnode->prev->next = __np;
        __np->prev = __position._ipnode->prev;
        __np->next = __position._ipnode;
        __position._ipnode->prev = __np;
    }

    void push_back(const value_type& __data) const {
        insert(end(), __data);
    }

    void pop_back() const {
        erase(--end());
    }

    void push_front(const value_type& __data) const {
        insert(begin(), __data);
    }

    void pop_front() const {
        erase(begin());
    }
    
    iterator erase(iterator __first, iterator __last) const {
        __first._ipnode->prev->next = __last._ipnode;
        __last._ipnode->prev = __first._ipnode->prev;

        while(__first != __last) {
            iterator __temp = __first;
            ++__first;
            destory_one_node(__temp._ipnode);
        }

        return __last;
    }

    iterator erase(iterator __position) const {
        iterator __temp = __position;
        __temp++;
        return erase(__position, __temp);

        // 不能使用下面这种写法，因为参数位置上是表达式时，这些表达式的执行顺序无法确定；
        // 比如下面，会先执行++__position，然后传入函数，两个参数是相同的
        // return erase(__position, ++__position);
    }

    void swap(list& __lst) {
        node_pointer __temp = _pnode;
        _pnode = __lst._pnode;
        __lst._pnode = __temp;
    }

    void clear() const {
        erase(begin(), end());
    }

    list& operator=(const list& __lst) {
        list __temp(__lst);
        _pnode = __temp._pnode;
        return *this;
    }

    void splice(iterator __position, list& __lst) {
        if(__lst.empty()) return;
        transfer(__position, __lst.begin(), __lst.end());
    }

    void splice(iterator __position, list& __lst, iterator __lst_position) {
        iterator __lst_next = __lst_position;
        ++__lst_next;

        if(__position == __lst_position 
        || __position == __lst_next) {
            return;
        }

        transfer(__position, __lst_position, __lst_next);
    }

    void splice(iterator __position, iterator __first, iterator __last) {
        if(__first == __last) return;
        transfer(__position, __first, __last);
    }

    // 反转整个链表
    void reverse() {
        if(size() <= 1) return;
        // 由于begin()函数的实现是与空白节点（pnode）有关的；
        // 所以需要事先保存原始list的begin()迭代器；
        // 后续如果直接使用begin()函数将导致错误；
        iterator __start = begin();
        iterator __iter = begin();
        do {
            node_pointer __next = __iter._ipnode->next;
            __iter._ipnode->next = __iter._ipnode->prev;
            __iter._ipnode->prev = __next;

            __iter._ipnode = __next;

        } while(__iter != __start);
    }

    // 合并两个有序链表，合并完成后，__lst长度为零
    void merge(list& __lst) {
        iterator __first1 = begin();
        iterator __last1 = end();
        iterator __first2 = __lst.begin();
        iterator __last2 = __lst.end();

        while(__first1 != __last1 && __first2 != __last2) {
            if(*__first2 < *__first1) {
                iterator __next2 = __first2;
                ++__next2;
                transfer(__first1, __first2, __next2);
                __first2 = __next2;
            }
            else {
                ++__first1;
            }
        }

        if(__first2 != __last2) {
            transfer(__last1, __first2, __last2);
        }
    }

    void sort() {
        // 链表长度为0或1时什么都不做
        if(_pnode->next != _pnode && _pnode->next->next != _pnode) {
            list __carry;
            list __counter[64];
            int __fill = 0;

            while(!empty()) {
                __carry.splice(__carry.begin(), *this, begin());
                
                int __i = 0;
                while(__i < __fill && !__counter[__i].empty()) {
                    __counter[__i].merge(__carry);
                    __carry.swap(__counter[__i++]);
                }

                __carry.swap(__counter[__i]);
                if(__i == __fill) ++__fill;
            }

            for(int __i = 1; __i < __fill; ++__i) {
                __counter[__i].merge(__counter[__i-1]);
            }
            swap(__counter[__fill-1]);
        }
    }

};

};

#endif