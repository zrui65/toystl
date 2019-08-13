/*
 * 本来准备将分配器与构造器放在同一个模板类中，每种容器特化一个，
 * 但是后来发现，分配器是与模板中的元素类型相关，每次分配器malloc一个
 * 元素空间，free一个元素空间；而构造器是与容器的迭代器相关联，构造器
 * “销毁”一个元素可以使用该元素的地址，但是“销毁”一连串元素，必须使用
 * 迭代器，因为不只有vector这样的连续内存分布的容器，还有list这样的
 * 不连续内存分布的容器；同时三个内存处理函数也是同样的道理，必须使用
 * 迭代器
 */

#ifndef __TOY_TOYALLOC__
#define __TOY_TOYALLOC__

#include <new>
#include <stdlib.h>

namespace toy {

/*
* 空间分配器与“传入容器的元素类型”相关
*/

template<typename T>
class toyalloc {
public:
    typedef T                   value_type;
    typedef value_type*         pointer;
    typedef const value_type*   const_pointer;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef size_t              size_type;
    typedef ptrdiff_t           defference_type;

public:
    static pointer allocate(size_type __n) {
        return static_cast<pointer>(malloc(__n * sizeof(value_type)));
    };

    static void deallocate(pointer __p, size_type) {
        free(__p);
    };
};

/*
* 空间构造器与“容器的"迭代器"”相关
*/
template<typename T>
inline void construct(T* __p, const T& __value) {
    new (__p) T(__value);
}

template<typename T>
inline void destory(T* __p) {
    __p->~T();
}

template<typename ForwardIterator>
inline void destory(ForwardIterator __first, ForwardIterator __last) {
    ForwardIterator cur = __first;
    for(;cur != __last; cur++) {
        // *cur取回迭代器所指对象
        // &*cur对迭代器所指对象取址
        destory(&*cur);
    }
}

/*
* 内存处理函数与“容器的"迭代器"”相关
*/
template<typename ForwardIterator, typename InputIterator>
inline ForwardIterator free_area_copy(InputIterator __first, InputIterator __last,
                        ForwardIterator result) {
    ForwardIterator cur = result;
    for( ; __first != __last; ++__first, ++cur) {
        construct(&*cur, *__first);
    }
    return cur;
}

template<typename ForwardIterator, typename T>
inline void free_area_fill(ForwardIterator __first, ForwardIterator __last,
                        const T& x) {
    for( ; __first != __last; ++__first) {
        construct(&*__first, x);
    }
}

template<typename ForwardIterator, typename Size, typename T>
inline ForwardIterator free_area_fill_n(ForwardIterator __first, Size n,
                        const T& x) {
    ForwardIterator cur = __first;
    for( ; n > 0; --n, ++cur) {
        construct(&*cur, x);
    }
    return cur;
}

}

#endif