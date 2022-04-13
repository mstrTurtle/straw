#include "index/array/array.hpp"
#include <iostream>
using Inner = int;
using MyArray = array<int>;
using RecordHeader = MyArray::record_header;
using fh = MyArray::file_header;
using rh = MyArray::record_header;


int main(){

    std::cout
            << sizeof(fh) << ','
            << sizeof(rh) << '\n';

    std::cout
            << offsetof(fh,total_cnt) << ','
            << offsetof(fh,inner_sz) << '\n';

    std::cout
            << offsetof(rh,inner) << '\n';

    MyArray::create("test.bin");

    MyArray obj("test.bin");
    obj.append(4);
    obj.append(8);
    obj.append(12);
    Inner inner;

    inner = obj.visit_inner(0);
    std::cout << inner << "\n";

    inner = obj.visit_inner(1);
    std::cout << inner << "\n";

    inner = obj.visit_inner(2);
    std::cout << inner << "\n";
}