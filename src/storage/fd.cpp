#include <cstdio>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

struct IndexMetaInfo{
    typedef char IndexNameTag[32];
    typedef char IndexFileName[32];
};

FILE* file = nullptr;

void init_config_file(){
    
}

void load_config_or_create(){
    fs::path bin_path = fs::current_path();
    fs::path listfile_path = bin_path/"lists.bin";
    if(fs::exists(listfile_path)){
        file = fopen(listfile_path.string().c_str(),"b");
    }else{
        file = fopen(listfile_path.string().c_str(),"b");
        init_config_file();
    }
}