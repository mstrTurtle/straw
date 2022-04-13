#include "utils/taxi_loader/taxi_reader.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <chrono>
#include <functional>
#include <format>
#include <string>
#include <thread>

auto taxiLineParser =
    [](std::ifstream& s) -> Record { // 将流中的内容塞进一个Record结构体并返回
        Record record;
        using namespace std;
        using namespace std::chrono;
        /* 匹配，并处理错误 */
        auto check = [&](char c) {
            char t = s.get();
            if (c != t) {
                std::cout << "\nwrong: character '" << t << "'" << endl;
                char rem[100] = "";
                s.getline(rem,100);
                std::cout << endl << "Error-remaining: " << rem << endl;
            }
        };
        /* 逐项匹配 */
        s >> record.id;
        system_clock::time_point tp{};
        check(',');
        s >> parse("%F %T"s, tp);
        record.time = std::chrono::system_clock::to_time_t(tp);
        check(',');
        s >> record.lat;
        check(',');
        s >> record.lng;
        s.get();
        /* 检查换行与双换行 */
        //check('\n');
        char cur = s.get();
        if(cur == '\n'){
            check('\n');
            check('\0');
        }
        else
            s.unget();
        return record;
    };

int main() {
    auto taxiRecordHandler = [](Record record) -> void {
        using namespace std;
        cout << std::format("\nid = {}, time = {}, lat = {}, lng = {}", record.id, record.time, record.lat, record.lng);
        return;
    };
    fstreamFeeder(R"(D:\taxi_proj\dataset)")(taxiLineParser)(taxiRecordHandler);

    return 0;
}