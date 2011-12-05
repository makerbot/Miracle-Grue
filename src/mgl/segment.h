/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef SEGMENT_H_
#define SEGMENT_H_



#include "meshy.h"

namespace mgl 
{

struct Segment
{
	BGL::Point a;
	BGL::Point b;
};

bool sameSame(double a, double b)
{
	return (a-b) * (a-b) < 0.00000001;
}


bool sliceTriangle(const BGL::Point3d& vertex1, const BGL::Point3d& vertex2, const BGL::Point3d& vertex3, Scalar Z, BGL::Point &a, BGL::Point &b)
{
	Scalar u, px, py, v, qx, qy;
	if (vertex1.z > Z && vertex2.z > Z && vertex3.z > Z)
	{
		// Triangle is above Z level.
		return false;
	}
	if (vertex1.z < Z && vertex2.z < Z && vertex3.z < Z)
	{
		// Triangle is below Z level.
		return false;
	}
	if (sameSame(vertex1.z, Z) )
	{
		if (sameSame(vertex2.z,Z) )
		{
			if (sameSame(vertex3.z,Z) )
			{
				// flat face.  Ignore.
				return false;
			}
			//lnref = Line(Point(vertex1), Point(vertex2));
			a.x = vertex1.x;
			a.y = vertex1.y;

			b.x = vertex2.x;
			b.y = vertex2.y;
			return true;
		}
		if (sameSame(vertex3.z,Z) )
		{
			// lnref = Line(Point(vertex1), Point(vertex3));
			a.x = vertex1.x;
			a.y = vertex1.y;
			b.x = vertex3.x;
			b.y = vertex3.y;
			return true;
		}
		if ((vertex2.z > Z && vertex3.z > Z) || (vertex2.z < Z && vertex3.z < Z))
		{
			// only touches vertex1 tip.  Ignore.
			return false;
		}
		u = (Z-vertex2.z)/(vertex3.z-vertex2.z);
		px =  vertex2.x+u*(vertex3.x-vertex2.x);
		py =  vertex2.y+u*(vertex3.y-vertex2.y);
		// lnref = Line(Point(vertex1), Point(px,py));
		a.x = vertex1.x;
		a.y = vertex1.y;
		b.x = px;
		b.y = py;
		return true;
	}
	else if (sameSame(vertex2.z, Z) )
	{
		if (sameSame(vertex3.z,Z) )
		{
			// lnref = Line(Point(vertex2), Point(vertex3));
			a.x = vertex2.x;
			a.y = vertex2.y;
			b.x = vertex3.x;
			b.y = vertex3.y;
			return true;
		}
		if ((vertex1.z > Z && vertex3.z > Z) || (vertex1.z < Z && vertex3.z < Z))
		{
			// only touches vertex2 tip.  Ignore.
			return false;
		}
		u = (Z-vertex1.z)/(vertex3.z-vertex1.z);
		px =  vertex1.x+u*(vertex3.x-vertex1.x);
		py =  vertex1.y+u*(vertex3.y-vertex1.y);
		// lnref = Line(Point(vertex2), Point(px,py));
		a.x = vertex2.x;
		a.y = vertex2.y;
		b.x = px;
		b.y = py;
		return true;
	}
	else if (sameSame(vertex3.z, Z) )
	{
		if ((vertex1.z > Z && vertex2.z > Z) || (vertex1.z < Z && vertex2.z < Z))
		{
			// only touches vertex3 tip.  Ignore.
			return false;
		}
		u = (Z-vertex1.z)/(vertex2.z-vertex1.z);
		px =  vertex1.x+u*(vertex2.x-vertex1.x);
		py =  vertex1.y+u*(vertex2.y-vertex1.y);
		// lnref = Line(Point(vertex3), Point(px,py));
		a.x = vertex3.x;
		a.y = vertex3.y;
		b.x = px;
		b.y = py;
		return true;
	}
	else if ((vertex1.z > Z && vertex2.z > Z) || (vertex1.z < Z && vertex2.z < Z))
	{
		u = (Z-vertex3.z)/(vertex1.z-vertex3.z);
		px =  vertex3.x+u*(vertex1.x-vertex3.x);
		py =  vertex3.y+u*(vertex1.y-vertex3.y);
		v = (Z-vertex3.z)/(vertex2.z-vertex3.z);
		qx =  vertex3.x+v*(vertex2.x-vertex3.x);
		qy =  vertex3.y+v*(vertex2.y-vertex3.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		b.x = qx;
		b.y = qy;
		return true;
	}
	else if ((vertex1.z > Z && vertex3.z > Z) || (vertex1.z < Z && vertex3.z < Z))
	{
		u = (Z-vertex2.z)/(vertex1.z-vertex2.z);
		px =  vertex2.x+u*(vertex1.x-vertex2.x);
		py =  vertex2.y+u*(vertex1.y-vertex2.y);
		v = (Z-vertex2.z)/(vertex3.z-vertex2.z);
		qx =  vertex2.x+v*(vertex3.x-vertex2.x);
		qy =  vertex2.y+v*(vertex3.y-vertex2.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		b.x = qx;
		b.y = qy;
		return true;
	}
	else if ((vertex2.z > Z && vertex3.z > Z) || (vertex2.z < Z && vertex3.z < Z))
	{
		u = (Z-vertex1.z)/(vertex2.z-vertex1.z);
		px =  vertex1.x+u*(vertex2.x-vertex1.x);
		py =  vertex1.y+u*(vertex2.y-vertex1.y);
		v = (Z-vertex1.z)/(vertex3.z-vertex1.z);
		qx =  vertex1.x+v*(vertex3.x-vertex1.x);
		qy =  vertex1.y+v*(vertex3.y-vertex1.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		b.x = qx;
		b.y = qy;
		return true;
	}
	return false;
}


void segmentology(const std::vector<BGL::Triangle3d> &allTriangles, const TriangleIndices &trianglesForSlice, double z, std::vector<Segment> &segments)
{
	assert(segments.size() == 0);
	segments.reserve(trianglesForSlice.size());
	for(TriangleIndices::const_iterator i= trianglesForSlice.begin(); i != trianglesForSlice.end(); i++)
	{
		const BGL::Triangle3d &t = allTriangles[(*i)];
		const BGL::Point3d &vertex0 = t.vertex1;
		const BGL::Point3d &vertex1 = t.vertex2;
		const BGL::Point3d &vertex2 = t.vertex3;

		Segment s;
		bool cut = sliceTriangle(vertex0, vertex1, vertex2, z, s.a, s.b);
		if(cut)
		{
			segments.push_back(s);
		}
	}
}

void translateSegments(std::vector<mgl::Segment> &segments, BGL::Point p)
{
	for(int i=0; i < segments.size(); i++)
	{
		segments[i].a += p;
		segments[i].b += p;
	}
}


BGL::Point rotate2d(const BGL::Point &p, Scalar angle)
{
	// rotate point
	double s = sin(angle); // radians
	double c = cos(angle);
	BGL::Point rotated;
	rotated.x = p.x * c - p.y * s;
	rotated.y = p.x * s + p.y * c;
	return rotated;
}




void rotateSegments(std::vector<mgl::Segment> &segments, Scalar angle)
{
	for(int i=0; i < segments.size(); i++)
	{
		segments[i].a = rotate2d(segments[i].a, angle);
		segments[i].b = rotate2d(segments[i].b, angle);
	}
}

bool segmentSegmentIntersection(Scalar p0_x, Scalar p0_y, Scalar p1_x, Scalar p1_y,
		Scalar p2_x, Scalar p2_y, Scalar p3_x, Scalar p3_y, Scalar &i_x, Scalar &i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        i_x = p0_x + (t * s1_x);
        i_y = p0_y + (t * s1_y);
        return true;
    }

    return false; // No collision
}

}

#endif
