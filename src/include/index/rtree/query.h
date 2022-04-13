namespace index::rtree::query{

template<typename Interval>
detail::intersect<Interval>
intersect(Interval interval){
    return detail::intersect<Interval>(interval);
}

template<typename Interval>
detail::within<Interval>
within(Interval interval){
    return detail::within<Interval>(interval);
}

template<typename PointOrRelation>
detail::nearest<PointOrRelation>
nearest(PointOrRelation const& por, size_t n){
    return detail::nearest<PointOrRelation>(por,n);
}

}

namespace index::rtree::query::detail{

template<typename Interval>
struct intersect{
    intersect(Interval const& interval)
        : m_interval(interval)
    {}
    Interval m_interval;
};

template<typename Interval>
struct within{
    within(Interval const& interval)
        : m_interval(interval)
    {}
    Interval m_interval;
};

template<typename PointOrRelation>
struct nearest{
    nearest(PointOrRelation const& por, size_t n)
        : m_por(por),
          m_n(n)
    {}
    PointOrRelation m_por;
    size_t m_n;
};

}