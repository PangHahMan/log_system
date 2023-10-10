#pragma once
#include "level.hpp"
#include "message.hpp"
#include "util.hpp"
#include <cassert>
#include <ctime>
#include <memory>
#include <sstream>
#include <vector>
namespace log {
    // 抽象格式化子项子类
    class FormatItem {
    public:
        virtual ~FormatItem() {}
        using ptr = std::shared_ptr<FormatItem>;
        virtual void format(std::ostream &out, const LogMessage &message) = 0;
    };
    // 派生格式化子项子类--消息，等级，时间，文件名，行号，线程ID，日志器名，制表符，换行，其他
    // 消息%m
    class MsgFormatItem : public FormatItem {
    public:
        //MsgFormatItem(const std::string &str = "") {}
        virtual void format(std::ostream &out, const LogMessage &message) override {
            out << message._payload;//_payload有效消息数据
        }
    };
    // 等级%p
    class LevelFormatItem : public FormatItem {
    public:
        //LevelFormatItem(const std::string &str = "") {}
        virtual void format(std::ostream &out, const LogMessage &message) override {
            out << LogLevel::toString(message._level);//_level转换成字符串
        }
    };
    //时间%d
    class TimeFormatItem : public FormatItem {
    public:
        TimeFormatItem(const std::string &fmt = "%H:%M:%S")
            : _time_fmt(fmt) {
        }

        virtual void format(std::ostream &out, const LogMessage &message) override {
            struct tm t;
            localtime_r(&message._ctime, &t);
            char tmp[32] = {0};
            strftime(tmp, 31, _time_fmt.c_str(), &t);
            out << tmp;
        }

    private:
        std::string _time_fmt;
    };
    //文件%f
    class FileFormatItem : public FormatItem {
    public:
        //FileFormatItem(const std::string &str = "") {}
        virtual void format(std::ostream &out, const LogMessage &message) override {
            out << message._file;
        }
    };
    //行号%l
    class LineFormatItem : public FormatItem {
    public:
        //LineFormatItem(const std::string &str = "") {}
        virtual void format(std::ostream &out, const LogMessage &message) override {
            out << message._line;
        }
    };
    //线程ID %t
    class ThreadFormatItem : public FormatItem {
    public:
        //ThreadFormatItem(const std::string &str = "") {}
        virtual void format(std::ostream &out, const LogMessage &message) override {
            out << message._tid;
        }
    };
    //日志器名%c
    class LoggerFormatItem : public FormatItem {
    public:
        //LoggerFormatItem(const std::string &str = "") {}
        virtual void format(std::ostream &out, const LogMessage &message) override {
            out << message._logger;
        }
    };
    //\t
    class TabFormatItem : public FormatItem {
    public:
        //TabFormatItem(const std::string &str = "") {}
        virtual void format(std::ostream &out, const LogMessage &message) override {
            out << "\t";
        }
    };
    //\n
    class NLineFormatItem : public FormatItem {
    public:
        //NLineFormatItem(const std::string &str = "") {}
        virtual void format(std::ostream &out, const LogMessage &message) override {
            out << "\n";
        }
    };
    //其他字符
    class OtherFormatItem : public FormatItem {
    public:
        OtherFormatItem(const std::string &str) : _str(str) {}
        virtual void format(std::ostream &out, const LogMessage &message) override {
            out << _str;
        }

    private:
        std::string _str;
    };

    /* 
    %d 表示日期，包含子格式{%H:%M:%S}
    %t 表示线程ID
    %c 表示日志器名称
    %f 表示源码文件名
    %l 表示源码行号 
    %p 表示日志级别
    %T 表示制表符缩进
    %m 表示主体消息
    %n 表示换行
     */
    class Formatter {
    public:
        using ptr = std::shared_ptr<Formatter>;
        Formatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n")
            : _pattern(pattern) {
            assert(parsePattern());//字符串解析必须为真
        }

        //对msg进行格式化，分别用于将LogMessage对象进行格式化并输出到不同的目标
        void format(std::ostream &out, const LogMessage &msg) {
            //将格式化后的消息输出到给定的std::ostream流（输出流中
            for (auto &items: _items) {
                items->format(out, msg);//根据items的类型，形成多态,进行输出
            }
        }

