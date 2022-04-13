/*
 * 算法g：两矩形之间最快路径
 */

#include <map>
#include <queue>
#include "index/index.h"
#include "algorithm/algorithm.h"
#include "index/rtree/query.h"
namespace algorithm::g{

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

    /* 对应时间间隔，route id，对应方向，对应开始时钟，对应停止时钟 */
    using Record = std::tuple<double,int,int,double,double>;

    std::vector<Record> records;

    auto [beg, end] = 矩形A.get();
    double 起始时间 = beg.get(2);
    double 终止时间 = end.get(2);
    for(auto route: v){
        /*
         * 对应的是：
         * t1 - t2 时段
         * A. 从A到B
         * B. 从B到A
         * B. 已通过矩形A但却尚未通过矩形B（即车子在截止时，即将通过矩形B）
         * C. 已通过矩形B但却尚未通过矩形A（即车子在截止时，即将通过矩形A）
         */
        const int 初态 = 0, 已经过A = 1, 已经过B = 2;
        int 状态 = 初态;
        int 起始状态 = 0, 终止状态 = 0;
        double 开始时钟{0}, 停止时钟{0};
        double 最小间隔 = 1e308; // 即最短通行时间。先置成正无穷，后面会迭代地减小
        double 对应开始时钟{0}, 对应停止时钟{0};
        const int 从A到B = 0, 从B到A = 1;
        int 对应方向 = 0;
        /* 选取该route对应的最快路径，保存在对应方向，对应开始时钟，对应停止时钟中。 */
        for(double 当前时间 = 起始时间; 当前时间 < 终止时间; 当前时间 += 时间增量){
            Coord2 coord = route.at<double>(当前时间);
            if (!inside(coord,矩形A) && !inside(coord,矩形B)) {
                if(状态 == 已经过A || 状态 == 已经过B)
                    开始时钟 = 当前时间;
            }
            else if(inside(coord,矩形A)){
                if(状态 == 已经过B){
                    停止时钟 = 当前时间;
                    int 间隔 = 停止时钟 - 开始时钟;
                    if(间隔 < 最小间隔){
                        最小间隔 = 间隔;
                        对应方向 = 从B到A;
                        对应开始时钟 = 开始时钟;
                        对应停止时钟 = 停止时钟;
                    }
                }

            }else if(inside(coord,矩形B)){
                if(状态 == 已经过A){
                    停止时钟 = 当前时间;
                    int 间隔 = 停止时钟 - 开始时钟;
                    if(间隔 < 最小间隔){
                        最小间隔 = 间隔;
                        对应方向 = 从A到B;
                        对应开始时钟 = 开始时钟;
                        对应停止时钟 = 停止时钟;
                    }
                }
            }
        }
        records.emplace_back(Record{最小间隔,route.get_id(),对应方向,对应开始时钟,对应停止时钟});
    }

    using HeapElement = Record;
    // 构造一个小顶堆
    auto less = [](HeapElement e1, HeapElement e2) -> bool {return std::get<0>(e1) < std::get<0>(e2);};
    std::priority_queue<HeapElement,std::vector<HeapElement>,decltype(less)> heap;
    for(int i = 0; i<k; i++)
        heap.push(HeapElement{}); // 弄几个空的进去
    for(auto record : records){
        if(!less(record,heap.top())){
            heap.pop();
            heap.push(record);
        }
    }
    return heap;
}

} // end namespace : algorithm > b >