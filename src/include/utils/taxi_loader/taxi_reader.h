#pragma once
#include <chrono>
#include <string>

#include <cassert>

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <chrono>
#include <functional>
#include <string>
#include <thread>

// 先检查一下类型，免得出什么毛病（比如在POSIX上面跑，跑不了的）
typedef int64_t Time64;
static_assert(std::is_same_v<time_t, Time64>);

// 别给它加成员函数，它只是个裸的struct。
 #pragma pack(push,1)
 struct Record {
    int32_t            status{0}; // valid, end
    int32_t            id{0};
    Time64             time{0};        // 标准的C库时间
    double_t   lat{0}; // 注意编译器的浮点模型，可能会出问题
    double_t   lng{0};
 };
 #pragma pack(pop)

namespace fs = std::filesystem;

// entry被隐式cast成path了。
// 这个symlink_status，是正常举动版的status。因为这东西不会像status一样，把symlink直接转换到真的文件上。
// 套娃，实现科里化。
// 写成这样（柯里化）有什么用？或许有助于他们捕获闭包？？
// 也许还真有用呢！留着就这样吧！
// fstreamFeeder :: Directory -> streamHandler -> recordHandler -> void
auto fstreamFeeder(std::string dir) {
    return [=](std::function<Record(std::ifstream&)> recordReader) {
        return [=](std::function<void(Record)> recordHandler) -> void {
            fs::path p(dir);
            for (auto const& entry : fs::directory_iterator(p)) {
                auto what = static_cast<std::filesystem::path>(entry).generic_string();
                std::ifstream ifs(what, std::ios::in);
                while (!ifs.eof()) {
                    recordHandler(recordReader(ifs));
                }
            };
        };
    };
}

auto fstreamFeederMultiThread(std::string dir) {
    return [=](std::function<Record(std::ifstream&)> recordReader) {
        return [=](std::function<void(Record)> recordHandler) -> void {
            fs::path p(dir);
            std::vector<std::thread> threads;
            auto f = [&](std::filesystem::directory_entry entry) {
                auto what = static_cast<std::filesystem::path>(entry).generic_string();
                std::ifstream ifs(what, std::ios::in);
                while (!ifs.eof()) {
                    recordHandler(recordReader(ifs));
                }
            };
            int count = 0;

            for (auto const& entry : fs::directory_iterator(p)) {
                ++count;
                if (count != 100) {
                    threads.emplace_back(f, entry);
                }
                else {
                    for (auto& thread : threads) {
                        thread.join();
                    }
                    threads.clear();
                    count = 0;
                }
            }
            for (auto& thread : threads) {
                thread.join();
            }
            threads.clear();
            count = 0;
        };
    };
}