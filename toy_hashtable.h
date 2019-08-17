#ifndef __TOY_HASHTABLE__
#define __TOY_HASHTABLE__

#include <toy_iterator.h>
#include <toy_alloc.h>
#include <toy_vector.h>
#include <toy_hashtable_aux.h>

namespace toy {

template<class T>
struct hashtable_node {
    hashtable_node* _next;
    T _value;
};

template<class Value, class Key, class HashFunc, class ExtractKey,
         class EqualKey, class Alloc = toyalloc<hashtable_node<Value>>>
class hashtable;

template<class Value, class Key, class HashFunc, class ExtractKey,
         class EqualKey, class Alloc>
struct hashtable_iterator: public iterator<forward_iterator_tag, Value> {
    typedef hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> iterator;
    typedef hashtable_node<Value>* node_ptr;
    typedef hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc> hashtable_type;
    typedef size_t size_type;
    
    node_ptr _cur;
    hashtable_type* _ht;

    hashtable_iterator() {}
    hashtable_iterator(node_ptr __n, hashtable_type* __htab) :
                       _cur(__n), _ht(__htab) {}

    Value& operator*() const {
        return _cur->_value;
    }

    Value* operator->() const {
        return &(_cur->_value);
    }

    bool operator==(const iterator& __iter) const {
        return _cur == __iter._cur;
    }

    bool operator!=(const iterator& __iter) const {
        return _cur != __iter._cur;
    }

    iterator& operator++() {
        const node_ptr __old = _cur;
        _cur = _cur->_next;
        if(_cur == nullptr) {
            size_type __bucket_index = _ht->get_bucket_index(__old);
            while(!_cur && ++__bucket_index < _ht->_buckets.size()) {
                _cur = _ht->_buckets[__bucket_index];
            }
        }
        return *this;
    }

    iterator operator++(int) {
        iterator __temp;
        ++*this;
        return __temp;
    }
};

template<class Value, class Key, class HashFunc, class ExtractKey,
         class EqualKey, class Alloc>
class hashtable {
public:
    typedef Value value_type;
    typedef Key key_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey,
                               Alloc> iterator;
    
    friend struct hashtable_iterator<Value, Key, HashFunc, ExtractKey, 
                                     EqualKey, Alloc>;

private:
    // 声明的顺序最好与构造函数初始化列表一致，
    // 类内成员的初始化只与声明的顺序有关
    HashFunc _hash;
    EqualKey _equals;
    ExtractKey _get_key;

    typedef hashtable_node<value_type>* node_ptr;
    typedef Alloc node_alloc;

    // 此处将vector的空间配置器省略，使其采用默认的空间配置器
    // 不能将其“直接”设置为hashtable的空间配置器，因为hashtable的空间配置器是分配
    // 一个node；而不是vector想要的node_ptr
    vector<node_ptr> _buckets;
    size_type _node_count;

    node_ptr create_one_node(const value_type& __x) {
        node_ptr __np = node_alloc::allocate(1);
        construct(&(__np->_value), __x);
        return __np;
    }

    void destory_one_node(node_ptr __np) {
        destory(__np);
        node_alloc::deallocate(__np, 1);
    }

    size_type next_size(size_type __n) const {
        return next_prime(__n);
    }
    
    void init(size_type __n) {
        const size_type __buckets_num = next_prime(__n);
        vector<node_ptr> __temp(__buckets_num, nullptr);
        _buckets = __temp;
    }

    size_type get_bucket_index_key(const key_type& __k, const size_type __n) {
        return _hash(__k) % __n;
    }

    size_type get_bucket_index_key(const key_type& __k) {
        return get_bucket_index_key(__k, _buckets.size());
    }

    // 用于桶数发生变化，需要重新hash时
    size_type get_bucket_index(const node_ptr __np, size_type __n) {
        return get_bucket_index_key(_get_key(__np->_value), __n);
    }

    size_type get_bucket_index(const node_ptr __np) {
        return get_bucket_index_key(_get_key(__np->_value));
    }

    size_type get_bucket_index(const value_type& __x) {
        return get_bucket_index_key(_get_key(__x));
    }

    // 负载因子
    const float _max_load_factor = 1.0;
    // 扩容因子
    const size_type _growth_factor = 2;

    /**
     * @description: 
     * @param __n_bkt：当前拥有的桶的数量
     *        __n_new_elt：新的元素数量
     *  
     * @return: ==0 表示不需要扩容；
     *          >=0 表示新的扩容后的桶数
     */
    size_type need_rehash(size_type __n_bkt, size_type __n_new_elt) {
        // 插入__n_ins个元素之后所需要的最小的桶数（在负载因子
        // 为_max_load_factor的条件下）
        long double __min_bkts = __n_new_elt
                        / static_cast<long double>(_max_load_factor);
        if (__min_bkts >= __n_bkt)
            return next_size(max(static_cast<size_type>(__min_bkts) + 1, 
                             __n_bkt * _growth_factor));

        return 0;
    }

    void rehash(size_type __n_new_elt);

    std::pair<iterator, bool> insert_unique_core(const value_type& __x);
    
