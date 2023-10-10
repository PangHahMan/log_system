/* 实用工具类的实现
• 获取系统时间
• 判断⽂件是否存在
• 获取⽂件的所在⽬录路径
• 创建⽬录 
*/
#pragma once
#include <ctime>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
using std::string;
namespace log {
    namespace util {
        class Date {
        public:
            //获取系统时间
            static size_t now() {
                //time_t time(time_t *ptr);time_t *ptr是一个指向time_t类型的指针，用于存储返回值。如果ptr为nullptr，则函数直接返回当前时间
                return (size_t) time(nullptr);
            }
        };

        class File {
        public:
            static bool exists(const string &pathname) {
                //判断⽂件是否存在
                //stat结构体中存储了文件的各种信息
                struct stat st;
                //stat函数，用于获取文件或文件夹的状态信息
                if (stat(pathname.c_str(), &st) < 0) {
                    return false;
                }

                return true;
                //access是Linux的系统调用，而stat方式windows平台也支持头文件include<sys/stat.h>
                // return (access(pathname.c_str(), F_OK) == 0);
            }

            static string path(const string &pathname) {
                //获取⽂件的所在⽬录路径
                size_t pos = pathname.find_last_of("/\\");//  \\为转义字符,Linux路径分隔符/ 而Windows为"\"
                if (pos == string::npos) {
                    return ".";//.表示当前路径
                }

                //起始位置(即位置 0)提取到位置 pos + 1 的子字符串。这样就可以得到文件所在的目录路径。
                return pathname.substr(0, pos + 1); 
            }

            static void createDirectory(const string &pathname) {
                //创建⽬录
                // ./abc/bcd/a.txt
                size_t pos = 0, idx = 0;
                while (idx < pathname.size()) {
                    pos = pathname.find_first_of("/\\", idx);//从idx开始查找
                    if (pos == string::npos) {
                        //没找到"/",说明是当前目录，可以直接创建目录,权限设置为0777
                        mkdir(pathname.c_str(), 0777);
                        break;
                    }
                    //找到了"/或者\"，判断该目录是否存在
                    string parent_dir = pathname.substr(0, pos + 1);
                    //目录存在就找下一个
                    if (exists(parent_dir) == true) {
                        idx = pos + 1;//如果目录已经存在，那么就更新idx 找下一个"/"
                        continue;
                    }
                    //不存在则创建目录
                    mkdir(parent_dir.c_str(), 0777);
                    idx = pos + 1;
                }
            }
        };
    }// namespace util
}// namespace log