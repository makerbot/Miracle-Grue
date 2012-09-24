/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


//#include "meshy.h"
//#include "shrinky.h"
#include "segment.h"

#include <stdint.h>
#include <cstring>

using namespace mgl;
using namespace std;

#include "log.h"
#include "loop_path.h"


void mgl::segments2polygon(const std::vector<SegmentType> & segments, mgl::Polygon &loop)
{

    loop.reserve(segments.size());
    for(size_t j = 0;j < segments.size();j++){
        const SegmentType & line = segments[j];
        PointType p(line.a);
        loop.push_back(p);
        if(j == segments.size() - 1){
            PointType p(line.b);
            loop.push_back(p);
        }
    }

}


void mgl::createPolysFromloopSegments(const SegmentTable &segmentTable,
										Polygons& loops)
{
	// outline loops
	size_t count = segmentTable.size();
	for(size_t i=0; i < count; i++)
	{
		const std::vector<SegmentType> &segments = segmentTable[count-1 - i];
		loops.push_back(Polygon());
		Polygon &loop = loops[loops.size()-1];
	    segments2polygon(segments, loop);
	}
}

void mgl::rotateLoops(LoopList &loops, Scalar angle) {
	for (LoopList::iterator loop = loops.begin();
		 loop != loops.end(); ++loop) {
		for (Loop::finite_cw_iterator point = loop->clockwiseFinite();
			point != loop->clockwiseEnd(); ++point) {
			//point->rotate2d(angle);
			point->setPoint(point->getPoint().rotate2d(angle));
		}
	}
}

void mgl::rotateLoops(std::vector<std::vector<SegmentType> > &loops, Scalar angle)
{
	for(size_t i=0; i < loops.size(); i++)
	{
		rotateSegments(loops[i], angle);
	}
}

void mgl::rotateSegments(std::vector<SegmentType> &segments, Scalar angle)
{
	for(size_t i=0; i < segments.size(); i++)
	{
		segments[i].a = segments[i].a.rotate2d(angle);
		segments[i].b = segments[i].b.rotate2d(angle);
	}
}

