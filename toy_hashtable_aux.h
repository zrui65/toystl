/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-07-04 22:42:14
 * @LastEditTime: 2019-07-06 17:49:49
 * @LastEditors: Please set LastEditors
 */

#ifndef __TOY_HASHTABLE_AUX__
#define __TOY_HASHTABLE_AUX__

#include <algorithm>

namespace toy {

enum { _num_primes = 29 };

const unsigned long prime_list[_num_primes] =
{
    5ul,          53ul,         97ul,         193ul,       389ul,
    769ul,        1543ul,       3079ul,       6151ul,      12289ul,
    24593ul,      49157ul,      98317ul,      196613ul,    393241ul,
    786433ul,     1572869ul,    3145739ul,    6291469ul,   12582917ul,
    25165843ul,   50331653ul,   100663319ul,  201326611ul, 402653189ul,
    805306457ul,  1610612741ul, 3221225473ul, 4294967291ul
};

inline unsigned long
next_prime(unsigned long __n)
{
    const unsigned long* __first = prime_list;
    const unsigned long* __last = __first + static_cast<int>(_num_primes);
    const unsigned long* pos = std::lower_bound(__first, __last, __n);
    return pos == __last ? *(__last - 1) : *pos;
}

};

#endif