/*
 * 算法c：两矩形之间的往来车流量变化
 */

#include "index/index.h"
#include "algorithm/algorithm.h"
#include "index/rtree/query.h"
namespace algorithm::c{

std::vector<int>
invoke(xCoordInterval3 矩形A,
    xCoordInterval3 矩形B,
    double 时间增量)
{
    namespace ir = index::rtree;
    namespace irq = index::rtree::query;

    int cntA{0}, cntB{0}, cntC{0}, cntD{0};
    std::vector<route> v;
    ir::rtree rtree; // FIXME
    rtree.query(irq::intersect(矩形A) && irq::intersect(矩形B),std::back_inserter(v));
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
        for(double 当前时间 = 起始时间; 当前时间 < 终止时间; 当前时间 += 时间增量){
            Coord2 coord = route.at<double>(当前时间);
            if (inside(coord,矩形A)) {
                if(状态 == 已经过B){
                    cntB++;
                }else if(状态 == 初态){
                    起始状态 = 已经过A;
                }
                状态 = 已经过A;
            }
            else if (inside(coord,矩形B)) {
                if(状态 == 已经过A){
                    cntA++;
                }
                else if(状态 == 初态){
                    起始状态 = 已经过B;
                }
                状态 = 已经过B;
            }
        }
        if(起始状态 != 初态){
            xCoordInterval3& 目标区间 = (起始状态 == 已经过A)?(矩形B):(矩形A);
            int& cntTarget = (起始状态 == 已经过A)?(cntC):(cntD);
            for(double 当前时间 = 起始时间; 当前时间 > route.time_beg; 当前时间 -= 时间增量){
                Coord2 coord = route.at<double>(当前时间);
                if(inside(coord,目标区间)){
                    cntTarget++;
                }
            }
        }
        if(终止状态 != 初态){
            xCoordInterval3& 目标区间 = (终止状态 == 已经过A)?(矩形B):(矩形A);
            int& cntTarget = (终止状态 == 已经过A)?(cntC):(cntD);
            for(double 当前时间 = 终止时间; 当前时间 < route.time_end; 当前时间 += 时间增量){
                Coord2 coord = route.at<double>(当前时间);
                if(inside(coord,目标区间)){
                    cntTarget++;
                }
            }
        }
    }
    return {cntA,cntB,cntC,cntD};
}

} // end namespace : algorithm > c >