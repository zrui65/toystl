/*
 * @Description:
 * 发现vector的主要数据只有三个指针（_start,_end,_finish）用来管理一块分配的内存；
 * 
 * TODO: 移动构造函数，异常安全，insert函数等等；
 */

#ifndef __TOY_TOYVECTOR__
#define __TOY_TOYVECTOR__

#include <toy_alloc.h>
#include <toy_iterator.h>
#include <toy_algo.h>

namespace toy {

template<typename T, class alloc = toyalloc<T>>
class vector{
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t size_type;
    typedef ptrdiff_t defference_type;

private:
    iterator _start;
    iterator _end;
    iterator _finish;
    typedef alloc data_alloc;

    iterator alloc_and_fill(size_type __n, const value_type& __x) {
        iterator __iter = data_alloc::allocate(__n);
        free_area_fill_n(__iter, __n, __x);
        return __iter;
    }

    void fill_init(size_type __n, const value_type& __x) {
        _start = alloc_and_fill(__n, __x);
        _end = _start + __n;
        _finish = _end;
    }

    void deallocate() {
        if(_start)
            data_alloc::deallocate(_start, _finish - _start);
    }

    void realloc_and_push_back(iterator __p, const value_type& __x);

public:
    /*构造函数*/
    vector():_start(nullptr), _end(nullptr), _finish(nullptr) {}
    vector(size_type __n, const value_type& __value) { fill_init(__n, __value); }
    vector(int __n, const value_type& __value) { fill_init(__n, __value); }
    vector(long __n, const value_type& __value) { fill_init(__n, __value); }
    explicit vector(size_type __n) { fill_init(__n, T()); }

    /*拷贝构造函数*/
    // 参数中的vector其实省略了模板参数T
    vector(const vector& __x) {
        _start = data_alloc::allocate(__x.capacity());
        _end = free_area_copy(__x.begin(), __x.end(), _start);
        _finish = _start + __x.capacity();
    }

    /*析构函数*/
    ~vector() {
        destory(_start, _end);
        deallocate();
    }

    /*注意：这些函数必须定义为const函数，意味着这些成员函数的this指针被const修饰;
    * 例如：size(const vector<T>* this)
    * 当vector<T>被const修饰时，这些函数才能被正常使用;
    * 从另一个角度来看，const函数不能修改成员变量，即那三个指针，可以想象除了
    * 像push_back这样涉及到数据增减的函数之外的所有对外接口
    * 均不能修改这三个指针！所以这些对外接口函数均需要以const修饰！
    */
    size_type size() const { return size_type(_end - _start); }
    size_type capacity() const { return size_type(_finish - _start); }
    bool empty() const { return _start == _end; }

    iterator begin() const { return _start; };
    iterator end() const { return _end; };
    // iterator rbegin() const { return _end - 1; }
    // iterator rend() const { return _start - 1; }

    /*返回值是non-const引用类型的， 所以可以对其赋值*/
    reference operator[](size_type __idex) const {
        /*注意：idex超出范围是未定义的(参考g++)，应该是仿照原生数据类型 */
        return *(_start + __idex);
    }

    /*返回值类型无法作为区分函数重载的依据，
    * 所以下面这个函数与上面那个函数，在编译器看来是一样的 
    */
    // const reference operator[](size_type __idex) const {
    //     if(_start + __idex >= _end) {
    //         std::cerr << "out of range" << std::endl;
    //     }
    //     return *(_start + __idex);
    // }

    void swap(vector& a, vector&b) {
        std::swap(a._start, b._start);
        std::swap(a._end, b._end);
        std::swap(a._finish, b._finish);
    }

    void swap(vector&b) {
        std::swap((*this)._start, b._start);
        std::swap((*this)._end, b._end);
        std::swap((*this)._finish, b._finish);
    }

    vector& operator=(const vector& __x) {
        // 注意：自我赋值
        if(this == &__x) return *this;

        vector temp(__x);
        // 仅仅将临时vector的数据结构与this交换
        // 原this管理的内存将交给temp
        // 在函数退出时，temp将原内存释放
        swap(*this, temp);

        return *this;
        // 函数返回时，temp执行析构函数，将内存释放
    }

    /*返回值是const引用类型的， 所以不可以对其赋值*/
    const reference at(size_type __idex) const {
        if(_start + __idex >= _end) {
            std::cerr << "out of range" << std::endl;
        }
        return *(_start + __idex);
    }

    /*返回值是const引用类型的， 所以不可以对其赋值*/
    const reference front() const {
        if(_end == _start) {
            std::cerr << "out of range" << std::endl;
        }
        return *_start;
    }

    /*返回值是const引用类型的， 所以不可以对其赋值*/
    const reference back() const {
        if(_end == _start) {
            std::cerr << "out of range" << std::endl;
        }
        return *(_end - 1);
    }

    void push_back(const value_type&);
    void pop_back();

    iterator erase(iterator);
    iterator erase(iterator, iterator);
    void clear() { erase(begin(), end()); }

    // void insert(iterator, size_type, const reference);
};

template<typename T, class alloc>
void vector<T, alloc>::realloc_and_push_back(iterator __p, const value_type& __x) {
    if(_end != _finish) {     // 还有后备空间
        construct(_end, __x);
        ++_end;
    }
    else {
        const size_type __old_size = size();
        const size_type __len = __old_size != 0 ? 2 * __old_size : 1;

        iterator __new_start = data_alloc::allocate(__len);
        iterator __new_end = __new_start;

        try {
            __new_end = free_area_copy(_start, __p, __new_start);
            construct(__new_end, __x);
            ++__new_end;
        }
        catch(...) {
            destory(__new_start, __new_end);
            data_alloc::deallocate(__new_start, __len);
            throw;
        }

        destory(begin(), end());
        deallocate();

        _start = __new_start;
        _end = __new_end;
        _finish = __new_start + __len;
    }
}

/*不能使用reference替换value_type&，会使得例如当value_type为int时，
* 无法直接使用常量数字来调用此函数
*/
template<typename T, class alloc>
void vector<T, alloc>::push_back(const value_type& __x) {
    if(_end != _finish) {
        construct(end(), __x);
        ++_end;
    }
    else {
        realloc_and_push_back(end(), __x);
    }
}

template<typename T, class alloc>
void vector<T, alloc>::pop_back() {
    --_end;
    destory(_end);
}

template<typename T, class alloc>
typename vector<T, alloc>::iterator vector<T, alloc>::erase(iterator __p) {
    if(__p + 1 != _end)
        copy(__p + 1, _end, __p);
    --_end;
    destory(_end);
    return __p;
}

template<typename T, class alloc>
typename vector<T, alloc>::iterator vector<T, alloc>::erase(iterator first, iterator last) {
    copy(last-1, _end, first);
    destory(first + (_end - last), _end);
    _end = _end - (last - first);
    return first;
}

// template<typename T, class alloc>
// void vector<T, alloc>::insert(iterator __p, size_type __n, const reference __x) {

// }

}

#endif