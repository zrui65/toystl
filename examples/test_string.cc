#include <iostream>

#include "toy_string.h"

using namespace toy;

void printS(const string& s) {
    for(size_t i = 0; i < s.size(); i++) {
        std::cout << s[i];
    }
    std::cout << std::endl;
}

int main() {
    string s1;
    string s2("hello world!");
    string s3(26, 'c');
    string s4(s2);
    s1 = s3;
    s1[20] = 'M';

    std::cout << "string capacity:" << std::endl;
    std::cout << s1.capacity() << std::endl;
    std::cout << s2.capacity() << std::endl;
    std::cout << s3.capacity() << std::endl;
    std::cout << s4.capacity() << std::endl;
    
    printS(s1);
    printS(s2);
    printS(s3);
    printS(s4);

    s2.push_back('K');
    s4.pop_back();
    s1 = s4;

    std::cout << std::endl;
    std::cout << "string capacity:" << std::endl;
    std::cout << s1.capacity() << std::endl;
    std::cout << s2.capacity() << std::endl;
    std::cout << s3.capacity() << std::endl;
    std::cout << s4.capacity() << std::endl;

    std::cout << std::endl;
    std::cout << "C style string:" << std::endl;
    std::cout << s1.c_str() << std::endl;
    std::cout << s2.c_str() << std::endl;
    std::cout << s3.c_str() << std::endl;
    std::cout << s4.c_str() << std::endl;

    s1.erase(s1.begin() + 1, s1.end() - 1);
    std::cout << s1.c_str() << std::endl;
    s1.clear();
    std::cout << s1.c_str() << std::endl;
    s1.append(s2);
    std::cout << s1.c_str() << std::endl;
    s1.append(s3);
    std::cout << s1.c_str() << std::endl;
    s1.erase(s1.begin()+3, s1.end()-3);
    std::cout << s1.c_str() << std::endl;
    s1.append("hahahaha");
    std::cout << s1.c_str() << std::endl;
    std::cout << s1 << std::endl;

    string s5 = s1 + s3;
    std::cout << s5 << std::endl;
    std::cout << s1 + "asdfkjas" << std::endl;
    std::cout << "asdfkjas" + s1 << std::endl;
    
}


