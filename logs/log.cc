#include "util.hpp"
#include <iostream>
#include <string>

void test_util() {
    std::cout << log::util::Date::now() << std::endl;
    string pathname = "./abc/bcd/a.txt";
    log::util::File::createDirectory(pathname);
}

int main() {
    test_util();
    return 0;
}