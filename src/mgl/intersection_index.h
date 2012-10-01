/* 
 * File:   intersection_index.h
 * Author: Dev
 *
 * Created on September 28, 2012, 5:29 PM
 */

#ifndef MGL_INTERSECTION_INDEX_H
#define	MGL_INTERSECTION_INDEX_H

#include "spacial_data.h"
#include "libthing/LineSegment2.h"
#include <stdlib.h>

namespace mgl {

template <>
struct to_bbox<libthing::LineSegment2> {
    static AABBox bound(const libthing::LineSegment2& ls) {
        AABBox ret(ls.a);
        ret.growTo(ls.b);
        return ret;
    }
};

class LineSegmentFilter : public BBoxFilter{
public:
    LineSegmentFilter(const libthing::LineSegment2& data = 
            libthing::LineSegment2())
            : BBoxFilter(to_bbox<libthing::LineSegment2>::bound(data)), 
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
    libthing::LineSegment2 myData;
};

}

#endif	/* MGL_INTERSECTION_INDEX_H */

