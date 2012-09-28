/* 
 * File:   spacial_data.h
 * Author: Dev
 *
 * Created on September 28, 2012, 4:26 PM
 */

#ifndef MGL_SPACIAL_DATA_H
#define	MGL_SPACIAL_DATA_H

#include "mgl.h"
#include <stdexcept>
#include <limits>

namespace mgl {

class AABBox {
public:
    AABBox(PointType pt = PointType()) : m_min(pt), m_max(pt) {}
    void growTo(PointType pt) {
        m_min.x = std::min(m_min.x, pt.x);
        m_min.y = std::min(m_min.y, pt.y);
        m_max.x = std::max(m_max.x, pt.x);
        m_max.y = std::max(m_max.y, pt.y);
    }
        
    Scalar left() const { return m_min.x; }
    Scalar right() const { return m_max.x; }
    Scalar bottom() const { return m_min.y; }
    Scalar top() const { return m_max.y; }
    
    bool contains(const AABBox& other) const {
        return (other.right() < right() && left() < other.left() && 
                other.top() < top() && bottom() < other.bottom());
    }
    bool intersects(const AABBox& other) const {
        return !(other.right() < left() || right() < other.left() 
                || other.top() < bottom() || top() < other.bottom());
    }
    
    PointType m_min;
    PointType m_max;
};

template <typename T>
struct to_bbox{
    static AABBox bound(const T&) {
        throw std::logic_error("Unimplemented, must specialize!");
    }
};

class BBoxFilter{
public:
    BBoxFilter(const AABBox& bb = AABBox()) 
            : myBound(bb) {}
    bool filter(const AABBox& bb) const;
private:
    AABBox myBound;
};

}

#endif	/* MGL_SPACIAL_DATA_H */

