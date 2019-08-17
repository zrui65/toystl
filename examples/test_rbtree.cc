#include <iostream>
#include <functional>

#include "toy_tree.h"

using namespace toy;

int main() {
    rbtree<int, int, std::_Identity<int>, std::less<int>> rbt1;
    
    for(int i = 1; i < 100; i++) {
        rbt1.insert_unique(i);
    }

    rbt1 = rbt1;

    auto iter2 = rbt1.find(99);
    std::cout << *iter2 << std::endl;

    for(auto iter1 = rbt1.begin(); iter1 != rbt1.end(); ++iter1) {
        std::cout << *iter1 << " ";
    }
    std::cout << std::endl;

    rbt1.clear();

    for(int i = 1; i < 100; i++) {
        rbt1.insert_unique(i);
    }

    iter2 = rbt1.find(99);
    std::cout << *iter2 << std::endl;

    for(auto iter1 = rbt1.begin(); iter1 != rbt1.end(); ++iter1) {
        std::cout << *iter1 << " ";
    }
    std::cout << std::endl;

}


