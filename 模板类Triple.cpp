#include <iostream>
#include <string>

// 模板类 Triple
template <typename T>
class Triple {
private:
    T first;
    T second;
    T third;

public:
    // 构造函数
    Triple(T a, T b, T c) : first(a), second(b), third(c) {}

    // 访问成员的函数
    T getFirst() const {
        return first;
    }

    T getSecond() const {
        return second;
    }

    T getThird() const {
        return third;
    }
};

int main() {
    // 测试 int 类型
    Triple<int> t1(10, 20, 30);
    std::cout << "Triple<int>: "
              << t1.getFirst() << ", "
              << t1.getSecond() << ", "
              << t1.getThird() << std::endl;

    // 测试 string 类型
    Triple<std::string> t2("hello", "world", "c++");
    std::cout << "Triple<string>: "
              << t2.getFirst() << ", "
              << t2.getSecond() << ", "
              << t2.getThird() << std::endl;

    return 0;
}
