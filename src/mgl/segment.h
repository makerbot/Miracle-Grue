/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef SEGMENT_H_
#define SEGMENT_H_ (1)


#include "mgl.h"

//
// Exception class! All your errors are belong to us.
//


#include "loop_path.h"

namespace mgl
{

//
// Converts vectors of segments into polygons.
// The ordering is reversed... the last vector of segments is the first polygon
// This function fills a a list of poygon (table of Vector2) from a table of segments
void createPolysFromloopSegments(const libthing::SegmentVector &SegmentVector, Polygons& loops);

// segments are OK, but polys are better for paths (no repeat point)
void segments2polygon(const std::vector<SegmentType> & segments, mgl::Polygon &loop);

// turns triangles into lines
void segmentationOfTriangles(const TriangleIndices &trianglesForSlice,
		const std::vector<libthing::Triangle3> &allTriangles,
		Scalar z,
		std::vector<SegmentType> &segments);

// Assembles lines segments into loops (perimeter loops and holes)
void loopsAndHoleOgy(std::vector<SegmentType> &segments,
					Scalar tol,
					std::vector< std::vector<SegmentType> > &loops);

// 2D translation
void translateLoops(libthing::SegmentVector &loops, PointType p);

// moves Segments by a position
void translateSegments(std::vector<SegmentType> &segments, PointType p);

void translatePolygon(Polygon &polygon, PointType p);
void translatePolygons(Polygons &polygons, PointType p);

void rotateLoops(LoopList& loops, Scalar angle);
void translateLoops(LoopList& loops, PointType p);
void translateOpenPaths(OpenPathList &paths, PointType p);
void rotatePolygon(Polygon& polygon, Scalar angle);
void rotatePolygons(Polygons& polygons, Scalar angle);

void rotateLoops(std::vector<std::vector<SegmentType> > &loops, Scalar angle);
void rotateSegments(std::vector<SegmentType> &segments, Scalar angle);


// More important than meets the eyes: given 2 segments, where (if any) is the intersection?
bool segmentSegmentIntersection(const SegmentType &a, const SegmentType &b, PointType &p);
bool segmentSegmentIntersection(Scalar p0_x, Scalar p0_y, Scalar p1_x, Scalar p1_y,
		Scalar p2_x, Scalar p2_y, Scalar p3_x, Scalar p3_y, Scalar &i_x, Scalar &i_y);

bool sliceTriangle(const libthing::Vector3& vertex1,
		const libthing::Vector3& vertex2, const libthing::Vector3& vertex3,
		Scalar Z, libthing::Vector3 &a, libthing::Vector3 &b);
}

#endif

