/*
 * @Description:
 * string构造时，默认位置（_data指针）指向栈空间，构造过程中如果发现字符串大于15B，
 * 则在堆中分配空间，并将_data指针指向堆；
 */


#ifndef __TOY_STRING__
#define __TOY_STRING__

#include <iostream>
#include <assert.h>

#include <toy_alloc.h>
#include <toy_algo.h>

namespace toy {

class string {
public:
    typedef char value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t size_type;
    typedef ptrdiff_t defference_type;

private:
    pointer _data; // 指向字符数据的指针，既可以指向堆空间，也可以指向栈空间
    typedef toyalloc<char> data_alloc;

    size_t _length; // 此变量用于描述字符串长度，可能是在栈上，也可能是在堆中

    enum { _local_capacity = 15 };
    union
    {
        value_type  _local_buf[_local_capacity + 1]; // 栈上的数据空间
        size_type   _capacity; // 栈空间不使用时，用作变量_capacity，此变量用于描述堆空间大小
    };



    // 用来明确对_data的读写，防止误操作
    iterator data() const { return iterator(_data); }
    void data(iterator __p) { _data = pointer(__p); }

    iterator local_data() const { return pointer(_local_buf); }

    void capacity(size_type __capacity) { _capacity = __capacity; }

    void set_length(size_type __n) {
        _length = __n;
        data()[__n] = value_type(); // 在字符串尾添加'\0'
    }


    void alloc_and_fill(iterator __beg, iterator __end) {
        size_type __capacity = static_cast<size_type>(__end - __beg);

        // 如果要构造的字符串长度大于栈空间,
        // 则在堆上分配一块内存，并将_data从栈空间移动到堆空间
        if(__capacity > size_type(_local_capacity)) {
            data(create(__capacity, size_type(0)));
            capacity(__capacity);
        }
        
        free_area_copy(__beg, __end, data());
        set_length(__capacity);
    }

    void alloc_and_fill(size_type __n, value_type __c) {
        // 如果要构造的字符串长度大于栈空间
        if(__n > size_type(_local_capacity)) {
            data(create(__n, size_type(0)));
            capacity(__n);
        }
        
        free_area_fill_n(data(), __n, __c);
        set_length(__n);
    }

    // 注意：__capacity是以引用形式传入的，经过create函数之后，将被改变为真正的容量大小，
    // 使用create时需要注意，如果不想被改变，可传入一个临时值
    iterator create(size_type& __capacity, size_type __old_capacity) {
        if (__capacity > __old_capacity && __capacity < 2 * __old_capacity) {
            __capacity = 2 * __old_capacity;
        }

        return data_alloc::allocate(__capacity + 1);
    }

    void dispose() {
        if(!is_local()) {
            data_alloc::deallocate(data(), _capacity);
        }
    }

    bool is_local() const {
        return data() == local_data();
    }
    size_type chars_length(const value_type* __str) {
        int __i = 0;
        while(__str[__i] != value_type()) {
            ++__i;
        }
        return __i;
    }

    void erase_from(size_type __pos, size_type __n);

public:
    /*构造函数*/
    /*
    * 构造函数的初始化列表均将_data指针指向栈空间，
    * 在进一步构造的时候（函数construct中），可能移到堆空间
    */
    string(): _data(_local_buf) {
        set_length(0);
    }

    string(const value_type* __chars): _data(_local_buf) {
        size_type __chars_length = chars_length(__chars);

        alloc_and_fill(iterator(__chars), iterator(__chars + __chars_length));
    }
    
    string(size_type __n, const value_type __c): _data(_local_buf) { 
        alloc_and_fill(__n, __c);
    }

    /*拷贝构造函数*/
    string(const string& __str): _data(_local_buf) {
        alloc_and_fill(__str.data(), __str.data() + __str.length());
        set_length(__str._length);
    }

    /*析构函数*/
    ~string() { dispose(); }


    size_type size() const { return _length; }
    size_type length() const { return size(); }
    size_type capacity() const {
        return is_local() ? size_type(_local_capacity)
                          : _capacity;
    }

    bool empty() const { return size() == 0; }

    iterator begin() const { return data(); };
    iterator end() const { return data() + size(); };

    reference operator[](size_type __idex) const {
        assert(__idex < size());
        return data()[__idex];
    }

    string& operator=(const string& __str) {
        return this->assign(__str);
    }

    string& operator=(const value_type* __chars) {
        return this->assign(__chars);
    }

    string& operator+=(const value_type __c) {
        this->push_back(__c);
        return *this;
    }

    string& operator+=(const string& __str) {
        this->append(__str);
        return *this;
    }

    string& operator+=(const value_type* __chars) {
        this->append(__chars);
        return *this;
    }

    reference at(size_type __idex) const {
        if(__idex >= size()) {
            std::cerr << "out of range" << std::endl;
        }
        return data()[__idex];
    }

    /*返回值是const引用类型的， 所以不可以对其赋值*/
    const reference front() const {
        assert(!empty());
        return operator[](0);
    }

    /*返回值是const引用类型的， 所以不可以对其赋值*/
    const reference back() const {
        assert(!empty());
        return operator[](size()-1);
    }

    void push_back(const value_type);
    void pop_back();

    string& assign(const string&);
    string& assign(const value_type*);
    string& append(const string&);
    string& append(const value_type*);

