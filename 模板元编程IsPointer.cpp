#include <iostream>
#include <string>

// 1. 主模板：默认不是指针
template <typename T>
struct IsPointer {
    static constexpr bool value = false;
};

// 2. 偏特化：专门匹配 T* 指针类型
template <typename T>
struct IsPointer<T*> {
    static constexpr bool value = true;
};

int main() {
    // 编译期断言测试
    static_assert(IsPointer<int*>::value == true, "int* 是指针");
    static_assert(IsPointer<int>::value == false, "int 不是指针");
    
    static_assert(IsPointer<std::string*>::value == true, "string* 是指针");
    static_assert(IsPointer<std::string>::value == false, "string 不是指针");
    
    static_assert(IsPointer<void*>::value == true, "void* 是指针");
    static_assert(IsPointer<double**>::value == true, "二级指针也是指针");

    std::cout << "所有 static_assert 测试通过！" << std::endl;
    return 0;
}
