#include <iostream>

#include "toy_list.h"

using namespace toy;

void printL(const list<int>& l) {
    for(auto iter = l.begin(); iter != l.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}

int main() {
    list<int> l1;

    for(int i = 0; i < 10; i++) {
        l1.push_back(i);
    }
    printL(l1);

    list<int> l2(l1); // 0 1 2 3 4 5 6 7 8 9
    printL(l2);
    l2.erase(++l2.begin(), --l2.end()); // 0 9
    printL(l2);
    l2.pop_back(); // 0
    printL(l2);
    l2.clear(); // 
    printL(l2);
    l2.push_back(8); // 8
    printL(l2);
    l2.push_front(2); // 2 8
    printL(l2);
    l2.pop_front();  // 8
    printL(l2);
    // l2.erase(l2.begin(), ++l2.begin());  // 8
    // printL(l2);
    
    list<int> l3 = l1;
    printL(l3);

    std::cout<< l3.size() << std::endl;
    // l3.reverse();
    printL(l3);
    printL(l1);
    l3.merge(l1);
    printL(l3);
    printL(l1);
    l1.swap(l2);
    printL(l1);

    l3.reverse();
    printL(l3);
    l3.sort();
    printL(l3);
    // while(1);
}


