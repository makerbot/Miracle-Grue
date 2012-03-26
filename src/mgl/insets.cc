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
#include "clipper/clipper.h"

using namespace std;
using namespace mgl;

#define DBLTOINT 1000


void  clipperToMgl(const ClipperLib::Polygons &polys, mgl::SegmentTable & outlinesSegments)
{
	size_t loopCount = polys.size();
	for(size_t i=0; i < loopCount; i++)
	{
		const ClipperLib::Polygon &loop = polys[i];
		outlinesSegments.push_back(std::vector<mgl::LineSegment2 > ());
		std::vector<mgl::LineSegment2 > &segments = *outlinesSegments.rbegin();
		for(size_t j=0; j < loop.size(); j++)
		{
			size_t next = j==loop.size()-1?0:j+1;
			const ClipperLib::IntPoint &point = loop[j];
			const ClipperLib::IntPoint &nextPoint = loop[next];

			mgl::LineSegment2 s;
			s.a[0] = point.X / (Scalar)DBLTOINT;
			s.a[1] = point.Y / (Scalar)DBLTOINT;
			s.b[0] = nextPoint.X / (Scalar)DBLTOINT;
			s.b[1] = nextPoint.Y / (Scalar)DBLTOINT;
			segments.push_back(s);
		}
	}
}

void  mglToClipper(const mgl::SegmentTable &segmentTable, ClipperLib::Polygons &out_polys )
{
	for(size_t i=0; i < segmentTable.size(); i++)
	{
		out_polys.push_back(vector<ClipperLib::IntPoint>());
		vector<ClipperLib::IntPoint>& poly = *out_polys.rbegin();

		const vector<mgl::LineSegment2> &loop = segmentTable[i];
		for(size_t j=0; j < loop.size(); j++)
		{
			size_t reverseIndex = loop.size()-1 -j;
			const mgl::LineSegment2 &seg = loop[reverseIndex];
			ClipperLib::IntPoint p;
			p.X = seg.a[0] * DBLTOINT;
			p.Y = seg.a[1] * DBLTOINT;
			poly.push_back(p);
		}
	}
}

void  dumpSegmentTable(const char* name, const SegmentTable & outTable)
{
    for(int i = 0;i < outTable.size();i++){
        const vector<LineSegment2> & segs = outTable[i];
        stringstream ss;
        ss << name << "_" << i;
        ScadTubeFile::segment3(cout, "", ss.str().c_str(), segs, 0, 0);
    }
}

void  dumpClipperPolys(const char*name, const ClipperLib::Polygons  &polys)
{
	for(size_t i=0; i < polys.size(); i++)
	{
		const ClipperLib::Polygon &poly = polys[i];
		cout <<  name <<"_" << i << "= [";
		for(size_t j=0; j < poly.size(); j++)
		{
			const ClipperLib::IntPoint &p = poly[j];
			cout << "[" << p.X << ", "<< p.Y << "]," << endl;
		}
		cout<< "];" << endl;
	}
}


class ClipperInsetter
{

public:
	ClipperInsetter(){}
	virtual ~ClipperInsetter(){}
	void inset( const mgl::SegmentTable & inputPolys,
				Scalar insetDist,
				mgl::SegmentTable & outputPolys);
};

void ClipperInsetter::inset( const mgl::SegmentTable &inputPolys,
							 Scalar insetDist,
							 mgl::SegmentTable& outputPolys)
{

	ClipperLib::Polygons in_polys, out_polys;
	ClipperLib::JoinType jointype = ClipperLib::jtMiter;
	double miterLimit = 3.0;

	double delta = -insetDist * DBLTOINT;

	mglToClipper  (inputPolys, in_polys);
	//dumpClipperPolys(in_polys);
	ClipperLib::OffsetPolygons(in_polys, out_polys, delta, jointype, miterLimit);
	//dumpClipperPolys(out_polys);
	clipperToMgl(out_polys, outputPolys);
}

bool useShrinky = true;
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

	if(useShrinky)
	{
		createShellsForSliceUsingShrinky(	outlinesSegments,
											insetDistances,
											sliceId,
											scadFile,
											writeDebugScadFiles,
											insetsForLoops);
	}
	else
	{
		ClipperInsetter insetter;
		for(size_t i=0; i < insetDistances.size(); i++)
		{
			outlinesSegments:insetsForLoops[i-1];
			Scalar dist = insetDistances[i];
			insetsForLoops.push_back(SegmentTable());
			SegmentTable & outputs = *insetsForLoops.rbegin();
			if(i==0)
			{
				insetter.inset(outlinesSegments, dist, outputs);
			}
			else
			{
				insetter.inset(insetsForLoops[i-1], dist, outputs);
			}
		}

	}
}
