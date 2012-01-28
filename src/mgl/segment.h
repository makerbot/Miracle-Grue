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

#include <cassert>
#include <vector>
#include <fstream>


#include "core.h"

//
// Exception class! All your errors are belong to us.
//



namespace mgl
{


// Segmentology turns triangles into lines
void segmentology(const std::vector<Triangle3> &allTriangles,
					const TriangleIndices &trianglesForSlice,
					Scalar z,
					Scalar tol,
					std::vector< std::vector<LineSegment2> > &loops);

// Assembles lines segments into loops (perimeter loops and holes)
void loopsAndHoles(std::vector<LineSegment2> &segments,
					Scalar tol,
					std::vector< std::vector<LineSegment2> > &loops);

// I have no idea what this does
void translateLoops(std::vector<std::vector<mgl::LineSegment2> > &loops, Vector2 p);

// moves Segments by a position
void translateSegments(std::vector<mgl::LineSegment2> &segments, Vector2 p);

Vector2 rotate2d(const Vector2 &p, Scalar angle);

void rotateLoops(std::vector<std::vector<mgl::LineSegment2> > &loops, Scalar angle);

void rotateSegments(std::vector<mgl::LineSegment2> &segments, Scalar angle);

// More important than meets the eyes: given 2 segments, where (if any) is the intersection?
bool segmentSegmentIntersection(const LineSegment2 &a, const LineSegment2 &b, Vector2 &p);
bool segmentSegmentIntersection(Scalar p0_x, Scalar p0_y, Scalar p1_x, Scalar p1_y,
		Scalar p2_x, Scalar p2_y, Scalar p3_x, Scalar p3_y, Scalar &i_x, Scalar &i_y);


}

#endif

