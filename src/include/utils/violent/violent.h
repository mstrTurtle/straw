#pragma once
#pragma region decla

#include <string>
#include <map>
#include <functional>

namespace parser {


    enum class type {
        left = 0,
        right = 1,
        closed = 2,
        content = 3,
        unkown = 4,
        header = 5,
    };

    inline const char* type_what[] = { "left","right","closed","content","unkown","header" };

    /*****************************************************************
    *
    * 单例模式的InfoSaver，作为全局变量。故细节可见。
    *
    * 虽然这个结构相当不优雅，但却是实用之选。
    *
    * 毕竟这只是顺序的，又没有并行。传参也没用。
    *
    * 搞个临时结构反而效率高。
    *
    * 注意：
    *
    * 这种写法不是线程安全的，因为静态的局部变量是在调用的时候分配到静态存储区，
    * 所以在编译的时候没有分配。
    *
    * 静态局部对象：
    *
    * 在程序执行到该对象的定义处时，创建对象并调用相应的构造函数！
    *
    * 如果在定义对象时没有提供初始指，则会暗中调用默认构造函数，如果没有默认
    * 构造函数，则自动初始化为0。
    *
    * 如果在定义对象时提供了初始值，则会暗中调用类型匹配的带参的构造函数
    * （包括拷贝构造函数），如果没有定义这样的构造函数，编译器可能报错！
    *
    * 直到main（）结束后才会调用析构函数！
    *
    * https://blog.csdn.net/zhanghuaichao/article/details/79459130
    *
    ****************************************************************/
    struct InfoSaver {
        type            type = type::unkown;
        size_t          lineno = 0;
        size_t          offset = 0;
        int             level = 0; // 英明的做法
        std::string     tag = "";
        std::map<std::string, std::string> attrs;

        static InfoSaver& getInstance() {
            static InfoSaver instance;
            return instance;
        }
    private:
        InfoSaver() = default; // 必须放在private。
        InfoSaver(const InfoSaver&) = delete;
        InfoSaver(InfoSaver&) = delete;
        InfoSaver(InfoSaver&&) = delete;
    };

    using callback_t = std::function<void(InfoSaver&)>;

    void init(const char* path, callback_t callback);

    void execute();

} /* namespace parser */

#pragma endregion decla