#include "spacial_data.h"

namespace mgl {

void AABBox::growTo(PointType pt) {
    m_min.x = std::min(m_min.x, pt.x);
    m_min.y = std::min(m_min.y, pt.y);
    m_max.x = std::max(m_max.x, pt.x);
    m_max.y = std::max(m_max.y, pt.y);
}
void AABBox::reset(PointType pt) {
    m_min = m_max = pt;
}
bool AABBox::contains(const AABBox& other) const {
    return (other.right() < right() && left() < other.left() && 
            other.top() < top() && bottom() < other.bottom());
}
bool AABBox::intersects(const AABBox& other) const {
    return !(other.right() < left() || right() < other.left() 
            || other.top() < bottom() || top() < other.bottom());
}
bool AABBox::fits(const AABBox& other) const {
    return other.size_x() < size_x() && other.size_y() < size_y();
}

}