        const std::string format(const LogMessage &msg) {
            std::stringstream ss;
            for (auto &it: _items) {
                it->format(ss, msg);//根据items的类型，形成多态,进行输出
            }
            return ss.str();
            //返回字符串
        }

    private:
        //根据不同的格式化字符创建不同的格式化子项对象
        FormatItem::ptr createItem(const std::string &key, const std::string &value) {
            if (key == "d") {
                return std::make_shared<TimeFormatItem>(value);
            } else if (key == "t") {
                return std::make_shared<ThreadFormatItem>();
            } else if (key == "c") {
                return std::make_shared<LoggerFormatItem>();
            } else if (key == "f") {
                return std::make_shared<FileFormatItem>();
            } else if (key == "l") {
                return std::make_shared<LineFormatItem>();
            } else if (key == "p") {
                return std::make_shared<LevelFormatItem>();
            } else if (key == "T") {
                return std::make_shared<TabFormatItem>();
            } else if (key == "m") {
                return std::make_shared<MsgFormatItem>();
            } else if (key == "n") {
                return std::make_shared<NLineFormatItem>();
            }
            if (key.empty()) {
                return std::make_shared<OtherFormatItem>(value);
            }
            std::cout << "没有对应的格式化字符:" << key << std::endl;
            abort();
            return FormatItem::ptr();
        }

        //对格式化规则字符串进行解析
        bool parsePattern() {
            //1.对格式化规则字符串进行解析
            // %cab%%cde[%d{%H:%M:%S}][%p]%T%m%
            std::vector<std::pair<std::string, std::string>> fmt_order;//用来保存解析完的格式化字符串的结果。
            size_t pos = 0;
            //key:存储格式化字符,遇到 % 字符，其后面的字符（只有一个）会被视为格式化字符
            //val的两个作用：
            /* 一是用于存储原始字符，也就是不被格式化的字符。如果在遍历格式化字符串时，遇到的字符不是 %，那么这个字符就被视为原始字符，直接添加到 val 中。
            二是用于存储子规则。如果格式化字符后面跟随着 {}，那么 {} 里面的内容会被视为子规则，被存储在 val 中。这个子规则通常用于更细粒度的格式控制，例如小时、分钟、秒等。 */
            std::string key, val;
            while (pos < _pattern.size()) {
                //1.处理原始字符串 -- 判断是否是%d,不是就是原始字符
                if (_pattern[pos] != '%') {
                    val.push_back(_pattern[pos++]);//普通的字符，直接append到val字符串中
                    continue;
                }
                //能走下来就代表pos位置是%字符,需要判断是否是%%字符
                if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%') {
                    //表示当前是一个%%字符
                    val.push_back('%');
                    pos += 2;//两个%需要走两步
                    continue;
                }
                //这时候代表原始字符串处理完毕
                if (val.empty() == false) {
                    fmt_order.push_back(std::make_pair("", val));
                    val.clear();
                }

                //这时候格式化字符的处理
                //能走下来，代表%后边是个格式化字符,或者%是最后的字符，后面是字符串结束
                if (pos + 1 == _pattern.size()) {
                    std::cout << "%之后,没有对应的格式化字符!" << std::endl;
                    return false;
                }
                //取pos后面的格式化字符
                key = _pattern[++pos];
                //接下来还要判断格式化后有没有{}
                pos += 1;

                if (pos < _pattern.size() && _pattern[pos] == '{') {
                    pos += 1;//这时候pos指向{之后子规则的起始位置
                    while (pos < _pattern.size() && _pattern[pos] != '}') {
                        val.push_back(_pattern[pos++]);
                    }

                    //走到了末尾跳出了循环，也没用遇到},则表示格式有问题，直接返回false
                    if (pos == _pattern.size()) {
                        std::cout << "子规则{}匹配出错" << std::endl;
                        return false;
                    }
                    pos += 1;//因为这时候pos指向的是}位置,向后走一步，走到了下次处理的新位置
                }

                fmt_order.push_back(std::make_pair(key, val));
                key.clear();
                val.clear();
            }
            //2.根据解析得到的数据初始化格式化子项数组成员
            for (auto &it: fmt_order) {
                _items.push_back(createItem(it.first, it.second));
            }

            return true;
        }

    private:
        std::string _pattern;               //格式化规则字符串
        std::vector<FormatItem::ptr> _items;//FormatItem::ptr是一个智能指针
    };
}// namespace log