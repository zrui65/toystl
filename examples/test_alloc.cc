#include "toy_alloc.h"
#include <iostream>

using namespace toy;

int main() {
    int n = 10;

    toyalloc<int> alloc;

    auto p = alloc.allocate(n);
    for(int i = 0; i < n; i++) {
        toy::construct(p+i, i);
        p[i] = i;
    }

    for(int i = 0; i < n; i++) {
        std::cout << *(p+i) << " ";
    }
    std::cout << std::endl;


    auto p1 = alloc.allocate(n * 2);
    free_area_copy(p, p+n, p1);

    for(int i = 0; i < n*2; i++) {
        std::cout << *(p1+i) << " ";
    }
    std::cout << std::endl;

    toy::destory(p, p + n);
    alloc.deallocate(p, n);
    toy::destory(p1, p1 + n);
    alloc.deallocate(p1, n*2);
}


