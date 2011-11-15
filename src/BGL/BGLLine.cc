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
    return (minimumExtendedLineDistanceFromPoint(pt) < CLOSEENOUGH);
}



bool Line::hasInBounds(const Point &pt) const
{
    double px = pt.x;
    double py = pt.y;
    double sx = startPt.x;
    double sy = startPt.y;
    double ex = endPt.x;
    double ey = endPt.y;
    
    if (px > sx+EPSILON && px > ex+EPSILON) {
        return false;
    }
    if (px < sx-EPSILON && px < ex-EPSILON) {
        return false;
    }
    if (py > sy+EPSILON && py > ey+EPSILON) {
        return false;
    }
    if (py < sy-EPSILON && py < ey-EPSILON) {
        return false;
    }
    return true;
}



bool Line::contains(const Point &pt) const
{
    if (hasInBounds(pt)) {
	return (minimumSegmentDistanceFromPoint(pt) < CLOSEENOUGH);
    }
    return false;
}



Point Line::closestSegmentPointTo(const Point &pt) const
{
    double x1 = startPt.x;
    double y1 = startPt.y;
    double x2 = endPt.x;
    double y2 = endPt.y;
    double xd = x2 - x1;
    double yd = y2 - y1;
    double u;

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
    double x1 = startPt.x;
    double y1 = startPt.y;
    double x2 = endPt.x;
    double y2 = endPt.y;
    double xd = x2 - x1;
    double yd = y2 - y1;
    double u;
    
    if (startPt == endPt) {
        return Point(startPt);
    }
    u = ((pt.x - x1) * xd + (pt.y - y1) * yd) / (xd * xd + yd * yd);
    return Point(x1+u*xd, y1+u*yd);
}



double Line::minimumSegmentDistanceFromPoint(const Point &pt) const
{
    Point closePt = closestSegmentPointTo(pt);
    return pt.distanceFrom(closePt);
}




double Line::minimumExtendedLineDistanceFromPoint(const Point &pt) const
{
    Point closePt = closestExtendedLinePointTo(pt);
    return pt.distanceFrom(closePt);
}




