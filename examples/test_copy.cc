#include <iostream>
#include <toy_vector.h>
#include <toy_list.h>
#include <toy_algo.h>

using namespace toy;

void printV(const vector<int>& v) {
    std::cout << "Scan all: ";
    for(size_t i = 0; i < v.size(); i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
}

void printL(const list<int>& l) {
    for(auto iter = l.begin(); iter != l.end(); ++iter) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}

void testVector() {
    std::cout << "*******test vector*******" << std::endl;
    vector<int> v1(6, 5);
    printV(v1);
    vector<int> v2(3, 8);
    printV(v2);
    toy::copy(v2.begin(), v2.end(), v1.begin()+3);
    printV(v1);
    printV(v2);
}

void testList() {
    std::cout << "*******test list*******" << std::endl;
    list<int> l1;
    for(int i = 0; i < 10; i++) {
        l1.push_back(i);
    }
    printL(l1);

    list<int> l2;
    for(int i = 40; i < 45; i++) {
        l2.push_back(i);
    }
    printL(l2);

    toy::copy(l2.begin(), l2.end(), l1.begin());
    printL(l1);
    printL(l2);
}


int main() {
    testVector();
    testList();
}