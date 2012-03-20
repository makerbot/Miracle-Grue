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


#include "core.h"

//
// Exception class! All your errors are belong to us.
//



namespace mgl
{

// turns triangles into lines
void segmentationOfTriangles(const TriangleIndices &trianglesForSlice,
		const std::vector<Triangle3> &allTriangles,
		Scalar z,
		std::vector<LineSegment2d> &segments);

// Assembles lines segments into loops (perimeter loops and holes)
void loopsAndHoleOgy(std::vector<LineSegment2d> &segments,
					Scalar tol,
					std::vector< std::vector<LineSegment2d> > &loops);

// I have no idea what this does, (2D translation?)
void translateLoops(SegmentTable &loops, Vector2 p);

// moves Segments by a position
void translateSegments(std::vector<LineSegment2d> &segments, Vector2 p);

void translatePolygon(Polygon &polygon, Vector2 p);
void translatePolygons(Polygons &polygons, Vector2 p);

void rotatePolygon(Polygon& polygon, Scalar angle);
void rotatePolygons(Polygons& polygons, Scalar angle);

void rotateLoops(std::vector<std::vector<LineSegment2d> > &loops, Scalar angle);
void rotateSegments(std::vector<LineSegment2d> &segments, Scalar angle);


// More important than meets the eyes: given 2 segments, where (if any) is the intersection?
bool segmentSegmentIntersection(const LineSegment2d &a, const LineSegment2d &b, Vector2 &p);
bool segmentSegmentIntersection(Scalar p0_x, Scalar p0_y, Scalar p1_x, Scalar p1_y,
		Scalar p2_x, Scalar p2_y, Scalar p3_x, Scalar p3_y, Scalar &i_x, Scalar &i_y);

bool sliceTriangle(const Vector3& vertex1,
					 const Vector3& vertex2,
						const Vector3& vertex3,
						   Scalar Z,
						   Vector3 &a,
						   	   Vector3 &b);
}

#endif

