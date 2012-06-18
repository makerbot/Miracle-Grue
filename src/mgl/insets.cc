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
#include "clipper.h"
#include "log.h"

using namespace std;
using namespace mgl;
using namespace libthing;


static const Scalar DBLTOINT = 1000;

void mgl::polygonsFromLoopSegmentTables( unsigned int nbOfShells,
									const Insets & insetsForLoops,
									std::vector<Polygons> & insetsPolys )
{
    // assert(insetsForLoops.size() == outlineSegmentCount);
    // we fill the structure "backwards" so that the inner shells come first
    for(unsigned int shellId = 0;shellId < nbOfShells;shellId++){
        insetsPolys.push_back(Polygons());
    }
    unsigned int shellCount = insetsForLoops.size();
    for(unsigned int shellId = 0;shellId < shellCount;shellId++){
        const SegmentTable & loopsForCurrentShell = insetsForLoops[shellId];
        unsigned int loopCount = loopsForCurrentShell.size();
        Polygons & polygons = insetsPolys[shellId];
        for(unsigned int outlineId = 0;outlineId < loopCount;outlineId++){
            const std::vector<LineSegment2> & segments = loopsForCurrentShell[outlineId];
            if(segments.size() > 2){
                polygons.push_back(Polygon());
                Polygon & polygon = *polygons.rbegin();
                segments2polygon(segments, polygon);
            }
        }
    }
}


 void  clipperToMgl(const ClipperLib::Polygons &polys, SegmentVector & outlinesSegments)
{
	size_t loopCount = polys.size();
	for(size_t i=0; i < loopCount; i++)
	{
		const ClipperLib::Polygon &loop = polys[i];
		outlinesSegments.push_back(std::vector<LineSegment2 > ());
		std::vector<LineSegment2 > &segments = *outlinesSegments.rbegin();
		unsigned int loopCount = loop.size();
		segments.resize(loopCount);
		for(size_t j=0; j < loopCount; j++)
		{
			size_t next = j==loop.size()-1?0:j+1;
			const ClipperLib::IntPoint &point = loop[j];
			const ClipperLib::IntPoint &nextPoint = loop[next];

			LineSegment2 s;
			s.b[0] = point.X / (Scalar)DBLTOINT;
			s.b[1] = point.Y / (Scalar)DBLTOINT;
			s.a[0] = nextPoint.X / (Scalar)DBLTOINT;
			s.a[1] = nextPoint.Y / (Scalar)DBLTOINT;

			unsigned int reverseId = loopCount -1 -j;
			segments[reverseId] = s;
		}
	}
}


void  mglToClipper(const SegmentVector &segmentTable, ClipperLib::Polygons &out_polys )
{
	for(size_t i=0; i < segmentTable.size(); i++)
	{
		out_polys.push_back(vector<ClipperLib::IntPoint>());
		vector<ClipperLib::IntPoint>& poly = *out_polys.rbegin();

		const vector<LineSegment2> &loop = segmentTable[i];
		for(size_t j=0; j < loop.size(); j++)
		{
			size_t reverseIndex = loop.size()-1 -j;
			const LineSegment2 &seg = loop[reverseIndex];
			ClipperLib::IntPoint p;
            p.X = (UINT64)(seg.a[0] * DBLTOINT);
            p.Y = (UINT64)(seg.a[1] * DBLTOINT);
			poly.push_back(p);
		}
	}
}

void  dumpSegmentTable(const char* name, const SegmentTable & outTable)
{
    for(size_t i = 0;i < outTable.size();i++)
    {
        const vector<LineSegment2> & segs = outTable[i];
        stringstream ss;
        ss << name << "_" << i;
        ScadDebugFile::segment3(cout, "", ss.str().c_str(), segs, 0, 0);
    }
}