    iterator erase(iterator);
    iterator erase(iterator, iterator);
    void clear() { erase(begin(), end()); }

    value_type* c_str() {
        return data();
    }

};

void string::push_back(const value_type __c) {
    if(size()+1 > capacity()) {
        size_type __old_capacity = capacity();
        size_type __new_capacity = capacity() + 1;
        iterator __temp = create(__new_capacity, __old_capacity);

        free_area_copy(begin(), end(), __temp);
        dispose();
        data(__temp);
        capacity(__new_capacity);
    }

    // 无论_data[size()]的位置上是否已经被构造，都可以使用这行代码；
    // 没有被构造可以当成构造函数，已经构造可以直接赋值
    // 其实，_data[size()]的位置一定是已经被构造过的，因为存放着'\0'
    _data[size()] = __c;
    set_length(size() + 1);
}

void string::pop_back() {
    set_length(size() - 1);
}

string& string::assign(const string& __str) {
    if(this == &__str) return *this;

    // 如果当前的capacity()小于__str.size()，说明__str.size()至少大于15，
    // 因为capacity()>=15，从而证明__str的数据放在堆中
    size_type __rsize = __str.size();
    size_type __capacity = capacity();

    if(__capacity < __rsize) {
        // 传入create的第一个参数是以引用形式传入的，有可能被改变；__rsize后面被用于
        // 设置字符串长度，所以不允许被改变，所以使用一个临时值
        size_type __new_capacity = __rsize;
        // 使用一个临时变量来分配内存，分配失败也不会影响原来的数据结构
        iterator __temp = create(__new_capacity, __capacity);

        free_area_copy(__str.begin(), __str.end(), __temp);
        dispose();
        data(__temp);
        capacity(__new_capacity);
    }
    else {
        // 这种情况表示原有的空间足以容纳新的数据，直接copy即可
        copy(__str.begin(), __str.end(), data());
    }

    set_length(__rsize);

    return *this;
}

string& string::assign(const value_type* __chars) {

    // 如果当前的capacity()小于__str.size()，说明__str.size()至少大于15，
    // 因为capacity()>=15，从而证明__str的数据放在堆中
    size_type __rsize = chars_length(__chars);
    size_type __capacity = capacity();

    if(__capacity < __rsize) {
        // 传入create的第一个参数是以引用形式传入的，有可能被改变；__rsize后面被用于
        // 设置字符串长度，所以不允许被改变，所以使用一个临时值
        size_type __new_capacity = __rsize;
        // 使用一个临时变量来分配内存，分配失败也不会影响原来的数据结构
        iterator __temp = create(__new_capacity, __capacity);

        free_area_copy(__chars, __chars + __rsize, __temp);
        dispose();
        data(__temp);
        capacity(__new_capacity);
    }
    else {
        // 这种情况表示原有的空间足以容纳新的数据，直接copy即可
        copy(__chars, __chars + __rsize, data());
    }

    set_length(__rsize);

    return *this;
}

string& string::append(const string& __str) {
    size_type __size = __str.size();
    size_type __new_length = __size + size();
    if(__new_length > capacity()) {
        size_type __new_capacity = __size + size();
        iterator __temp = create(__new_capacity, capacity());

        free_area_copy(begin(), end(), __temp);
        dispose();
        data(__temp);
        capacity(__new_capacity);
    }

    free_area_copy(__str.begin(), __str.end(), end());
    set_length(__new_length);
    return *this;
}

string& string::append(const value_type* __chars) {
    size_type __size = chars_length(__chars);
    size_type __new_length = __size + size();
    if(__new_length > capacity()) {
        size_type __new_capacity = __size + size();
        iterator __temp = create(__new_capacity, capacity());

        free_area_copy(begin(), end(), __temp);
        dispose();
        data(__temp);
        capacity(__new_capacity);
    }

    free_area_copy(__chars, __chars + __size, end());
    set_length(__new_length);
    
    return *this;
}

void string::erase_from(size_type __pos, size_type __n) {
    assert(__pos + __n <= size());

    copy(begin() + __pos + __n, end(), begin() + __pos);
    set_length(length() - __n);
}

string::iterator string::erase(iterator __p) {
    assert(__p >= begin() && __p < end());
    erase_from(size_type(__p - begin()), size_type(1));
    return __p;
}

string::iterator string::erase(iterator __first, iterator __last) {
    assert(__first >= begin() && __first <= __last && __last <= end());
    erase_from(size_type(__first - begin()), size_type(__last - __first));
    return __first;
}


// 非成员函数
string operator+(const string& __lhs, const string& __rhs) {
    string __temp(__lhs);
    __temp += __rhs;
    return __temp;
}

string operator+(const string::value_type* __lhs, const string& __rhs) {
    string __temp(__lhs);
    __temp += __rhs;
    return __temp;
}

string operator+(const string& __lhs, const string::value_type* __rhs) {
    string __temp(__lhs);
    __temp += __rhs;
    return __temp;
}

std::ostream& operator<<(std::ostream& __os, string& __str) {
    __os << __str.c_str();
    return __os;
}

// __str采用右值引用，可输出临时值
std::ostream& operator<<(std::ostream& __os, string&& __str) {
    __os << __str.c_str();
    return __os;
}

std::istream& operator>>(std::istream& __is, string& __str) {
    
    return __is;
}

}

#endif