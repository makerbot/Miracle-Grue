/* 
 * File:   intersection_index.h
 * Author: Dev
 *
 * Created on September 28, 2012, 5:29 PM
 */

#ifndef MGL_INTERSECTION_INDEX_H
#define	MGL_INTERSECTION_INDEX_H

#include "spacial_data.h"
#include <stdlib.h>

namespace mgl {

template <>
struct to_bbox<Segment2Type> {
    static AABBox bound(const Segment2Type& ls) {
        static const Point2Type epsilon(std::numeric_limits<Scalar>::epsilon(), 
                std::numeric_limits<Scalar>::epsilon());
        AABBox ret(ls.a);
        ret.expandTo(ls.b);
        ret.adjust(-epsilon, epsilon);
        return ret;
    }
};

class LineSegmentFilter : public BBoxFilter{
public:
    LineSegmentFilter(const Segment2Type& data = 
            Segment2Type())
            : BBoxFilter(to_bbox<Segment2Type>::bound(data)), 
            myData(data){}
    bool filter(const AABBox& bb) const {
        
        enum leg {
            l_les = -1,
            l_equ = 0,
            l_gre = 1
        };
        
        if(!BBoxFilter::filter(bb))
            return false;
        
        leg x1, y1, x2, y2;
        x1 = myData.a.x < bb.left() ? 
                l_les : (myData.a.x < bb.right() ? 
                l_equ : l_gre);
        x2 = myData.b.x < bb.left() ? 
                l_les : (myData.b.x < bb.right() ? 
                l_equ : l_gre);
        y1 = myData.a.y < bb.bottom() ? 
                l_les : (myData.a.y < bb.top() ? 
                l_equ : l_gre);
        y2 = myData.b.y < bb.bottom() ? 
                l_les : (myData.b.y < bb.top() ? 
                l_equ : l_gre);
        int x = x1 + x2;
        int y = y1 + y2;
        return (abs(x) < 2 && abs(y) < 2);
    }
protected:
    Segment2Type myData;
};

}

#endif	/* MGL_INTERSECTION_INDEX_H */

