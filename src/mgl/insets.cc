/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#include "insets.h"
#include "shrinky.h"


using namespace std;



// a) takes in a segment table (i.e a series of loops, clockwise segments for perimeters,
// and counter clockwise for holes)
// b) creates nbOfShells insets for each
// c) stores them in insetsForLoops (a list of segment tables: one table per loop,
// and nbOffShels insets)
//
void mgl::inshelligence( const SegmentTable & outlinesSegments,
					unsigned int nbOfShells,
					double layerW,
					unsigned int sliceId,
					Scalar insetDistanceFactor,
					const char *scadFile,
					bool writeDebugScadFiles,
					std::vector<SegmentTable> &insetsForLoops)
{
	assert(insetsForLoops.size() ==0);
	std::vector<Scalar> insetDistances;
	insetDistances.reserve(nbOfShells);
	for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
	{
		Scalar insetDistance = shellId ==0? insetDistance = 0.5*layerW: insetDistanceFactor *layerW;
		insetDistances.push_back(insetDistance);
	}

//	if(!useClipper)
	{
		shrinkyShells(	outlinesSegments,
					insetDistances,
					sliceId,
					scadFile,
					writeDebugScadFiles,
					insetsForLoops);
	}
//	else
	{
	/*
		for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
		{
			cout << "CLIPPER" << endl;
			ClipperLib::Polygons in_polys;
			if(shellId==0)
			{
				mglToClipper(outlinesSegments, in_polys);
			}
			else
			{
				SegmentTable &table = insetsForLoops[shellId - 1];
				mglToClipper(table, in_polys);
			}

			ClipperLib::Polygons out_polys;
			double delta = insetDistances[shellId];
			ClipperLib::JoinType jointype = ClipperLib::jtMiter;
			double miterLimit = 3.0;

			OffsetPolygons(in_polys, out_polys, delta, jointype, miterLimit);

			insetsForLoops.push_back(mgl::SegmentTable());
			SegmentTable& insetSegments = *insetsForLoops.rbegin();
			clipperToMgl(out_polys, insetSegments);
		}
		// void OffsetPolygons(const Polygons &in_polys, Polygons &out_polys, double delta, JoinType jointype, double MiterLimit)
	*/

	}
}
