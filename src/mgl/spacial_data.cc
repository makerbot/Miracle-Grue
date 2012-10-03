#include "spacial_data.h"

namespace mgl {

void AABBox::growTo(PointType pt) {
    using std::min;
    using std::max;
    m_min.x = min(m_min.x, pt.x);
    m_min.y = min(m_min.y, pt.y);
    m_max.x = max(m_max.x, pt.x);
    m_max.y = max(m_max.y, pt.y);
}
void AABBox::growTo(const AABBox& bb) {
    using std::min;
    using std::max;
    m_min.x = min(m_min.x, bb.m_min.x);
    m_min.y = min(m_min.y, bb.m_min.y);
    m_max.x = max(m_max.x, bb.m_max.x);
    m_max.y = max(m_max.y, bb.m_max.y);
}
void AABBox::reset(PointType pt) {
    m_min = m_max = pt;
}
void AABBox::adjust(PointType minDelta, PointType maxDelta) {
    m_min += minDelta;
    m_max += maxDelta;
}
AABBox AABBox::adjusted(PointType minDelta, PointType maxDelta) const {
    AABBox copy = *this;
    copy.adjust(minDelta, maxDelta);
    return copy;
}
Scalar AABBox::intersectiondistance_x(const AABBox& other) const {
    using std::min;
    using std::max;
    if (!intersects_x(other))
        return 0;
    Scalar a = max(m_min.x, other.m_min.x);
    Scalar b = min(m_max.x, other.m_max.x);
    return b - a;
}
Scalar AABBox::intersectiondistance_y(const AABBox& other) const {
    using std::min;
    using std::max;
    if (!intersects_y(other))
        return 0;
    Scalar a = max(m_min.y, other.m_min.y);
    Scalar b = min(m_max.y, other.m_max.y);
    return b - a;
}
bool AABBox::intersects_x(const AABBox& other) const {
    return coordinateIntersects(left(), right(), other.left(), other.right());
}
bool AABBox::intersects_y(const AABBox& other) const {
    return coordinateIntersects(bottom(), top(), other.bottom(), other.top());
}
bool AABBox::contains(const AABBox& other) const {
    return (other.right() < right() && left() < other.left() && 
            other.top() < top() && bottom() < other.bottom());
}
bool AABBox::intersects(const AABBox& other) const {
    return intersects_x(other) && intersects_y(other);
}
bool AABBox::fits(const AABBox& other) const {
    return other.size_x() < size_x() && other.size_y() < size_y();
}
Scalar AABBox::intersectionArea(const AABBox& other) const {
    return intersectiondistance_x(other) * intersectiondistance_y(other);
}

bool AABBox::coordinateIntersects(Scalar min1, Scalar max1, Scalar min2, Scalar max2) {
    return !(max2 < min1 || max1 < min2);
}

}


