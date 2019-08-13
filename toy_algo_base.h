
#ifndef __TOY_ALGO_BASE__
#define __TOY_ALGO_BASE__

#define __DEBUG__ 1
#ifdef __DEBUG__
#include <iostream>
#endif

#include <string.h>
#include <toy_iterator.h>
#include <toy_type_traits.h>

namespace toy {

struct _Iter_less_iter
{
    template<typename _Iter1, typename _Iter2>
    constexpr bool operator()(_Iter1 __it1, _Iter2 __it2) const {
        return (*__it1) < (*__it2); 
    }
};

template<class Value>
void swap(Value& __x, Value& __y) {
    Value __temp = __x;
    __x = __y;
    __y = __temp;
}

template<typename _Tp>
constexpr inline const _Tp&
min(const _Tp& __a, const _Tp& __b)
{
    if (__b < __a)
        return __b;
    return __a;
}

template<typename _Tp>
constexpr inline const _Tp&
max(const _Tp& __a, const _Tp& __b)
{
    if (__a < __b)
        return __b;
    return __a;
}

template<typename _Tp, typename Compare>
constexpr inline const _Tp&
min(const _Tp& __a, const _Tp& __b, Compare __comp)
{
    if (__comp(__b, __a))
        return __b;
    return __a;
}

template<typename _Tp, typename Compare>
constexpr inline const _Tp&
max(const _Tp& __a, const _Tp& __b, Compare __comp)
{
    if (__comp(__a, __b))
        return __b;
    return __a;
}

template<class RandomIter, class OutputIter>
OutputIter __copy_move_b(RandomIter __first, RandomIter __last,
                  OutputIter __result, random_access_iterator_tag) {
    while (__first != __last)
    *--__result = *--__last;
    return __result;
}

template<class Iterator>
void iter_swap(Iterator __iter1, Iterator __iter2) {
    swap(*__iter1, *__iter2);
}

template<class RandomIter, class OutputIter, class Distance>
OutputIter __copy_d(RandomIter __first, RandomIter __last,
                  OutputIter __result, Distance*) {
    for(Distance __n = __last - __first; __n > 0;
        --__n, ++__result, ++__first) {
        *__result = *__first;
    }
    return __result;
}

template<class InputIter, class OutputIter>
OutputIter __copy(InputIter __first, InputIter __last,
                  OutputIter __result, input_iterator_tag) {
#ifdef __DEBUG__
    std::cout << "input_iterator_tag" << std::endl;
#endif

    for( ;__first != __last; ++__result, ++__first) {
        *__result = *__first;
    }
    return __result;
}

template<class RandomIter, class OutputIter>
OutputIter __copy(RandomIter __first, RandomIter __last,
                  OutputIter __result, random_access_iterator_tag) {
#ifdef __DEBUG__
    std::cout << "random_access_iterator_tag" << std::endl;
#endif

    return __copy_d(__first, __last, __result, difference_type(__first));
}

template<class T>
T* __copy_t(const T* __first, const T* __last, T* __result, true_type) {
#ifdef __DEBUG__
    std::cout << "T*  has_trivial_assignment_operator->true_type" << std::endl;
#endif

    memmove(__result, __first, sizeof(T) * (__last - __first));
    return __result + (__last - __first);
}

template<class T>
T* __copy_t(const T* __first, const T* __last, T* __result, false_type) {
#ifdef __DEBUG__
    std::cout << "T*  has_trivial_assignment_operator->false_type" << std::endl;
#endif

    return __copy_d(__first, __last, __result, static_cast<ptrdiff_t*>(0));
}

template<class InputIter, class OutputIter>
OutputIter copy_dispatch(InputIter __first, InputIter __last,
                         OutputIter __result) {
    return __copy(__first, __last, __result,
                  iterator_category(__first));
}

template<class T>
T* copy_dispatch(T* __first, T* __last, T* __result) {
    typedef typename type_traits<T>::has_trivial_assignment_operator __t;
    return __copy_t(__first, __last, __result, __t());
}

template<class T>
T* copy_dispatch(const T* __first, const T* __last, T* __result) {
    typedef typename type_traits<T>::has_trivial_assignment_operator __t;
    return __copy_t(__first, __last, __result, __t());
}

template<class InputIter, class OutputIter>
inline OutputIter copy(InputIter __first, InputIter __last,
                OutputIter __result) {
    return copy_dispatch(__first, __last, __result);
}

// 针对char的特化版本
inline char* copy(char* __first, char* __last,
                           char* __result) {
#ifdef __DEBUG__
    std::cout << "char*" << std::endl;
#endif

    // memcpy假定两块区域没有重叠部分；
    // memmove可以拷贝有重叠的两块区域；
    memmove(__result, __first, __last - __first);
    return __result + (__last - __first);
}

}

#endif