//
//  BGLIntersection.h
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#ifndef BGL_INTERSECTION_H
#define BGL_INTERSECTION_H

#include <list>
#include "config.h"
#include "BGLCommon.h"
#include "BGLPoint.h"

namespace BGL {


typedef enum {
    NONE,
    POINT,
    SEGMENT,
    LINE
} IntersectionType;


class Line;

class Intersection {
public:
    IntersectionType type;
    Point p1, p2;
    int segment;

    Intersection() : type(NONE), p1(), p2(), segment(0) {}
    Intersection(int segnum) : type(LINE), p1(), p2(), segment(segnum) {}
    Intersection(const Point& pt1, int segnum) : type(POINT), p1(pt1), p2(), segment(segnum) {}
    Intersection(const Point& pt1, const Point& pt2, int segnum) : type(SEGMENT), p1(pt1), p2(pt2), segment(segnum) {}

    Line line() const;

    void quantize(float quanta);
    void quantize();
};

typedef list<Intersection> Intersections;


}
#endif

