#include <tuple>
#include <map>
#include <iostream>
#include <format>

#include "utils/violent/violent.h"

int main() {
    using namespace parser;
    using namespace std;

    map<long long, tuple<float, float>> m;

    /* 指定文件路径 */
    const char* path = R"(C:\Users\qq769\Downloads\map.osm)";
    /* 声明Callback */
    auto cb = [&](InfoSaver& Info) -> void {
        cout << '(' << type_what[(int)Info.type] << ')' << Info.tag << endl;
        if (Info.tag == "node") {
            auto id_str = Info.attrs["id"];
            auto lat_str = Info.attrs["lat"];
            auto lon_str = Info.attrs["lon"];
            auto id = atoll(id_str.c_str());
            auto lat = (float)atof(lat_str.c_str());
            auto lon = (float)atof(lon_str.c_str());
            m[id] = std::make_tuple(lat, lon);
        }
    };

    /* 执行Parser */
    init(path, cb);
    execute();

    /* 打印map收集到的东西 */
    for (auto& e : m) {
        auto& [lat, lon] = e.second;
        //cout << e.first << " -> " << "(" << lat << "," << lon << ")" << endl;
        cout << format("{} -> ({:.5f},{:.5f})\n", e.first, lat, lon);
    }

    return 0;
}