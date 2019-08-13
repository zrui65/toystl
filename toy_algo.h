
#ifndef __TOY_ALGO__
#define __TOY_ALGO__

#include <toy_algo_base.h>
#include <toy_iterator.h>
#include <toy_type_traits.h>

namespace toy {

/**
 * @description: 
 * @param __first：指向堆的顶部
 *        __holeIndex：待插入点与__first的距离
 *        __topIndex：“局部堆”顶点与__first的距离
 *        __value：待插入点的值
 *        __comp：比较函数
 * @return: 
 */
template<typename RandomIter, typename Distance, typename Tp,
         typename Compare>
void __push_heap(RandomIter __first, Distance __holeIndex,
                 Distance __topIndex, Tp __value, Compare& __comp) {
    // 找到待插入点的父节点
    Distance __parent = (__holeIndex - 1) / 2;
    RandomIter __pv(&__value);

    while(__holeIndex > __topIndex && __comp(__first + __parent, __pv)) {
        *(__first + __holeIndex) = *(__first + __parent);
        __holeIndex = __parent;
        __parent = (__holeIndex - 1) / 2;
    }

    *(__first + __holeIndex) = __value;
}

template<typename RandomIter, typename _Distance,
         typename _Tp, typename Compare>
void __adjust_heap(RandomIter __first, _Distance __holeIndex,
        _Distance __len, _Tp __value, Compare __comp) {

    const _Distance __topIndex = __holeIndex;
    _Distance __secondChild = __holeIndex;

    while (__secondChild < (__len - 1) / 2) {
        __secondChild = 2 * (__secondChild + 1);
        if (__comp(__first + __secondChild,
                __first + (__secondChild - 1)))
        __secondChild--;
        *(__first + __holeIndex) = *(__first + __secondChild);
        __holeIndex = __secondChild;
    }

    if ((__len & 1) == 0 && __secondChild == (__len - 2) / 2) {
        __secondChild = 2 * (__secondChild + 1);
        *(__first + __holeIndex) = *(__first + (__secondChild - 1));
        __holeIndex = __secondChild - 1;
    }

    __push_heap(__first, __holeIndex, __topIndex, __value, __comp);
}

template<typename RandomIter, typename Compare>
inline void __pop_heap(RandomIter __first, RandomIter __last,
                       RandomIter __result, Compare& __comp) {
    typedef typename iterator_traits<RandomIter>::value_type
                       _ValueType;
    typedef typename iterator_traits<RandomIter>::difference_type
                       _DistanceType;

    _ValueType __value = *__result;
    *__result = *__first;
    __adjust_heap(__first, _DistanceType(0),
                  _DistanceType(__last - __first), __value, __comp);
}

template<class RandomIter, class Compare>
void __make_heap(RandomIter __first, RandomIter __last,
                 Compare __comp, random_access_iterator_tag) {
    typedef typename iterator_traits<RandomIter>::value_type
                    _ValueType;
    typedef typename iterator_traits<RandomIter>::difference_type
                    _DistanceType;

    if (__last - __first < 2) return;

    const _DistanceType __len = __last - __first;
    _DistanceType __parent = (__len - 2) / 2;
    while (true)
    {
        _ValueType __value = *(__first + __parent);
        __adjust_heap(__first, __parent, __len, __value, __comp);
        if (__parent == 0) return;
        __parent--;
    }
}

template<class RandomIter, class Compare>
void make_heap(RandomIter __first, RandomIter __last,
               Compare __comp) {
    __make_heap(__first, __last, __comp,
                iterator_category(__first));
}

template<typename RandomIter, typename Compare>
void __heap_select(RandomIter __first, RandomIter __middle,
                   RandomIter __last, Compare __comp) {
    // 以从小到大排序为例
    // 先将[__first, __middle]区间内的元素做成最大堆
    make_heap(__first, __middle, __comp);
    // 然后将[__middle， __last]区间内的元素依次插入
    for (RandomIter __i = __middle; __i < __last; ++__i)
        // __first是[__first, __middle]区间内最大的元素
        if (__comp(__i, __first))
            // 如果__i < __first，那么弹出__first
            __pop_heap(__first, __middle, __i, __comp);
}

template<typename RandomIter, typename Compare>
void __sort_heap(RandomIter __first, RandomIter __last,
                 Compare& __comp)
{
    while (__last - __first > 1)
    {
        --__last;
        __pop_heap(__first, __last, __last, __comp);
    }
}

/**
 * @description: 先使用堆的特性将前__middle-__first个较小的元素放在前半区间内，
 *               然后使用堆排序将其排序
 * @param {type} 
 * @return: 
 */
template<typename RandomIter, typename Compare>
inline void __partial_sort(RandomIter __first, RandomIter __middle,
                           RandomIter __last, Compare __comp)
{
    // 此函数使得区间[__first, __middle]内的元素都小于
    // [__middle， __last]区间内的元素，并且构成最大堆或最小堆
    __heap_select(__first, __middle, __last, __comp);
    // 将[__first, __middle]内的元素进行堆排序
    __sort_heap(__first, __middle, __comp);
}

template<typename RandomIter>
inline void partial_sort(RandomIter __first, RandomIter __middle,
                         RandomIter __last) {
    __partial_sort(__first, __middle, __last, _Iter_less_iter());
}

template<typename Iterator, typename Compare>
void __move_median_to_first(Iterator __result,Iterator __a, Iterator __b,
            Iterator __c, Compare __comp) {
    if (__comp(__a, __b))
    {
        if (__comp(__b, __c))
            iter_swap(__result, __b);
        else if (__comp(__a, __c))
            iter_swap(__result, __c);
        else
            iter_swap(__result, __a);
    }
    else if (__comp(__a, __c))
        iter_swap(__result, __a);
    else if (__comp(__b, __c))
        iter_swap(__result, __c);
    else
        iter_swap(__result, __b);
}

template<typename RandomIter, typename Compare>
RandomIter __unguarded_partition(RandomIter __first,
        RandomIter __last, RandomIter __pivot, Compare __comp) {
    while (true)
    {
        while (__comp(__first, __pivot)) ++__first;

        --__last;

        while (__comp(__pivot, __last)) --__last;

        if (!(__first < __last)) return __first;

        iter_swap(__first, __last);

        ++__first;
    }
}

template<typename RandomIter, typename _Compare>
inline RandomIter __unguarded_partition_pivot(
    RandomIter __first, RandomIter __last, _Compare __comp) {
    
    RandomIter __mid = __first + (__last - __first) / 2;
    // 选取__first+1, __mid, __last-1三个位置当中的中间值作为此次排序的基准值，
    // 并将结果存放在__first位置上
    __move_median_to_first(__first, __first + 1, __mid, __last - 1,
                __comp);
    // 进行一次划分
    return __unguarded_partition(__first + 1, __last, __first, __comp);
}

enum { _S_threshold = 16 };

template<class RandomIter, class Size, class Compare>
void __introsort_loop(RandomIter __first, RandomIter __last,
                      Size __depth_limit, Compare __comp) {
    while (__last - __first > int(_S_threshold)) {
        if (__depth_limit == 0) {
            std::cout << "__partial_sort" << std::endl;
            // 其实是全排序，只不过与partial_sort共用了__partial_sort函数
            __partial_sort(__first, __last, __last, __comp);
            return;
        }
        --__depth_limit;

        RandomIter __cut =
            __unguarded_partition_pivot(__first, __last, __comp);
        __introsort_loop(__cut, __last, __depth_limit, __comp);
        __last = __cut;
	}
}

template<class Size>
inline Size __lg(Size __n) {
    Size __k;
    for(__k = 0; __n > 1; __n >>= 1) ++__k;
    return __k;
}

template<typename RandomIter, typename Compare>
void __unguarded_linear_insert(RandomIter __last,
                               Compare __comp) {
    typename iterator_traits<RandomIter>::value_type
    __val = *__last;
    RandomIter __pv(&__val);

    RandomIter __next = __last;
    --__next;
    while (__comp(__pv, __next)) {
        *__last = *__next;
        __last = __next;
        --__next;
    }
    *__last = __val;
}

template<typename RandomIter, typename Compare>
void __insertion_sort(RandomIter __first,
            RandomIter __last, Compare __comp) {
    if (__first == __last) return;

    for (RandomIter __i = __first + 1; __i != __last; ++__i) {
        if (__comp(__i, __first)) {
            typename iterator_traits<RandomIter>::value_type
            __val = *__i;
            __copy_move_b(__first, __i, __i + 1,
                          iterator_category(__first));
            *__first = __val;
        }
        else
        __unguarded_linear_insert(__i, __comp);
    }
}

template<typename RandomIter, typename Compare>
inline void __unguarded_insertion_sort(RandomIter __first,
                RandomIter __last, Compare __comp)
{
    for (RandomIter __i = __first; __i != __last; ++__i)
        __unguarded_linear_insert(__i, __comp);
}


template<typename RandomIter, typename Compare>
void __final_insertion_sort(RandomIter __first,
            RandomIter __last, Compare __comp) {
    if (__last - __first > int(_S_threshold)) {
        // 分为两端进行插入排序
        // 第一段：从0～_S_threshold
        // 由于快排的特性，此区间内存在整个序列的最值，在插入排序时每次比较都需要确定是否
        // 越界，即是否超出first之前
        __insertion_sort(__first, __first + int(_S_threshold), __comp);
        // 第二段：从_S_threshold～end
        // 由于第一段已经存在最值，所以此区间内的排序仅仅比较大小就可以，因为在越界之前必然
        // 遇到使此次排序停止的较小值或较大值
        __unguarded_insertion_sort(__first + int(_S_threshold), __last,
                    __comp);
    }
    else
        __insertion_sort(__first, __last, __comp);
}

template<class RandomIter, class Compare>
void __sort(RandomIter __first, RandomIter __last,
            Compare __comp, random_access_iterator_tag) {
    if(__first != __last) {
	    __introsort_loop(__first, __last,
				         __lg(__last - __first) * 2,
				         __comp);
	    __final_insertion_sort(__first, __last, __comp);
    }
}

template<class RandomIter>
void sort(RandomIter __first, RandomIter __last) {

    __sort(__first, __last, _Iter_less_iter(),
           iterator_category(__first));
}

template<class RandomIter, class Compare>
void sort(RandomIter __first, RandomIter __last, Compare __comp) {
    __sort(__first, __last, __comp, __comp, 
           iterator_category(__first));
}

}

#endif