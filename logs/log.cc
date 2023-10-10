#include "format.hpp"
#include "level.hpp"
#include "message.hpp"
#include "util.hpp"
#include <iostream>
#include <string>

void test_util() {
    std::cout << log::util::Date::now() << std::endl;
    string pathname = "./abc/bcd/a.txt";
    log::util::File::createDirectory(pathname);
}

void test_level() {
    std::cout << log::LogLevel::toString(log::LogLevel::value::DEBUG) << std::endl;
    std::cout << log::LogLevel::toString(log::LogLevel::value::INFO) << std::endl;
    std::cout << log::LogLevel::toString(log::LogLevel::value::WARN) << std::endl;
    std::cout << log::LogLevel::toString(log::LogLevel::value::ERROR) << std::endl;
    std::cout << log::LogLevel::toString(log::LogLevel::value::FATAL) << std::endl;
    std::cout << log::LogLevel::toString(log::LogLevel::value::OFF) << std::endl;
}

void test_message() {
    log::LogMessage msg(log::LogLevel::value::INFO, 53, "main.c", "root", "格式化功能测试...");
}

void test_format() {
    log::LogMessage msg(log::LogLevel::value::INFO, 53, "main.c", "root", "格式化功能测试...");
    //使用默认打印[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n
    log::Formatter fmt("[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n");
    std::string str = fmt.format(msg);
    std::cout << str << std::endl;

    log::Formatter fmt1("abc%%avc[%d{%H:%M:%S}] %m%n");
    str = fmt1.format(msg);
    std::cout << str << std::endl;

    //使用错误格式%g打印
    log::Formatter fmt2("abc%%g%gavc[%d{%H:%M:%S}] %m%n");
    str = fmt2.format(msg);
    std::cout << str << std::endl;
}

int main() {
    test_format();
    return 0;
}