bool mgl::segmentSegmentIntersection(Scalar p0_x, Scalar p0_y,
									 Scalar p1_x, Scalar p1_y,
									 Scalar p2_x, Scalar p2_y,
									 Scalar p3_x, Scalar p3_y,
									 Scalar &i_x, Scalar &i_y)
{
    Scalar s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;
    s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;
    s2_y = p3_y - p2_y;

    Scalar s, t;
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

bool mgl::segmentSegmentIntersection(const SegmentType &s0, const SegmentType &s1, PointType &p)
{

	bool s;
	s = segmentSegmentIntersection(	s0.a[0], s0.a[1],
									s0.b[0], s0.b[1],
									s1.a[0], s1.a[1],
									s1.b[0], s1.b[1],
									p.x, p.y);
	return s;
}

void mgl::segmentationOfTriangles(const TriangleIndices &trianglesForSlice,
		const std::vector<TriangleType> &allTriangles,
		Scalar z,
		std::vector<SegmentType> &segments)
{
    size_t triangleCount = trianglesForSlice.size();
    segments.reserve(triangleCount);
    //#pragma omp parallel for
    for(size_t i = 0;i < triangleCount;i++)
    {
        index_t triangleIndex = trianglesForSlice[i];
        const TriangleType & triangle = allTriangles[triangleIndex];
        Point3Type a, b;
        // bool cut = sliceTriangle(triangle[0], triangle[1], triangle[2], z, a, b);
        bool cut = triangle.cut(z, a, b);
        if(cut){
        	SegmentType s;
            s.a.x = a.x;
            s.a.y = a.y;
            s.b.x = b.x;
            s.b.y = b.y;
            //s.triangle = triangleIndex;

            segments.push_back(s);
        }
    }

}

///// Returns 's's relation to 'to' using -1, 0, or 1
//
//short compare(const Scalar& s, const Scalar& to, Scalar tol) {
//	if(sameSame(s, to, tol))
//		return 0;
//	else if(s - to < 0)
//		return -1;
//	else
//		return 1;
//}
//
//bool sliceSegment() {
//
//	u = (Z-vertex2.z)/(vertex3.z-vertex2.z);
//	px =  vertex2.x+u*(vertex3.x-vertex2.x);
//	py =  vertex2.y+u*(vertex3.y-vertex2.y);
//}
//
/////
/////
/////
//bool newSliceTriangle(const Point3Type& vertex1,
//					  const Point3Type& vertex2,
//					  const Point3Type& vertex3,
//					  Scalar Z,
//					  Scalar tol,
//					  Vector2 &a,
//					  Vector2 &b)
//{
//
//	Scalar u, px, py, v, qx, qy;
//
//	short v1ToZ = compare(vertex1.z, Z, tol);
//	short v2ToZ = compare(vertex2.z, Z, tol);
//	short v3ToZ = compare(vertex3.z, Z, tol);
//
//	// flat face.  Ignore.
//	if(v1ToZ == 0 && v2ToZ == 0 && v3ToZ == 0)
//		return false;
//
//	// Triangle is above/below Z level
//	short sum = v1ToZ + v2ToZ + v3ToZ;
//	if(sum == 3 || sum == -3)
//		return false;
//
//	//case where one triangle falls in between other two
//		//slice other two, get two triangles
//	hugo wants this to calculate four line segments,
//		in preparation for future representations of triangles
//
//}


bool mgl::sliceTriangle(const Point3Type& vertex1,
		const Point3Type& vertex2,
		const Point3Type& vertex3,
		Scalar Z, Point3Type &a, Point3Type &b)
{
	Scalar tol = 1e-6;

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
	if (TEQUALS(vertex1.z, Z, tol) )
	{
		if (TEQUALS(vertex2.z,Z, tol) )
		{
			if (TEQUALS(vertex3.z,Z, tol) )
			{
				// flat face.  Ignore.
				return false;
			}
//			lnref = Line(Point(vertex1), Point(vertex2));
			a.x = vertex1.x;
			a.y = vertex1.y;
			a.z = Z;
			b.x = vertex2.x;
			b.y = vertex2.y;
			b.z = Z;
			return true;
		}
		if (TEQUALS(vertex3.z,Z, tol) )
		{
			// lnref = Line(Point(vertex1), Point(vertex3));
			a.x = vertex1.x;
			a.y = vertex1.y;
			a.z = Z;

			b.x = vertex3.x;
			b.y = vertex3.y;
			b.z = Z;
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
		a.z = Z;
		b.x = px;
		b.y = py;
		b.z = Z;
		return true;
	}
	else if (TEQUALS(vertex2.z, Z, tol) )
	{
		if (TEQUALS(vertex3.z,Z, tol) )
		{
			// lnref = Line(Point(vertex2), Point(vertex3));
			a.x = vertex2.x;
			a.y = vertex2.y;
			a.z = Z;
			b.x = vertex3.x;
			b.y = vertex3.y;
			b.z = Z;
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
		a.z = Z;
		b.x = px;
		b.y = py;
		b.z = Z;
		return true;
	}
	else if (TEQUALS(vertex3.z, Z, tol) )
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
		a.z = Z;
		b.x = px;
		b.y = py;
		b.z = Z;
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
		a.z = Z;
		b.x = qx;
		b.y = qy;
		b.z = Z;
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
		a.z = Z;
		b.x = qx;
		b.y = qy;
		b.z = Z;
		return true;
	}
	else if ((vertex2.z > Z && vertex3.z > Z) || (vertex2.z < Z && vertex3.z < Z))
	{
		u = (Z-vertex1.z)/(vertex2.z-vertex1.z);//
		px =  vertex1.x+u*(vertex2.x-vertex1.x);
		py =  vertex1.y+u*(vertex2.y-vertex1.y);
		v = (Z-vertex1.z)/(vertex3.z-vertex1.z);
		qx =  vertex1.x+v*(vertex3.x-vertex1.x);
		qy =  vertex1.y+v*(vertex3.y-vertex1.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		a.z = Z;
		b.x = qx;
		b.y = qy;
		b.z = Z;
		return true;
	}
	return false;
}


// given a point, finds the LineSegment2 that starts the closest from that point
// and return the distance. Also, the iterator to the closest LineSegment2 is "returned"
Scalar findClosestLineSegment2(const PointType& endOfPreviousLineSegment2,
						vector<SegmentType>::iterator startIt,
						vector<SegmentType>::iterator endIt,
						vector<SegmentType>::iterator &bestSegmentIt ) // "returned here"
{
	bestSegmentIt = endIt; 	// just in case, we'll check for this on the caller side
	Scalar minDist = 1e100;

	Point3Type end(endOfPreviousLineSegment2.x,endOfPreviousLineSegment2.y, 0);
	vector<SegmentType>::iterator it = startIt;
	while(it != endIt)
	{
		Point3Type start(it->a.x, it->a.y, 0);
		Point3Type v = end-start;
		Scalar distance = v.squaredMagnitude();
		if (distance < minDist)
		{
			minDist = distance;
			bestSegmentIt = it;
			// we could decide to stop here ... if we had a threshold
		}
		it ++;
	}
	return minDist;
}



void mgl::loopsAndHoleOgy(std::vector<SegmentType> &segments,
		Scalar tol,
		std::vector< std::vector<SegmentType> > &loops)
{
	// Lets sort this mess out so we can extrude in a continuous line of shiny contour
	// Nota: from their normals (int their previous life as 3d triangles), LineSegment2 know their beginings from their endings

	std::vector<Scalar> distances;
	distances.reserve(segments.size());

	distances.push_back(0); // this value is not used, it represents the distance between the
							// first LineSegment2 and the one before (and there is no LineSegment2 before)
	for(vector<SegmentType>::iterator i = segments.begin(); i != segments.end(); i++)
	{
		PointType &startingPoint = i->b;
		vector<SegmentType>::iterator startIt = i+1;
		vector<SegmentType>::iterator bestSegmentIt;
		if(startIt != segments.end())
		{
			Scalar distance = findClosestLineSegment2(startingPoint, startIt, segments.end(), bestSegmentIt);
			if(bestSegmentIt != segments.end())
			{
				// Swap the segments, because the best is the closest segment to the current one
				swap(*startIt, *bestSegmentIt);
				distances.push_back(distance);

			}
		}
	}

	// we now have an optimal sequence of LineSegment2s (except we didn't optimise for interloop traversal).
	// we also have a hop (distances) between each LineSegment2 pair


	vector<Scalar>::iterator hopIt = distances.begin();
	vector<SegmentType>::iterator i = segments.begin();
	while(i != segments.end())
	{
		// lets make a loop... we'll call it loop
		loops.push_back(vector<SegmentType>());
		vector<SegmentType> &loop = loops[loops.size()-1];

		loop.push_back(*i);

		++i;
		++hopIt;
		if(i == segments.end())
		{
			// this is sad ... a loop with a single segment
			#ifdef STRONG_CHECKING
			assert(0);
			#endif
			break;
		}
		bool thisLoopIsDone = false;
		while(!thisLoopIsDone)
		{
			Scalar distance = *hopIt;
			if(distance < tol)
			{
				loop.push_back(*i);
				++i;
				++hopIt;
			}
			else
			{
				thisLoopIsDone = true;
			}
			if(i == segments.end())
			{
				thisLoopIsDone = true;
			}
		}
	}

	// sanity check
    for(unsigned int i=0; i < loops.size(); ++i)
    {
    	const std::vector<SegmentType > &loop = loops[i];
    	if (loop.size() < 2)
    	{
            Log::info() << "WARNING: loop " << i << " segment count: " << loop.size() << endl;
    	}
    }
}

void mgl::translateLoops(LoopList &loops, PointType p) {
	for (LoopList::iterator loop = loops.begin();
		 loop != loops.end(); ++loop) {
		Loop::cw_iterator begin = loop->clockwise();
		begin->setPoint(begin->getPoint() + p);

		Loop::cw_iterator point = begin;
		
		for (++point; point != begin; ++point) {
			point->setPoint(point->getPoint() + p);
		}
	}
}

void mgl::translateOpenPaths(OpenPathList &paths, PointType p) {
	for (OpenPathList::iterator path = paths.begin();
		 path != paths.end(); ++path) {
		for (OpenPath::iterator point = path->fromStart();
			 point != path->end(); ++point) {
			*point += p;
		}
	}
}

void mgl::translateLoops(SegmentTable &loops, PointType p)
{
	for(unsigned int i=0; i < loops.size(); ++i)
	{
		translateSegments(loops[i],p);
	}
}

void mgl::translatePolygons(Polygons &polygons, PointType p)
{
	for(size_t i=0; i < polygons.size(); ++i)
	{
		translatePolygon(polygons[i],p);
	}
}

void mgl::translatePolygon(Polygon &polygon, PointType p)
{
	for(size_t i=0; i < polygon.size(); ++i)
	{
		polygon[i] += p;
	}
}

void mgl::translateSegments(std::vector<SegmentType> &segments, PointType p)
{
	for(size_t i=0; i < segments.size(); i++)
	{
		segments[i].a += p;
		segments[i].b += p;
	}
}

