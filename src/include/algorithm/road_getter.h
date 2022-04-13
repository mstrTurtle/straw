/*
 * 算法a：指定时空范围内的车数统计
 */

#include "index/index.h"
#include "algorithm/algorithm.h"
#include "index/rtree/query.h"
namespace algorithm::road_getter{

std::vector<way>
invoke(xCoordInterval2 空间范围)
{
    namespace ir = index::rtree;
    namespace irq = index::rtree::query;

    std::vector<way> v;
    ir::rtree streetmap; // FIXME
    streetmap.query(irq::intersect(空间范围),std::back_inserter(v));

    return v;
}

} // end namespace : algorithm > a >