/*
 * 算法f：两矩形之间前k个最频繁路径
 */

#include <map>
#include <queue>
#include "index/index.h"
#include "algorithm/algorithm.h"
#include "index/rtree/query.h"
namespace algorithm::f{

auto
invoke(xCoordInterval3 矩形A,
    xCoordInterval3 矩形B,
    double 时间增量,
    int k)
{
    namespace ir = index::rtree;
    namespace irq = index::rtree::query;

    std::map<int,int> m;
    int count = 0;
    std::vector<route> v;
    ir::rtree rtree; // FIXME
    rtree.query(irq::intersect(矩形A) && irq::intersect(矩形B),std::back_inserter(v));


    for(auto route: v){
        auto [beg, end] = 矩形A.get();
        double 起始时间 = beg.get(2);
        double 终止时间 = end.get(2);

        std::vector<int> ways;
        ways = route.get_ways();
        for(auto way:ways){
            bool exist = (m.find(way) != m.end());
            if(!exist){
                m[way] = 0;
            }else{
                m[way]++;
            }
        }
    }
    using HeapElement = std::pair<const int,int>;
    // 构造一个小顶堆
    auto less = [](HeapElement e1, HeapElement e2) -> bool {return e1.first < e2.first;};
    std::priority_queue<HeapElement,std::vector<HeapElement>,decltype(less)> heap;
    for(int i = 0; i<k; i++)
        heap.push(HeapElement{}); // 弄几个空的进去
    for(auto kv : m){
        if(!less(kv,heap.top())){
            heap.pop();
            heap.push(kv);
        }
    }
    return heap;
}

} // end namespace : algorithm > b >