// Returns the intersection of two line segments.
// If they don't intersect, the Intersection will have a type of NONE.
Intersection Line::intersectionWithSegment(const Line &ln) const
{
    bool dodebug = false;
    /*
    if (startPt.distanceFrom(Point(20.500,27.188)) <= 0.001) {
	if (ln.startPt.distanceFrom(Point(1.419,15.262)) <= 0.001) {
	    dodebug = true;
	}
    }
    */

    if (startPt == endPt) {
	// Line is actually a zero length directionless line. (AKA a point.)
	if (dodebug) {
	    cerr << "    exit A" << endl;
	}
	return Intersection();
    } else if (ln.startPt == ln.endPt) {
	// ln is actually a zero length directionless line. (AKA a point.)
	if (dodebug) {
	    cerr << "    exit B" << endl;
	}
	return Intersection();
    }

    double x1 = startPt.x;
    double y1 = startPt.y;
    double x2 = endPt.x;
    double y2 = endPt.y;
    double x3 = ln.startPt.x;
    double y3 = ln.startPt.y;
    double x4 = ln.endPt.x;
    double y4 = ln.endPt.y;

    double dx1 = x2 - x1;
    double dy1 = y2 - y1;
    
    double dx2 = x4 - x3;
    double dy2 = y4 - y3;
    
    double dx3 = x1 - x3;
    double dy3 = y1 - y3;
    
    double d  = dy2 * dx1 - dx2 * dy1;
    double na = dx2 * dy3 - dy2 * dx3;
    double nb = dx1 * dy3 - dy1 * dx3;
    
    cerr.precision(18);
    cerr.setf(ios::fixed);

    if (dodebug) {
	cerr << "testing isect of " << *this << " and " << ln << endl;
	cerr << "d = " << d << " for " << *this << " and " << ln << endl;
    }
    if (fabs(d) <= EPSILON) {
	if (dodebug) {
	    cerr << "PARALLEL" << endl;
	}
	if (
	    minimumExtendedLineDistanceFromPoint(ln.startPt) < CLOSEENOUGH &&
	    minimumExtendedLineDistanceFromPoint(ln.endPt) < CLOSEENOUGH
	) {
            // Lines are coincident (or very close to).  Check for overlap.
	    Points isects;
	    if (ln.contains(startPt)) {
		isects.push_back(startPt);
	    }
	    if (ln.contains(endPt)) {
		isects.push_back(endPt);
	    }
	    if (contains(ln.startPt)) {
		if (!hasEndPoint(ln.startPt)) {
		    isects.push_back(ln.startPt);
		}
	    }
	    if (contains(ln.endPt)) {
		if (!hasEndPoint(ln.endPt)) {
		    isects.push_back(ln.endPt);
		}
	    }
	    int icnt = isects.size();
	    if (icnt == 0) {
		if (dodebug) {
		    cerr << "    exit C" << endl;
		}
		return Intersection();
	    } else if (icnt == 1) {
		if (dodebug) {
		    cerr << "    exit D" << endl;
		}
                return Intersection(isects.front(),0);
	    } else  {
		if (dodebug) {
		    cerr << "    exit E" << endl;
		}
		if (isects.front() == isects.back()) {
		    return Intersection(isects.front(),0);
		}
                return Intersection(isects.front(),isects.back(),0);
            }
        }
	if (dodebug) {
	    cerr << "    exit F" << endl;
	}
	return Intersection();
    }
    
    double ua = na / d;
    double ub = nb / d;
    
    double xi = x1 + ua * dx1;
    double yi = y1 + ua * dy1;
    
    if (dodebug) {
	cerr << "    ua=" << ua << ", ub=" << ub << endl;
	cerr << "    isect at " << xi << ", " << yi << endl;
    }
    if (ua < -EPSILON || ua > 1.0+EPSILON) {
        // Intersection wouldn't be inside first segment
	return Intersection();
    }
    
    if (ub < -EPSILON || ub > 1.0+EPSILON) {
        // Intersection wouldn't be inside second segment
	return Intersection();
    }
    
    if (ua < 0.0) {
	return Intersection(Point(x1,y1),0);
    } else if (ua > 1.0) {
	return Intersection(Point(x2,y2),0);
    } else if (ub < 0.0) {
	return Intersection(Point(x3,y3),0);
    } else if (ub > 1.0) {
	return Intersection(Point(x4,y4),0);
    }
    return Intersection(Point(xi,yi),0);
}



// Returns the intersection of two extended lines.
// If they don't intersect, the Intersection will have a type of NONE.
// If they are coincident, the Intersection will have a type of COINCIDENT.
Intersection Line::intersectionWithExtendedLine(const Line &ln) const
{
    double x1 = startPt.x;
    double y1 = startPt.y;
    double x2 = endPt.x;
    double y2 = endPt.y;
    
    double dx1 = x2 - x1;
    double dy1 = y2 - y1;
    
    double dx2 = ln.endPt.x - ln.startPt.x;
    double dy2 = ln.endPt.y - ln.startPt.y;
    
    double dx3 = x1 - ln.startPt.x;
    double dy3 = y1 - ln.startPt.y;
    
    double d  = dy2 * dx1 - dx2 * dy1;
    double na = dx2 * dy3 - dy2 * dx3;
    //double nb = dx1 * dy3 - dy1 * dx3;
    
    if (fabs(d) <= EPSILON) {
        if (
	    minimumExtendedLineDistanceFromPoint(ln.startPt) < CLOSEENOUGH &&
	    minimumExtendedLineDistanceFromPoint(ln.endPt) < CLOSEENOUGH
	) {
	    // Lines are coincident.
	    return Intersection(0);
        } else {
	    // No intersection; lines are parallel
	    return Intersection();
	}
    }
    
    double ua = na / d;
    //double ub = nb / d;
    double xi = x1 + ua * dx1;
    double yi = y1 + ua * dy1;
    return Intersection(Point(xi,yi),0);
}



Line& Line::leftOffset(double offsetby)
{
    double ang = angle();
    double pang = ang + M_PI_2;  /* 90 degs ccw */
    Point offPt;
    offPt.polarOffset(pang, offsetby);
    startPt += offPt;
    endPt += offPt;
    return *this;
}



ostream& operator <<(ostream &os, const Line &ln)
{
    os << "[" << ln.startPt << " - " << ln.endPt << "]";
    return os;
}


}

