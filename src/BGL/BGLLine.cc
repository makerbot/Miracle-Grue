//
//  BGLLine.m
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/13/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#include "BGLLine.h"

using namespace std;

namespace BGL {

bool Line::isLinearWith(const Point& pt) const
{
    if (startPt == endPt) {
         // Line segment is 0 length.
        if (pt == startPt) {
            return true; // Point is coincident.
        }
        return false;
    }
    if (pt == startPt || pt == endPt) {
        // Point is one of this line's endpoints.
        return true;
    }

    Scalar x1 = startPt.x;
    Scalar y1 = startPt.y;
    Scalar x2 = endPt.x;
    Scalar y2 = endPt.y;
    
    Scalar dx1 = x2 - x1;
    Scalar dy1 = y2 - y1;
    
    Scalar dx2 = x2 - pt.x;
    Scalar dy2 = y2 - pt.y;
    
    Scalar dx3 = x1 - pt.x;
    Scalar dy3 = y1 - pt.y;
    
    Scalar d  = dy2 * dx1 - dx2 * dy1;
    Scalar na = dx2 * dy3 - dy2 * dx3;
    Scalar nb = dx1 * dy3 - dy1 * dx3;
    
    if (d == 0) {
        if (fabsf(na) < EPSILON && fabsf(nb) < EPSILON) {
            return true;  // Lines are coincident.
        }
        return false; // Lines are parallel but not coincident.
    }
    return false;
}



bool Line::hasInBounds(const Point &pt) const
{
    Scalar px = pt.x;
    Scalar py = pt.y;
    Scalar sx = startPt.x;
    Scalar sy = startPt.y;
    Scalar ex = endPt.x;
    Scalar ey = endPt.y;
    
    if (px > sx && px > ex) {
        return false;
    }
    if (px < sx && px < ex) {
        return false;
    }
    if (py > sy && py > ey) {
        return false;
    }
    if (py < sy && py < ey) {
        return false;
    }
    return true;
}



bool Line::contains(const Point &pt) const
{
    if (this->hasInBounds(pt) && this->isLinearWith(pt)) {
        return true;
    }
    return false;
}



Point Line::closestSegmentPointTo(const Point &pt) const
{
    Scalar x1 = startPt.x;
    Scalar y1 = startPt.y;
    Scalar x2 = endPt.x;
    Scalar y2 = endPt.y;
    Scalar xd = x2 - x1;
    Scalar yd = y2 - y1;
    Scalar u;

    if (startPt == endPt) {
        return Point(startPt);
    }
    u = ((pt.x - x1) * xd + (pt.y - y1) * yd) / (xd * xd + yd * yd);
    if (u < 0.0) {
        return Point(startPt);
    } else if (u > 1.0) {
        return Point(endPt);
    }
    return Point(x1+u*xd, y1+u*yd);
}



Point Line::closestExtendedLinePointTo(const Point &pt) const
{
    Scalar x1 = startPt.x;
    Scalar y1 = startPt.y;
    Scalar x2 = endPt.x;
    Scalar y2 = endPt.y;
    Scalar xd = x2 - x1;
    Scalar yd = y2 - y1;
    Scalar u;
    
    if (startPt == endPt) {
        return Point(startPt);
    }
    u = ((pt.x - x1) * xd + (pt.y - y1) * yd) / (xd * xd + yd * yd);
    return Point(x1+u*xd, y1+u*yd);
}



Scalar Line::minimumSegmentDistanceFromPoint(const Point &pt) const
{
    Point closePt = closestSegmentPointTo(pt);
    return pt.distanceFrom(closePt);
}




Scalar Line::minimumExtendedLineDistanceFromPoint(const Point &pt) const
{
    Point closePt = closestExtendedLinePointTo(pt);
    return pt.distanceFrom(closePt);
}




// Returns the intersection of two line segments.
// If they don't intersect, the Intersection will have a type of NONE.
Intersection Line::intersectionWithSegment(const Line &ln) const
{
    Scalar x1 = startPt.x;
    Scalar y1 = startPt.y;
    Scalar x2 = endPt.x;
    Scalar y2 = endPt.y;
    Scalar x3 = ln.startPt.x;
    Scalar y3 = ln.startPt.y;
    Scalar x4 = ln.endPt.x;
    Scalar y4 = ln.endPt.y;

    Scalar dx1 = x2 - x1;
    Scalar dy1 = y2 - y1;
    
    Scalar dx2 = x4 - x3;
    Scalar dy2 = y4 - y3;
    
    Scalar dx3 = x1 - x3;
    Scalar dy3 = y1 - y3;
    
    Scalar d  = dy2 * dx1 - dx2 * dy1;
    Scalar na = dx2 * dy3 - dy2 * dx3;
    Scalar nb = dx1 * dy3 - dy1 * dx3;
    
    if (d == 0) {
	if (startPt == endPt) {
            // Line is actually a zero length directionless line. (AKA a point.)
            return Intersection();
	} else if (ln.startPt == ln.endPt) {
            // ln is actually a zero length directionless line. (AKA a point.)
            return Intersection();
        } else if (fabsf(na) < EPSILON && fabsf(nb) < EPSILON) {
            // Lines are coincident.  Check for overlap.
	    Point p0(startPt);
	    Point p1(endPt);
	    Point p2(ln.startPt);
	    Point p3(ln.endPt);
	    Point tmp;
            if (startPt > endPt) {
	        tmp = p0; p0 = p1; p1 = tmp;
            }
            if (ln.startPt > ln.endPt) {
	        tmp = p2; p2 = p3; p3 = tmp;
            }
	    if (p0 > p2) {
	        tmp = p0; p0 = p2; p2 = tmp;
		tmp = p1; p1 = p3; p3 = tmp;
            }
            if (p1 == p2) {
                return Intersection(p1,0);
	    } else if (p1 > p2) {
                return Intersection(p2,p1,0);
            }
        }
	return Intersection();
    }
    
    Scalar ua = na / d;
    Scalar ub = nb / d;
    
    if (ua < 0.0 || ua > 1.0) {
        // Intersection wouldn't be inside first segment
	return Intersection();
    }
    
    if (ub < 0.0 || ub > 1.0) {
        // Intersection wouldn't be inside second segment
	return Intersection();
    }
    
    Scalar xi = x1 + ua * dx1;
    Scalar yi = y1 + ua * dy1;
    
    return Intersection(Point(xi,yi),0);
}



// Returns the intersection of two extended lines.
// If they don't intersect, the Intersection will have a type of NONE.
// If they are coincident, the Intersection will have a type of COINCIDENT.
Intersection Line::intersectionWithExtendedLine(const Line &ln) const
{
    Scalar x1 = startPt.x;
    Scalar y1 = startPt.y;
    Scalar x2 = endPt.x;
    Scalar y2 = endPt.y;
    
    Scalar dx1 = x2 - x1;
    Scalar dy1 = y2 - y1;
    
    Scalar dx2 = ln.endPt.x - ln.startPt.x;
    Scalar dy2 = ln.endPt.y - ln.startPt.y;
    
    Scalar dx3 = x1 - ln.startPt.x;
    Scalar dy3 = y1 - ln.startPt.y;
    
    Scalar d  = dy2 * dx1 - dx2 * dy1;
    Scalar na = dx2 * dy3 - dy2 * dx3;
    Scalar nb = dx1 * dy3 - dy1 * dx3;
    
    if (d == 0) {
        if (na == 0.0 && nb == 0.0) {
	    // Lines are coincident.
	    return Intersection(0);
        } else {
	    // No intersection; lines are parallel
	    return Intersection();
	}
    }
    
    Scalar ua = na / d;
    //Scalar ub = nb / d;
    Scalar xi = x1 + ua * dx1;
    Scalar yi = y1 + ua * dy1;
    return Intersection(Point(xi,yi),0);
}



ostream& operator <<(ostream &os, const Line &ln)
{
    os << "[" << ln.startPt << " - " << ln.endPt << "]";
    return os;
}


}

