#include <iostream>
#include <string>

int main() {
    std::string utf8String = u8"Hello, 世界"; // UTF-8 encoded string
    const char* cStr = utf8String.c_str();

    std::cout << cStr << std::endl; // Outputs: Hello, 世界
    return 0;
}
