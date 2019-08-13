#include <iostream>
#include <vector>
#include <string>

using namespace std;

void printV(const vector<int>& v) {
    cout << "Scan all: ";
    for(size_t i = 0; i < v.size(); i++) {
        cout << v[i] << " ";
    }
    cout << endl;
}

int main(int argc, char const *argv[])
{
    string s("hello");
    cout << s << endl;

    s[2] = char();
    cout << s.c_str() << endl;

    return 0;
}

