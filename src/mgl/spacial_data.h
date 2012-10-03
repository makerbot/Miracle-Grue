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
    void expandTo(PointType pt);
    void expandTo(const AABBox& bb);
    AABBox expandedTo(PointType pt) const;
    AABBox expandedTo(const AABBox& bb) const;
    void reset(PointType pt = PointType());
    void adjust(PointType minDelta, PointType maxDelta);
    AABBox adjusted(PointType minDelta, PointType maxDelta) const;
        
    Scalar left() const { return m_min.x; }
    Scalar right() const { return m_max.x; }
    Scalar bottom() const { return m_min.y; }
    Scalar top() const { return m_max.y; }
    Scalar size_x() const { return m_max.x - m_min.x; }
    Scalar size_y() const { return m_max.y - m_min.y; }
    Scalar intersectiondistance_x(const AABBox& other) const;
    Scalar intersectiondistance_y(const AABBox& other) const;
    Scalar area() const { return size_x() * size_y(); }
    Scalar perimeter() const { return (size_x() + size_y()) * 2; }
    
    bool contains(const AABBox& other) const;
    bool intersects(const AABBox& other) const;
    bool fits(const AABBox& other) const;
    bool intersects_x(const AABBox& other) const;
    bool intersects_y(const AABBox& other) const;
    /*!Returns shared area if intersecting, 0 if not*/
    Scalar intersectionArea(const AABBox& other) const;
    
    static Scalar coordinateIntersectionDistance(Scalar min1, Scalar max1, 
            Scalar min2, Scalar max2);
    static bool coordinateIntersects(Scalar min1, Scalar max1, 
            Scalar min2, Scalar max2);
    
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

