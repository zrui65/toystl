#include <iostream>

#include <toy_vector.h>
#include <toy_string.h>

using namespace toy;

void printV(const vector<int>& v) {
    std::cout << "Scan all: ";

    for(size_t i = 0; i < v.size(); i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
}

void printVS(const vector<string>& v) {
    std::cout << "Scan all: ";

    for(size_t i = 0; i < v.size(); i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    vector<int> v1;
    vector<int> v2(5);
    vector<int> v3(3, 7);

    std::cout << "v1.size() = " << v1.size() << std::endl;
    std::cout << "v2.size() = " << v2.size() << std::endl;
    std::cout << "v3.size() = " << v3.size() << std::endl;

    v1.push_back(7);
    v1.push_back(8);
    int b = 9;
    v1.push_back(b);
    printV(v1); // 7 8 9

    v1 = v1;

    std::cout << "index: first = " << v1.front() << std::endl;
    std::cout << "index: last  = " << v1.back() << std::endl;
    std::cout << "index: 1     = " << v1.at(1) << std::endl;
    v1[1] = 4;
    std::cout << "index: 1     = " << v1[1] << std::endl;
    printV(v1); // 7 4 9

    vector<int> v4(v1);
    printV(v4); // 7 4 9

    v4.swap(v3);
    printV(v3); // 7 4 9
    printV(v4); // 7 7 7

    v2 = v3;
    printV(v2); // 7 4 9
    v2.pop_back();
    printV(v2); // 7 4
    v2.erase(v2.begin()+1, v2.end());
    printV(v2); // 7

    *(v1.erase(v1.begin())) = 0;

    v1.clear();
    v1.push_back(3);
    printV(v1);
    
    vector<string> vs1(5, "abcde");
    printVS(vs1);
}


