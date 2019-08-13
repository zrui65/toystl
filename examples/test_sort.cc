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
    vector<int> v1;
    // for(int i = 200; i > 0; --i) {
    //     v1.push_back(rand());
    // }
    for(int i = 10; i > 0; --i) {
        v1.push_back(rand()/1000000);
    }
    printV(v1);
    toy::partial_sort(v1.begin(), v1.begin() + 5, v1.end());
    printV(v1);
    toy::sort(v1.begin(), v1.end());
    printV(v1);
}

void testList() {
    std::cout << "*******test list*******" << std::endl;
    list<int> l1;
    for(int i = 10; i > 0; --i) {
        l1.push_back(rand()/1000000);
    }
    // toy::sort(l1.begin(), l1.end());

}


int main() {
    testVector();
    testList();
}