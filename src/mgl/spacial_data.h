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
    AABBox(Point2Type pt = Point2Type()) : m_min(pt), m_max(pt) {}
    AABBox(Point2Type bl, Point2Type tr) : m_min(bl), m_max(tr) {}
    void expandTo(Point2Type pt);
    void expandTo(const AABBox& bb);
    AABBox expandedTo(Point2Type pt) const;
    AABBox expandedTo(const AABBox& bb) const;
    void reset(Point2Type pt = Point2Type());
    void adjust(Point2Type minDelta, Point2Type maxDelta);
    AABBox adjusted(Point2Type minDelta, Point2Type maxDelta) const;
        
    Scalar left() const { return m_min.x; }
    Scalar right() const { return m_max.x; }
    Scalar bottom() const { return m_min.y; }
    Scalar top() const { return m_max.y; }
    Point2Type bottom_center() const { return Point2Type((m_min.x + m_max.x) * 0.5 , m_min.y); }
    Point2Type bottom_left() const { return m_min; }
    Point2Type bottom_right() const { return Point2Type(m_max.x, m_min.y); }
    Point2Type top_center() const { return Point2Type((m_min.x + m_max.x) * 0.5 , m_max.y); }
    Point2Type top_left() const { return Point2Type(m_min.x, m_max.y); }
    Point2Type top_right() const { return m_max; }
    Point2Type left_center() const { return Point2Type(m_min.x, (m_min.y + m_max.y) * 0.5); }
    Point2Type right_center() const { return Point2Type(m_max.x, (m_min.y + m_max.y) * 0.5); }
    Point2Type center() const { return (m_min + m_max) * 0.5; }
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
    
    Point2Type m_min;
    Point2Type m_max;
};

/**
 @breif A base template for converting objects into bounding boxes.
 @param T the type of object which is converted into a bounding box
 This struct defines a static function AABBox bound(const T&) where T
 is the type of object from which you wish to generate a bounding box.
 Loops, paths, lines, and points are valid examples.
 
 There is no generic implementation. You must provide one for any type 
 you wish to store in a spacial index. Consider the example for a line:
 
 template <>
 struct to_bbox<Segment2Type> {
    static AABBox bound(const Segment2Type& line) {
        AABBox box(line.a);
        box.expandTo(line.b);
        return box;
    }
 };
 */
template <typename T>
struct to_bbox{
    static AABBox bound(const T&);
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

