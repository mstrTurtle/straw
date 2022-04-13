/*
 * 算法b：指定时空范围内的R*R网格车数统计
 */

#include "index/index.h"
#include "algorithm/algorithm.h"
#include "index/rtree/query.h"
namespace algorithm::b{

std::vector<int>
invoke(xCoordInterval3 空时范围,
    int r)
{
    namespace ir = index::rtree;
    namespace irq = index::rtree::query;

    int count = 0;
    std::vector<route> v;
    std::vector<int> matrix(r*r,0);
    ir::rtree rtree; // FIXME
    rtree.query(irq::intersect(空时范围),std::back_inserter(v));

    for(auto route: v){
        auto [beg, end] = 空时范围.get();
        Coord2 coord = route.at<double>(beg.get(2));
        if(inside(coord,空时范围)){
            int row = (int)(coord.get(0) - beg.get(0)) / r;
            int col = (int)(coord.get(1) - beg.get(1)) / r;
            matrix[row * r + col] ++;
        }
    }
    return matrix;
}

} // end namespace : algorithm > b >