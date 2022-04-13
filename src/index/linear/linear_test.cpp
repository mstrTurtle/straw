#include "index/linear/linear.hpp"
#include <iostream>
using MyLinear = linear<int>;
using RecordHeader = MyLinear::record_header;
using fh = MyLinear::file_header;
using rh = MyLinear::record_header;


int main(){

    std::cout << sizeof(fh) << ',' << sizeof(rh) << '\n';

    std::cout << offsetof(fh,first_pos) << ','
            << offsetof(fh,last_pos) << ','
            << offsetof(fh,total_cnt) << ','
            << offsetof(fh,inner_sz) << '\n';

    std::cout << offsetof(rh,prev_pos) << ','
            << offsetof(rh,next_pos) << ','
            << offsetof(rh,data_len) << ','
            << offsetof(rh,inner) << '\n';

    MyLinear::create("test.bin");

    MyLinear obj("test.bin");
    obj.append(4);
    obj.append(8);
    RecordHeader rh;
    obj.begin();
    rh = obj.get_record_header();
    std::cout << rh.inner << "\n";
    obj.next();
    rh = obj.get_record_header();
    std::cout << rh.inner << "\n";
    obj.next();
    rh = obj.get_record_header();
    std::cout << rh.inner << "\n";
}