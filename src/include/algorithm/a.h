/*
 * 算法a：指定时空范围内的车数统计
 */

#include "index/index.h"
#include "algorithm/algorithm.h"
#include "index/rtree/query.h"
namespace algorithm::a{

int
invoke(xCoordInterval3 空时范围)
{
    namespace ir = index::rtree;
    namespace irq = index::rtree::query;

    int count = 0;
    std::vector<route> v;
    ir::rtree rtree; // FIXME
    rtree.query(irq::intersect(空时范围),std::back_inserter(v));

    for(auto route: v){
        auto [beg, end] = 空时范围.get();
        Coord2 coord = route.at<double>(beg.get(2));
        if(inside(coord,空时范围)){
            count++;
        }
    }
    return count;
}

} // end namespace : algorithm > a >