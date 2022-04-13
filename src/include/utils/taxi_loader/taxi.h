#pragma once
namespace taxi {

    typedef int int32_t;
    struct InfoData {
        int32_t id;
        double latitude;
        double longtitude;
        char datetime[32];
    };

    constexpr int len_aInfoData = 8 * 1024 / sizeof(InfoData);

    typedef InfoData* Info;

}
