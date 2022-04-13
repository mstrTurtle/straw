#include <tuple>
#include <type_traits>
#include <numbers>
#include <vector>
template<typename _Ty, size_t _Dimension>
struct
[[deprecated("这东西不好用，所以弃用了")]]
Interval{
    using ValueType = _Ty;
    using Range = std::pair<_Ty,_Ty>;

    template<typename N>
    Range get(){
        return {m_begins[N],m_ends[N]};
    }

    Range get(size_t n){
        return {m_begins[n],m_ends[n]};
    }

    template<typename N>
    void set(Range const& range){
        auto& [beg, end] = range;
        m_begins[N] = beg;
        m_ends[N] = end;
    }

    void set(size_t n, _Ty const& beg, _Ty const& end){
        m_begins[n] = beg;
        m_ends[n] = end;
    }

private:
    _Ty m_begins[_Dimension];
    _Ty m_ends[_Dimension];
};

using CoordInterval2 = Interval<double, 2>;
using CoordInterval3 = Interval<double, 3>;



template<typename _Ty, size_t _Dimension>
struct Coord{
    using ValueType = _Ty;
    using Dimension = std::integral_constant<size_t,_Dimension>;

    Coord(){}

    Coord(_Ty... args){
        static_assert(sizeof...(args) == _Dimension);
        int i = 0;
        (values[i++] = xs),...;
    }

    template<typename N>
    _Ty get(){
        return values[N];
    }

    _Ty get(size_t n){
        return values[n];
    }

    template<typename N>
    void set(_Ty const& val){
        values[N] = val;
    }

    void set(size_t n, _Ty const& val){
        values[n] = val;
    }

    template<size_t D>
    struct GenTuple{
        using TupleType = void;
    };

    template<>
    struct GenTuple<2>{
        using TupleType = std::tuple<_Ty,_Ty>;
        TupleType Gen(){
            return {values[0],values[1]};
        }
    };

    template<>
    struct GenTuple<3>{
        using TupleType = std::tuple<_Ty,_Ty,_Ty>;
        TupleType Gen(){
            return {values[0],values[1],values[2]};
        }
    };

    operator GenTuple<_Dimension>::TupleType(){
        return GenTuple<_Dimension>::Gen();
    }

private:
    _Ty values[_Dimension];
};

using Coord2 = Coord<double, 2>;
using Coord3 = Coord<double, 3>;

template<typename Coord>
struct GetCoordTupleType{
    using type = Coord::GenTuple<Coord::Dimension::value>::TupleType;
};

template<typename _Coord>
struct xInterval{
    using CoordType = _Coord;
    using Ret = std::pair<_Coord,_Coord>;
    const size_t Dimension = _Coord::Dimension;

    xInterval(){}

    xInterval(_Coord const& begin, _Coord const& end){
        m_begin = begin;
        m_end = end;
    }

    Ret get(){
        return {m_begin, m_end};
    }

    void set(_Coord const& begin, _Coord const& end){
        m_begin = begin;
        m_end = end;
    }

private:
    _Coord m_begin;
    _Coord m_end;
};

struct route{
    template<typename TimePoint>
    Coord2 at(TimePoint tp){
        return {};
    }
    template<typename TimePoint>
    int at_way(TimePoint tp){
        return {};
    }

    std::vector<int> get_ways(){
        return {};
    }
    int get_id(){
        return {};
    }
    double time_beg;
    double time_end;
};


using xCoordInterval2 = xInterval<Coord2>;
using xCoordInterval3 = xInterval<Coord2>;

bool inside(Coord2 coord,
    xCoordInterval2 interval)
{
    auto [beg,end] = interval.get();
    bool ret = (beg.get(0) <= coord.get(0) && coord.get(0) < end.get(0))
        && (beg.get(1) <= coord.get(1) && coord.get(1) < end.get(1));
    return ;
}

bool inside(Coord2 coord,
    xCoordInterval3 interval)
{
    auto [beg,end] = interval.get();
    bool ret = (beg.get(0) <= coord.get(0) && coord.get(0) < end.get(0))
        && (beg.get(1) <= coord.get(1) && coord.get(1) < end.get(1));
    return ;
}

struct GpsPoint{
    double lat;
    double lng;
    double timepoint;
};

struct way{};


namespace calc{

const int EARTH_RADIUS = 6378137;

double rad(double ang) {
    using namespace std::numbers;
	return ang * pi / 180;
}

/******************************************************************
 * 从google maps脚本中的计算距离代码抄的
 * https://blog.csdn.net/lp310018931/article/details/72457595
 * 传入：角度制的两个WGS84坐标
 * 传出：大地线距离
 ******************************************************************/
double GetDistance(double lat1, double lng1, double lat2, double lng2)
{
	double radLat1 = rad(lat1);
	double radLat2 = rad(lat2);
	double a = radLat1 - radLat2;
	double b = rad(lng1) - rad(lng2);
	double s = 2 * asin(sqrt(pow(sin(a / 2), 2) +
		cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
	s = s * EARTH_RADIUS;
	s = round(s * 10000) / 10000;
	return s;
}
}