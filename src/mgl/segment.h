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

#include "core.h"

//
// Exception class! All your errors are belong to us.
//



namespace mgl
{







void segmentology(const std::vector<BGL::Triangle3d> &allTriangles, const TriangleIndices &trianglesForSlice, double z, std::vector<Segment> &segments);
void translateSegments(std::vector<mgl::Segment> &segments, BGL::Point p);
BGL::Point rotate2d(const BGL::Point &p, Scalar angle);
void rotateSegments(std::vector<mgl::Segment> &segments, Scalar angle);
bool segmentSegmentIntersection(Scalar p0_x, Scalar p0_y, Scalar p1_x, Scalar p1_y,
		Scalar p2_x, Scalar p2_y, Scalar p3_x, Scalar p3_y, Scalar &i_x, Scalar &i_y);






}

#endif
