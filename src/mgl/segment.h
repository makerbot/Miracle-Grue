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

#include "BGL/config.h"
#include "BGL/BGLTriangle3d.h"
#include "BGL/BGLMesh3d.h"

//
// Exception class! All your errors are belong to us.
//



namespace mgl
{

bool sameSame(double a, double b);


class Except
{
public:
	std::string error;
	Except(const char *msg)
	 :error(msg)
	{

	}
};


typedef unsigned int index_t;
typedef std::vector<index_t> TriangleIndices;
typedef std::vector<TriangleIndices> TrianglesInSlices;





struct Segment
{
	BGL::Point a;
	BGL::Point b;
};


bool sliceTriangle(const BGL::Point3d& vertex1,
					 const BGL::Point3d& vertex2,
						const BGL::Point3d& vertex3,
						   Scalar Z, BGL::Point &a,
						      BGL::Point &b);





void segmentology(const std::vector<BGL::Triangle3d> &allTriangles, const TriangleIndices &trianglesForSlice, double z, std::vector<Segment> &segments);
void translateSegments(std::vector<mgl::Segment> &segments, BGL::Point p);
BGL::Point rotate2d(const BGL::Point &p, Scalar angle);
void rotateSegments(std::vector<mgl::Segment> &segments, Scalar angle);
bool segmentSegmentIntersection(Scalar p0_x, Scalar p0_y, Scalar p1_x, Scalar p1_y,
		Scalar p2_x, Scalar p2_y, Scalar p3_x, Scalar p3_y, Scalar &i_x, Scalar &i_y);






}

#endif
