/*
 * @Description:
 * 发现vector的主要数据只有三个指针（_start,_end,_finish）用来管理一块分配的内存；
 * 
 * TODO: 移动构造函数，异常安全，insert函数等等；
 */

#ifndef __TOY_TOYVECTOR__
#define __TOY_TOYVECTOR__

#include <new>
#include <stdlib.h>
#include <iostream>

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

    iterator alloc_and_fill(size_type n, const value_type& x) {
        iterator result = data_alloc::allocate(n);
        free_area_fill_n(result, n, x);
        return result;
    }

    void fill_init(size_type n, const value_type& x) {
        _start = alloc_and_fill(n, x);
        _end = _start + n;
        _finish = _end;
    }

    void deallocate() {
        if(_start)
            data_alloc::deallocate(_start, _finish - _start);
    }

    void realloc_and_push_back(iterator p, const value_type& x);

public:
    /*构造函数*/
    vector():_start(nullptr), _end(nullptr), _finish(nullptr) {}
    vector(size_type n, const value_type& value) { fill_init(n, value); }
    vector(int n, const value_type& value) { fill_init(n, value); }
    vector(long n, const value_type& value) { fill_init(n, value); }
    explicit vector(size_type n) { fill_init(n, T()); }

    /*拷贝构造函数*/
    // 参数中的vector其实省略了模板参数T
    vector(const vector& x) {
        _start = data_alloc::allocate(x.capacity());
        _end = free_area_copy(x.begin(), x.end(), _start);
        _finish = _start + x.capacity();
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
    reference operator[](size_type idex) const {
        /*注意：idex超出范围是未定义的(参考g++)，应该是仿照原生数据类型 */
        return *(_start + idex);
    }

    /*返回值类型无法作为区分函数重载的依据，
    * 所以下面这个函数与上面那个函数，在编译器看来是一样的 
    */
    // const reference operator[](size_type idex) const {
    //     if(_start + idex >= _end) {
    //         std::cerr << "out of range" << std::endl;
    //     }
    //     return *(_start + idex);
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

    vector& operator=(const vector& x) {
        // 注意：自我赋值
        if(this == &x) return *this;

        vector temp(x);
        // 仅仅将临时vector的数据结构与this交换
        // 原this管理的内存将交给temp
        // 在函数退出时，temp将原内存释放
        swap(*this, temp);

        return *this;
        // 函数返回时，temp执行析构函数，将内存释放
    }

    /*返回值是const引用类型的， 所以不可以对其赋值*/
    reference at(size_type idex) const {
        if(_start + idex >= _end) {
            std::cerr << "out of range" << std::endl;
        }
        return *(_start + idex);
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
void vector<T, alloc>::realloc_and_push_back(iterator p, const value_type& x) {
    if(_end != _finish) {     // 还有后备空间
        construct(_end, x);
        ++_end;
    }
    else {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;

        iterator new_start = data_alloc::allocate(len);
        iterator new_end = new_start;

        try {
            new_end = free_area_copy(_start, p, new_start);
            construct(new_end, x);
            ++new_end;
        }
        catch(...) {
            destory(new_start, new_end);
            data_alloc::deallocate(new_start, len);
            throw;
        }

        destory(begin(), end());
        deallocate();

        _start = new_start;
        _end = new_end;
        _finish = new_start + len;
    }
}

/*不能使用reference替换value_type&，会使得例如当value_type为int时，
* 无法直接使用常量数字来调用此函数
*/
template<typename T, class alloc>
void vector<T, alloc>::push_back(const value_type& x) {
    if(_end != _finish) {
        construct(end(), x);
        ++_end;
    }
    else {
        realloc_and_push_back(end(), x);
    }
}

template<typename T, class alloc>
void vector<T, alloc>::pop_back() {
    --_end;
    destory(_end);
}

template<typename T, class alloc>
typename vector<T, alloc>::iterator vector<T, alloc>::erase(iterator p) {
    if(p + 1 != _end)
        copy(p + 1, _end, p);
    --_end;
    destory(_end);
    return p;
}

template<typename T, class alloc>
typename vector<T, alloc>::iterator vector<T, alloc>::erase(iterator first, iterator last) {
    copy(last-1, _end, first);
    destory(first + (_end - last), _end);
    _end = _end - (last - first);
    return first;
}

// template<typename T, class alloc>
// void vector<T, alloc>::insert(iterator p, size_type n, const reference x) {

// }

}

#endif