void  dumpClipperPolys(const char*name, const ClipperLib::Polygons  &polys)
{
	for(size_t i=0; i < polys.size(); i++)
	{
		const ClipperLib::Polygon &poly = polys[i];
        Log::info() <<  name <<"_" << i << "= [";
		for(size_t j=0; j < poly.size(); j++)
		{
			const ClipperLib::IntPoint &p = poly[j];
            Log::info() << "[" << p.X << ", "<< p.Y << "]," << endl;
		}
        Log::info() << "];" << endl;
	}
}



class ClipperInsetter {

public:
	ClipperInsetter(){}
	virtual ~ClipperInsetter(){}
	void inset( const SegmentVector & inputPolys,
				Scalar insetDist,
				SegmentVector & outputPolys);
};

void ClipperInsetter::inset( const SegmentVector &inputPolys,
							 Scalar insetDist,
							 SegmentVector& outputPolys)
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

/// a) takes in a segment table (i.e a series of loops, clockwise segments for perimeters,
// and counter clockwise for holes)
/// b) creates nbOfShells insets for each
/// c) stores them in insetsForLoops (a list of segment tables: one table per loop,
/// and nbOffShels insets)
///
/// @param inOutlineSegments : segment table used for shell section lookup.
/// @param nShells : number of shells to generate
/// @param layerW : width of extrusion of layer
///	@param insetDistanceFactor : TBD
/// @param scanFile : debug openScad file
/// @param writeDebugScadFiles : true/false if we want to write scad files
/// @param insetsForLoops : vector of TBD
void mgl::inshelligence( SegmentTable const& inOutlinesSegments,
		const unsigned int nShells,
		const double layerW,
		//unsigned int sliceId,
		Scalar insetDistanceFactor,
		const char *scadFile,
		bool /*writeDebugScadFiles*/,
		Insets &insetsForLoops) {
	
	static const bool USE_SHRINKY = false;
	
	std::vector<Scalar> insetDistances;
	insetDistances.reserve(nShells);
	for (unsigned int shellId=0; shellId < nShells; shellId++) {
		Scalar insetDistance;
		if(shellId ==0)
			insetDistance = 0.5*layerW;
		else
			insetDistance = insetDistanceFactor *layerW;
		insetDistances.push_back(insetDistance);
	}

	if(!USE_SHRINKY) {
		ClipperInsetter insetter;
		for(size_t i=0; i < insetDistances.size(); i++) {
			Scalar dist = insetDistances[i];
			insetsForLoops.push_back(SegmentTable());
			SegmentTable & outputs = *insetsForLoops.rbegin();
			if(i==0) {
				insetter.inset(inOutlinesSegments, dist, outputs);
			} else {
				SegmentTable &inputs = insetsForLoops[i-1];
				insetter.inset(inputs, dist, outputs);
			}
		}
		//innerOutlinesSegments = *insetsForLoops.rbegin();
	} else {
		createShellsForSliceUsingShrinky(	inOutlinesSegments,
				insetDistances,
				0,//sliceId,
				scadFile,
				insetsForLoops);

		unsigned int loopCount = inOutlinesSegments.size();
		for (unsigned int loop = 0; loop < loopCount; loop++) {
			// const std::vector<LineSegment2 > &deppestInset = *insetsForLoops[i].rbegin();

			int lastKnownShell = -1;
			for (unsigned int shellId=0; shellId < nShells; shellId++)
			{
				const SegmentTable &loopsForCurrentShell = insetsForLoops[shellId];
				const vector<LineSegment2> &segmentsForLoop = loopsForCurrentShell[loop];
				if(segmentsForLoop.size() > 2)
				{
					lastKnownShell = shellId;
				}
			}
                        if(lastKnownShell >= 0)
                        {
//				const vector<LineSegment2> &deppestInset = insetsForLoops[lastKnownShell][loop];
//				//innerOutlinesSegments.push_back(deppestInset);
                        }
//			else
//			{
//				const vector<LineSegment2> &deppestInset = outlinesSegments[loop];
//				//innerOutlinesSegments.push_back(deppestInset);
//			}
		}
	}
}