    iterator insert_multi_core(const value_type& __x);
    
    void erase_bucket(size_type __bucket_index) {
        node_ptr __first = _buckets[__bucket_index];
        while(__first)
        {
            _buckets[__bucket_index] = __first->_next;
            destory_one_node(__first);
            __first = _buckets[__bucket_index];
        }
    }

public:
    hashtable(int __n, const HashFunc& __hf, const EqualKey& __eq) 
    : _hash(__hf), _equals(__eq), _get_key(ExtractKey()), _node_count(0) {
        init(__n);
    }

    ~hashtable() {
        clear();
    }

    iterator begin() {
        for(size_type __n = 0; __n < _buckets.size(); ++__n)
            if (_buckets[__n])
                return iterator(_buckets[__n], this);
        return end();
    }

    iterator end() { 
        return iterator(nullptr, this); 
    }

    size_type size() const {
        return _node_count;
    }

    size_type bucket_num() const {
        return _buckets.size();
    }

    std::pair<iterator, bool> insert_unique(const value_type& __x) {
        rehash(_node_count + 1);
        return insert_unique_core(__x);
    }

    iterator insert_multi(const value_type& __x) {
        rehash(_node_count + 1);
        return insert_multi_core(__x);
    }

    iterator find(const key_type& __k) {
        const size_type __buckets_index = get_bucket_index_key(__k);
        node_ptr __first = _buckets[__buckets_index];

        while(__first) {
            if(_equals(_get_key(__first->_value), __k)) {
                return iterator(__first, this);
            }
            __first = __first->_next;
        }
        return iterator(nullptr, this);
    }

    size_type count(const key_type& __k) {
        const size_type __buckets_index = get_bucket_index_key(__k);
        node_ptr __first = _buckets[__buckets_index];
        size_type __result = 0;

        while(__first) {
            if(_equals(_get_key(__first->_value), __k)) {
                ++__result;
            }
            __first = __first->_next;
        }
        return __result;
    }

    void clear() {
        for(size_type __i = 0; __i < _buckets.size(); ++__i) {
            erase_bucket(__i);
        }
    }
};

/**
 * @description: 首先判断是否需要重新hash（需要的桶数发生变化），如果不需要，什么都不做，
 *               如果需要，先得到新的桶数并构造新的桶vector，然后将原有的每个node重新
 *               移动到新的桶中；
 * @param {type} 
 * @return: 
 */
template<class Value, class Key, class HashFunc, class ExtractKey,
         class EqualKey, class Alloc>
void hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
::rehash(size_type __n_new_elt) {
    const size_type __old_buckets_num = _buckets.size();
    size_type __new_buckets_num = need_rehash(__old_buckets_num, __n_new_elt);
    
    if(__new_buckets_num > 0) {
        vector<node_ptr> __temp(__new_buckets_num, nullptr);
        for(size_type __i = 0; __i < __old_buckets_num; ++__i) {
            node_ptr __first = _buckets[__i];
            while(__first) {
                size_type __new_bucket = get_bucket_index(__first, 
                                                          __new_buckets_num);
                _buckets[__i] = __first->_next;
                __first->_next = __temp[__new_bucket];
                __temp[__new_bucket] = __first;
                __first = _buckets[__i];
            }
        }

        _buckets.swap(__temp);
    }
}

template<class Value, class Key, class HashFunc, class ExtractKey,
         class EqualKey, class Alloc>
std::pair<typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
::iterator, bool> hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
::insert_unique_core(const value_type& __x) {
    const size_type __bucket_index = get_bucket_index(__x);
    node_ptr __first = _buckets[__bucket_index];

    node_ptr __cur = __first;
    while(__cur) {
        if(_equals(_get_key(__cur->_value), _get_key(__x))) {
            // zhangrui
            return std::pair<iterator, bool>(iterator(__cur, this), false);
        }
        __cur = __cur->_next;
    }

    node_ptr __new_node = create_one_node(__x);
    __new_node->_next = __first;
    _buckets[__bucket_index] = __new_node;
    ++_node_count;
    return std::pair<iterator, bool>(iterator(__new_node, this), true);
}

template<class Value, class Key, class HashFunc, class ExtractKey,
         class EqualKey, class Alloc>
typename hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
::iterator hashtable<Value, Key, HashFunc, ExtractKey, EqualKey, Alloc>
::insert_multi_core(const value_type& __x) {
    const size_type __bucket_index = get_bucket_index(__x);
    node_ptr __first = _buckets[__bucket_index];
    node_ptr __new_node = create_one_node(__x);

    node_ptr __cur = __first;
    while(__cur) {
        // 如果发现相同的元素，就将新节点插入该元素下方
        if(_equals(_get_key(__cur->_value), _get_key(__x))) {
            __new_node->_next = __cur->_next;
            __cur->_next = __new_node;
            ++_node_count;
            return iterator(__cur, this);
        }
        __cur = __cur->_next;
    }

    // 如果没有找到相同的元素，就将新节点插入链表头部
    __new_node->_next = __first;
    _buckets[__bucket_index] = __new_node;
    ++_node_count;
    return iterator(__new_node, this);
}

};

#endif