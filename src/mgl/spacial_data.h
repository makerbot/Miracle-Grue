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
    void growTo(PointType pt);
    void reset(PointType pt = PointType());
        
    Scalar left() const { return m_min.x; }
    Scalar right() const { return m_max.x; }
    Scalar bottom() const { return m_min.y; }
    Scalar top() const { return m_max.y; }
    Scalar size_x() const { return m_max.x - m_min.x; }
    Scalar size_y() const { return m_max.y - m_min.y; }
    
    bool contains(const AABBox& other) const;
    bool intersects(const AABBox& other) const;
    bool fits(const AABBox& other) const;
    
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
    bool filter(const AABBox& bb) const {
        return myBound.intersects(bb);
    }
protected:
    AABBox myBound;
};

}

#endif	/* MGL_SPACIAL_DATA_H */

