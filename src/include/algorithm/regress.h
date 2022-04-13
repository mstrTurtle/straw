/*
 * 回归算法：把GPS点集回归成路线
 */

#include "index/index.h"
#include "algorithm/algorithm.h"
#include "index/rtree/query.h"
#include <numbers>
#include <cmath>
#include <queue>
#include <map>
#include <numeric>
namespace algorithm::regress{

double 求测量概率(double dist) {
    using namespace std::numbers;
    double σ = 4.07;
    return inv_sqrtpi * (1 / (sqrt2 * σ)) * exp(-0.5 * pow(dist / σ, 2));
}

double 求转移概率(double dt) {
    using namespace std::numbers;
    double median_delta = 20;
    double β = 1 / ln2 * median_delta;
    return (1/β) * exp(-dt/β);
}

struct PointedWay {
    Coord2 coord;
    int way_id;
};

std::pair<double,PointedWay>
proj(GpsPoint point,
    way way)
{

}

using Coord2Tuple = GetCoordTupleType<Coord2>::type;

double 求大地线距离(PointedWay in1,PointedWay in2){
    auto [lat1,lng1] = (Coord2Tuple)in1.coord;
    auto [lat2,lng2] = (Coord2Tuple)in2.coord;
    return calc::GetDistance(lat1,lng1,lat2,lng2);
}

struct ShortestPathInfo{};

int
heuristic(PointedWay beg,
    PointedWay end,
    PointedWay mid)
{
    const double scale = 1.0;
    auto [lat1,lng1] = (Coord2Tuple)beg.coord;
    auto [lat2,lng2] = (Coord2Tuple)end.coord;
    auto [lat3,lng3] = (Coord2Tuple)mid.coord;
    return scale * (lat1*lat2 + lng1*lng2) / (std::hypot(lat1,lng1) * std::hypot(lat2,lng2));
}

std::pair<double,ShortestPathInfo> // FIXME
求最短路(PointedWay in1,PointedWay in2){
    // FIXME
    using WayId = int;
    using PrioType = std::pair<WayId,int>;
    auto less = [](PrioType e1, PrioType e2) -> bool {return std::get<1>(e1) < std::get<1>(e2);};

    std::priority_queue<PrioType,
        std::vector<PrioType>,decltype(less)> frontier;

    auto start = PrioType{in1.way_id, 0};
    frontier.push({in1.way_id, 0});
    auto came_from = std::map<PrioType,WayId>();
    auto cost_so_far = std::map<PrioType,int>();
    came_from[start] = 0; // he's a real no_where man
    cost_so_far[start] = 0;

    PrioType current;

    auto neighbors
        = [&](PrioType in) -> std::vector<PrioType> {
            // FIXME
        };

    auto cost
        = [](PrioType in1, PrioType in2) -> double {
            // FIXME
        };

    while (! frontier.empty()){
        current = frontier.top();
        frontier.pop();

        if(std::get<0>(current) == in2.way_id)
            break;

        for (auto next : neighbors(current)){
            auto new_cost = cost_so_far[current] + cost(current, next);
            if (!(cost_so_far.find(next) == cost_so_far.end()) || new_cost < cost_so_far[next]){
                cost_so_far[next] = new_cost;
                // PointedWay pointed_next = xxxx // FIXME
                auto priority = new_cost + heuristic(in2, pointed_next);
                frontier.push({std::get<0>(next), priority});
                came_from[next] = std::get<1>(current);
            }
        }
    }
}

std::vector<int>
invoke(std::vector<GpsPoint> points
)
{
    namespace ir = index::rtree;
    namespace irq = index::rtree::query;

    int count = 0;
    ir::rtree streetmap; // FIXME


    for(auto point: points){
        // double delta = 0.00005;
        // Coord2 min_cornor{point.lat - delta,point.lng - delta};
        // Coord2 max_cornor{point.lat + delta,point.lng + delta};
        // xCoordInterval2 scope{min_cornor,max_cornor};

        std::vector<way> ways;
        streetmap.query(irq::nearest(Coord2{point.lat,point.lng},5),std::back_inserter(ways));

        PointedWay former_pointedway;

        for(auto way:ways){
            auto [dist, pointedway] = proj(point,way);
            auto 视在概率 = 求测量概率(dist);
            auto [最短路距离,详情] = 求最短路(former_pointedway,pointedway);
            auto 大地线距离 = 求大地线距离(former_pointedway,pointedway);
            auto 距离差值 = abs(最短路距离 - 大地线距离);
            auto 转移概率 = 求转移概率(距离差值);
        }
    }
    return {};
}

} // end namespace : algorithm > b >