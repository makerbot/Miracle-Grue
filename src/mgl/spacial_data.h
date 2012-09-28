/* 
 * File:   spacial_data.h
 * Author: Dev
 *
 * Created on September 28, 2012, 4:26 PM
 */

#ifndef MGL_SPACIAL_DATA_H
#define	MGL_SPACIAL_DATA_H

#include "mgl.h"

namespace mgl {

class AABBox {
public:
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
    BBoxFilter(const AABBox& bb = AABBox());
    bool filter(const AABBox& bb) const;
private:
    AABBox myBound;
};

}

#endif	/* MGL_SPACIAL_DATA_H */

