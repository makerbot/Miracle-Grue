//
//  BGLTriangle3D.cc
//  Part of the Belfry Geometry Library
//
//  Created by GM on 10/14/10.
//  Copyright 2010 Belfry Software. All rights reserved.
//

#include "BGLTriangle3d.h"
#include "BGLPoint3d.h"
#include "BGLLine.h"

namespace BGL {


Triangle3d& Triangle3d::rotateX(const Point3d& center, Scalar rad)
{
    Scalar cr = cos(rad);
    Scalar sr = sin(rad);

    *this -= center;

    Point3d pt;
    pt.y = vertex1.y*cr - vertex1.z*sr;
    pt.z = vertex1.y*sr + vertex1.z*cr;
    pt.x = vertex1.x;
    vertex1 = pt;

    pt.y = vertex2.y*cr - vertex2.z*sr;
    pt.z = vertex2.y*sr + vertex2.z*cr;
    pt.x = vertex2.x;
    vertex2 = pt;

    pt.y = vertex3.y*cr - vertex3.z*sr;
    pt.z = vertex3.y*sr + vertex3.z*cr;
    pt.x = vertex3.x;
    vertex3 = pt;

    *this += center;
    return *this;
}



Triangle3d& Triangle3d::rotateY(const Point3d& center, Scalar rad)
{
    Scalar cr = cos(rad);
    Scalar sr = sin(rad);

    *this -= center;

    Point3d pt;
    pt.z = vertex1.z*cr - vertex1.x*sr;
    pt.x = vertex1.z*sr + vertex1.x*cr;
    pt.y = vertex1.y;
    vertex1 = pt;

    pt.z = vertex2.z*cr - vertex2.x*sr;
    pt.x = vertex2.z*sr + vertex2.x*cr;
    pt.y = vertex2.y;
    vertex2 = pt;

    pt.z = vertex3.z*cr - vertex3.x*sr;
    pt.x = vertex3.z*sr + vertex3.x*cr;
    pt.y = vertex3.y;
    vertex3 = pt;

    *this += center;
    return *this;
}



Triangle3d& Triangle3d::rotateZ(const Point3d& center, Scalar rad)
{
    Scalar cr = cos(rad);
    Scalar sr = sin(rad);

    *this -= center;

    Point3d pt;
    pt.x = vertex1.x*cr - vertex1.y*sr;
    pt.y = vertex1.x*sr + vertex1.y*cr;
    pt.z = vertex1.z;
    vertex1 = pt;

    pt.x = vertex2.x*cr - vertex2.y*sr;
    pt.y = vertex2.x*sr + vertex2.y*cr;
    pt.z = vertex2.z;
    vertex2 = pt;

    pt.x = vertex3.x*cr - vertex3.y*sr;
    pt.y = vertex3.x*sr + vertex3.y*cr;
    pt.z = vertex3.z;
    vertex3 = pt;

    *this += center;
    return *this;
}



bool Triangle3d::sliceAtZ(Scalar Z, Line& lnref) const
{
    Scalar u, px, py, v, qx, qy;
    if (vertex1 > Z && vertex2 > Z && vertex3 > Z) {
	// Triangle is above Z level.
        return false;
    }
    if (vertex1 < Z && vertex2 < Z && vertex3 < Z) {
	// Triangle is below Z level.
        return false;
    }
    if (vertex1 == Z) {
	if (vertex2 == Z) {
	    if (vertex3 == Z) {
                // flat face.  Ignore.
                return false;
            }
	    lnref = Line(Point(vertex1), Point(vertex2));
	    return true;
        }
	if (vertex3 == Z) {
	    lnref = Line(Point(vertex1), Point(vertex3));
	    return true;
        }
	if ((vertex2 > Z && vertex3 > Z) || (vertex2 < Z && vertex3 < Z)) {
            // only touches vertex1 tip.  Ignore.
            return false;
        }
        u = (Z-vertex2.z)/(vertex3.z-vertex2.z);
        px =  vertex2.x+u*(vertex3.x-vertex2.x);
        py =  vertex2.y+u*(vertex3.y-vertex2.y);
	lnref = Line(Point(vertex1), Point(px,py));
	return true;
    } else if (vertex2 == Z) {
	if (vertex3 == Z) {
	    lnref = Line(Point(vertex2), Point(vertex3));
	    return true;
        }
	if ((vertex1 > Z && vertex3 > Z) || (vertex1 < Z && vertex3 < Z)) {
            // only touches vertex2 tip.  Ignore.
            return false;
        }
        u = (Z-vertex1.z)/(vertex3.z-vertex1.z);
        px =  vertex1.x+u*(vertex3.x-vertex1.x);
        py =  vertex1.y+u*(vertex3.y-vertex1.y);
	lnref = Line(Point(vertex2), Point(px,py));
	return true;
    } else if (vertex3 == Z) {
	if ((vertex1 > Z && vertex2 > Z) || (vertex1 < Z && vertex2 < Z)) {
            // only touches vertex3 tip.  Ignore.
            return false;
        }
        u = (Z-vertex1.z)/(vertex2.z-vertex1.z);
        px =  vertex1.x+u*(vertex2.x-vertex1.x);
        py =  vertex1.y+u*(vertex2.y-vertex1.y);
	lnref = Line(Point(vertex3), Point(px,py));
	return true;
    } else if ((vertex1 > Z && vertex2 > Z) || (vertex1 < Z && vertex2 < Z)) {
        u = (Z-vertex3.z)/(vertex1.z-vertex3.z);
        px =  vertex3.x+u*(vertex1.x-vertex3.x);
        py =  vertex3.y+u*(vertex1.y-vertex3.y);
        v = (Z-vertex3.z)/(vertex2.z-vertex3.z);
        qx =  vertex3.x+v*(vertex2.x-vertex3.x);
        qy =  vertex3.y+v*(vertex2.y-vertex3.y);
	lnref = Line(Point(px,py), Point(qx,qy));
	return true;
    } else if ((vertex1 > Z && vertex3 > Z) || (vertex1 < Z && vertex3 < Z)) {
        u = (Z-vertex2.z)/(vertex1.z-vertex2.z);
        px =  vertex2.x+u*(vertex1.x-vertex2.x);
        py =  vertex2.y+u*(vertex1.y-vertex2.y);
        v = (Z-vertex2.z)/(vertex3.z-vertex2.z);
        qx =  vertex2.x+v*(vertex3.x-vertex2.x);
        qy =  vertex2.y+v*(vertex3.y-vertex2.y);
	lnref = Line(Point(px,py), Point(qx,qy));
	return true;
    } else if ((vertex2 > Z && vertex3 > Z) || (vertex2 < Z && vertex3 < Z)) {
        u = (Z-vertex1.z)/(vertex2.z-vertex1.z);
        px =  vertex1.x+u*(vertex2.x-vertex1.x);
        py =  vertex1.y+u*(vertex2.y-vertex1.y);
        v = (Z-vertex1.z)/(vertex3.z-vertex1.z);
        qx =  vertex1.x+v*(vertex3.x-vertex1.x);
        qy =  vertex1.y+v*(vertex3.y-vertex1.y);
	lnref = Line(Point(px,py), Point(qx,qy));
	return true;
    }
    return false;
}


}

