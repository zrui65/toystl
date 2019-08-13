#include <iostream>

#include <toy_hashtable.h>

using namespace toy;

#define LOG std::cout
#define END std::endl

int main() {
    typedef hashtable<int, int, std::hash<int>, std::_Identity<int>, 
              std::equal_to<int>> hashtable_type;
    
    hashtable_type ht1(3, std::hash<int>(), std::equal_to<int>());

    LOG << "size = " << ht1.size() << END;
    LOG << "bucket_num = " << ht1.bucket_num() << END;

    ht1.insert_unique(23);
    ht1.insert_unique(54);
    ht1.insert_unique(2);
    ht1.insert_unique(78);
    ht1.insert_unique(56);
    ht1.insert_unique(87);
    ht1.insert_unique(65);
    ht1.insert_unique(14);
    ht1.insert_unique(38);
    ht1.insert_unique(62);
    ht1.insert_multi(2);
    ht1.insert_unique(2);

    LOG << "size = " << ht1.size() << END;
    LOG << "bucket_num = " << ht1.bucket_num() << END;

    for(auto iter = ht1.begin(); iter != ht1.end(); ++iter) {
        LOG << *iter << " ";
    }
    LOG << END;

    auto iter1 = ht1.find(78);
    LOG << *iter1 << END;
}


