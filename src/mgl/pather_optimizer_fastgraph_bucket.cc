#include <list>

#include "pather_optimizer_fastgraph.h"

namespace mgl {

#define BUCKET pather_optimizer_fastgraph::bucket

BUCKET::bucket(Point2Type testPoint) 
        : m_testPoint(testPoint), m_insideCount(0), m_empty(true) {}
bool BUCKET::contains(Point2Type point) const {
    Segment2Type testLine(m_infinitePoint, point);
    return countIntersections(testLine, m_bounds) & 1;  //even-odd test
}
bool BUCKET::contains(const bucket& other) const {
    return contains(other.m_testPoint);
}
void BUCKET::insertBoundary(const Segment2Type& line) {
    m_bounds.insert(line);
    if(m_empty) {
        m_limits = AABBox(line.a);
        m_limits.expandTo(line.b);
        m_empty = false;
    } else {
        m_limits.expandTo(line.a);
        m_limits.expandTo(line.b);
    }
}
void BUCKET::insertBoundary(const Loop& loop) {
    Point2Type testPoint = *loop.clockwise();
    for(bucket_list::iterator childIter = m_children.begin();
            childIter = m_children.end(); 
            ++childIter) {
        if(childIter->contains(testPoint)) {
            childIter->insertBoundary(loop);
            return;
        }
    }
    m_children.push_back(bucket(testPoint));
    for(Loop::const_finite_cw_iterator loopIter = loop.clockwiseFinite(); 
            loopIter != loop.clockwiseEnd(); 
            ++loopIter) {
        m_children.back().insertBoundary(loop.segmentAfterPoint(loopIter));
    }
}
BUCKET& BUCKET::select(Point2Type point) {
    for(bucket_list::iterator childIter = m_children.begin();
            childIter = m_children.end(); 
            ++childIter) {
        if(childIter->contains(point))
            return childIter->select(point);
    }
    return *this;
}
void BUCKET::optimize(LabeledOpenPaths& , Point2Type& ) {
    
}

#undef BUCKET

}


