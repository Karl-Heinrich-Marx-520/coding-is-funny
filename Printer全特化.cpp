#include <iostream>
#include <string>
using namespace std;

// 通用类模板 Printer
template <typename T>
class Printer {
public:
    void print(const T& value) {
        cout << "General Printer: " << value << endl;
    }
};

// 针对 bool 类型的全特化
template <>
class Printer<bool> {
public:
    void print(bool value) {
        cout << "Boolean Printer: " << (value ? "true" : "false") << endl;
    }
};

int main() {
    // 通用版本测试
    Printer<int> p1;
    p1.print(123);

    Printer<string> p2;
    p2.print("hello");

    Printer<double> p3;
    p3.print(3.14);

    // bool 特化版本测试
    Printer<bool> p4;
    p4.print(true);
    p4.print(false);

    return 0;
}
