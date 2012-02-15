/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include <stdint.h>
#include <cstring>

#include "slicy.h"


using namespace mgl;
using namespace std;



// segments are OK, but polys are better for paths (no repeat point)
void segments2polygon(const std::vector<TriangleSegment2> & segments, Polygon &loop)
{

    loop.reserve(segments.size());
    for(int j = 0;j < segments.size();j++){
        const TriangleSegment2 & line = segments[j];
        Vector2 p(line.a);
        loop.push_back(p);
        if(j == segments.size() - 1){
            Vector2 p(line.b);
            loop.push_back(p);
        }
    }

}

//
// Converts vectors of segments into polygons.
// The ordering is reversed... the last vector of segments is the first polygon
// This function fills a a list of poygon (table of Vector2) from a table of segments
void createPolysFromloopSegments(const SegmentTable &segmentTable,
										Polygons& loops)
{
	// outline loops
	unsigned int count = segmentTable.size();
	for(int i=0; i < count; i++)
	{
		const std::vector<TriangleSegment2> &segments = segmentTable[count-1 - i];
		loops.push_back(Polygon());
		Polygon &loop = loops[loops.size()-1];
	    segments2polygon(segments, loop);
	}
}


void inshelligence( const SegmentTable & outlinesSegments,
					unsigned int nbOfShells,
					double layerW,
					unsigned int sliceId,
					Scalar insetDistanceFactor,
					const char *scadFile,
					std::vector<SegmentTable> &insetsForLoops)
{

	assert(insetsForLoops.size() ==0);
	//
	// dbgs__( "outlineSegmentCount " << outlineSegmentCount)
    for(unsigned int outlineId=0; outlineId < outlinesSegments.size(); outlineId++)
	{
    	const std::vector<TriangleSegment2> &outlineLoop = outlinesSegments[outlineId];
    	assert(outlineLoop.size() > 0);

		// dbgs__("outline " << outlineId << "/" <<  outlinesSegments.size())
		// prepare  a new vector of loops for insets of this outline
		insetsForLoops.push_back(SegmentTable());
		assert(insetsForLoops.size() == outlineId + 1);

		SegmentTable &insetTable = *insetsForLoops.rbegin(); // inset curves for a single loop
		insetTable.reserve(nbOfShells);

		MyComputer myComputer;


		unsigned int shellId = 0;
		Shrinky shrinky; // loopScadFile.c_str());

		unsigned int segmentCountBefore =0;
		unsigned int segmentCountAfter =0;

		vector<Scalar> insetDistances;
		vector<Scalar> layerWidths;

		insetDistances.reserve(nbOfShells);
		layerWidths.reserve(nbOfShells);

		for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
		{
			insetTable.push_back(std::vector<TriangleSegment2>());
			Scalar insetDistance = shellId ==0? insetDistance = 0.5*layerW: insetDistanceFactor *layerW;
			insetDistances.push_back(insetDistance);
			layerWidths.push_back(layerW);
		}
		try
		{
			vector<TriangleSegment2> tmp = outlineLoop;
			vector<TriangleSegment2> &previousInsets  = tmp;
			for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
			{
				Scalar insetDistance = insetDistances[shellId];
				std::vector<TriangleSegment2> &insets = insetTable[shellId];
				shrinky.inset(previousInsets, insetDistance, insets);
				previousInsets = insets;
			}
		}
		catch(ShrinkyMess &messup)
		{
			cout << "sliceId: " <<  sliceId   << endl;
			cout << "loopId : " <<  outlineId << endl;
			cout << "shellId: " <<  shellId   << endl;

			stringstream ss;
			ss << "_slice_" << sliceId << "_loop_" << outlineId << ".scad";
			string loopScadFile = myComputer.fileSystem.ChangeExtension(scadFile, ss.str());
			Shrinky shriker(loopScadFile.c_str());
			try
			{
				vector<TriangleSegment2> &previousInsets  = const_cast<vector<TriangleSegment2> &> (outlineLoop);
				std::vector<TriangleSegment2> insets;
				for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
				{
					Scalar insetDistance = insetDistances[shellId];
					shrinky.inset(previousInsets, insetDistance, insets);
					previousInsets = insets;
					insets.clear();
				}
			}
			catch(ShrinkyMess &messup)
			{
				cout << "saving to file " << ss.str() << endl;
			}
		}
	}
}


Slicy::Slicy(const std::vector<Triangle3> &allTriangles,
			const Limits& limits,
			Scalar layerW,
			Scalar layerH,
			unsigned int sliceCount,
			const char* scadFile)
		:tol(1e-6),
		 layerW(layerW),
		 layerH(layerH),
		 scadFile(scadFile),
		 allTriangles(allTriangles),
		 limits(limits),
		 progress(sliceCount),
		 sliceCount(sliceCount)
{


    openScadFile(scadFile, layerW, layerH, sliceCount);

	tubularLimits = limits.centeredLimits();
	tubularLimits.inflate(1.0, 1.0, 0.0);
	// make it square along z so that rotation happens inside the limits
	// hence, tubular limits around z
	tubularLimits.tubularZ();

	Vector3 c = limits.center();

	toRotationCenter[0] = -c[0];
	toRotationCenter[1] = -c[1];
	backToOrigin[0] = c[0];
	backToOrigin[1] = c[1];


	Vector3 rotationCenter = limits.center();


}

void Slicy::openScadFile(const char *scadFile, double layerW,Scalar layerH ,size_t sliceCount)
{
    if(scadFile != NULL)
	{
    	fscad.open(scadFile);

		std::ofstream &out = fscad.getOut();

	    out << "// use min and max to see individual layers " << std::endl;
	    out << "min = 0;" << std::endl;
	    out << "max = " << sliceCount << ";" << std::endl;
	    out << "// triangles(min,max);" << std::endl;
	    out << "outlines(min,max);" << std::endl;
	    out << "// infills(min,max);" << std::endl;
	    out << std::endl;
	    out << "stl_color = [0,1,0, 0.025];" << std::endl;

		out << "module out_line(x1, y1, z1, x2, y2, z2)" << std::endl;
		out << "{" << std::endl;
		out << "    tube(x1, y1, z1, x2, y2, z2, diameter1=0.4, diameter2=0, faces=4, thickness_over_width=1);" << std::endl;
		out << "}" << std::endl;

		out  << std::endl;
		out << "module extrusion(x1, y1, z1, x2, y2, z2)" << std::endl;
		out << "{" << std::endl;
		out << "    d = " << layerH << ";" << std::endl;
		out << "    f = 6;" << std::endl;
		out << "    t =  "  << layerH / layerW << ";"<< std::endl;
		out << "    corner(x1,y1,z1, diameter=d, faces=f, thickness_over_width =t );" << std::endl;
		out << "    tube(x1, y1, z1, x2, y2, z2, diameter1=d, diameter2=d, faces=f, thickness_over_width=t);" << std::endl;
		out << "}" << std::endl;

		out << std::endl;
		out << "module outline(points, paths)" << std::endl;
		out << "{" << std::endl;
		out << "    for (p= paths)" << std::endl;
		out << "    {" << std::endl;
		out << "       out_line(points[p[0]][0],points[p[0]][1],points[p[0]][2],points[p[1]][0],points[p[1]][1],points[p[1]][2] );" << std::endl;
		out << "    }" << std::endl;
		out << "}" << std::endl;
		out << std::endl;

		out << std::endl;
		out << "module infill(points, paths)" << std::endl;
		out << "{" << std::endl;
		out << "     for (p= paths)" << std::endl;
		out << "    {" << std::endl;
		out << "        extrusion(points[p[0]][0],points[p[0]][1],points[p[0]][2], points[p[1]][0],points[p[1]][1],points[p[1]][2] );" << std::endl;
		out << "    }" << std::endl;
		out << "}" << std::endl;
		out << std::endl;

	    out << std::endl;
	    out << "module outline_segments(segments)" << std::endl;
	    out << "{" << std::endl;
	    out << "    for(seg = segments)" << std::endl;
	    out << "    {" << std::endl;
	    out << "        out_line(seg[0][0], seg[0][1], seg[0][2], seg[1][0], seg[1][1], seg[1][2]);" << std::endl;
	    out << "    }" << std::endl;
	    out << "}" << std::endl;
	    out << std::endl;

	    out << "module infill_segments(segments)" << std::endl;
	    out << "{" << std::endl;
	    out << "    for(seg = segments)" << std::endl;
	    out << "    {" << std::endl;
	    out << "        extrusion(seg[0][0], seg[0][1], seg[0][2], seg[1][0], seg[1][1], seg[1][2]);" << std::endl;
	    out << "   }" << std::endl;
	    out << "}" << std::endl;

	    fscad.writeHeader();
	}
}


void Slicy::writeScadSlice(const TriangleIndices & trianglesForSlice,
							const Polygons & loopsPolys,
							const Polygons & infillsPolys,
							const vector<Polygons> & insetsPolys,
							Scalar zz,
							unsigned int sliceId)
{
    if(scadFile != NULL)
		{
			#ifdef OMPFF
			OmpGuard lock (my_lock);
			cout << "slice "<< sliceId << "/" << sliceCount << " thread: " << "thread id " << omp_get_thread_num() << " (pool size: " << omp_get_num_threads() << ")"<< endl;
			#endif

			fscad.writeTrianglesModule("tri_", allTriangles, trianglesForSlice, sliceId);
			fscad.writePolygons("outlines_", "color([0,0,1,1])outline", loopsPolys, zz, sliceId);
			fscad.writePolygons("infills_",   "color([1,0,0,1])infill" , infillsPolys, zz, sliceId);


			unsigned int insetCount = insetsPolys.size();
			for(unsigned int shellId=0; shellId <  insetCount; shellId++)
			{
				const Polygons &polygons = insetsPolys[shellId];
				stringstream ss;
				ss << "insets_" << sliceId << "_";

				fscad.writePolygons(ss.str().c_str(), "color([0,1,0,1])infill",  polygons, zz, shellId);
				cout << ss.str().c_str() << endl;
				cout <<"  NB of polygons: "<< polygons.size()<<endl;
				for (int i=0; i < polygons.size(); i++)
				{
					cout << "     " << polygons[i].size() << " points" << endl;
				}
			}
			// one function that calls all insets


			if(insetCount > 0)
			{
				stringstream ss;
				ss << "insets_" << sliceId;
				string insetsForSlice = ss.str();
				ss << "_";
				fscad.writeMinMax(insetsForSlice.c_str(), ss.str().c_str(), insetCount);
				//cout << " SCAD: " << insetsForSlice.c_str() << endl;
			}
		}
}

void Slicy::closeScadFile()
{
    // finalize the scad file
    if(scadFile != NULL)
	{
		fscad.writeMinMax("outlines", "outlines_", sliceCount);
		fscad.writeMinMax("triangles", "tri_", sliceCount);
		fscad.writeMinMax("infills", "infills_", sliceCount);
		fscad.writeMinMax("insets", "insets_", sliceCount);
		fscad.close();
	}
    std::cout << "closed OpenSCad file: " << fscad.getScadFileName() << std::endl;
}

bool Slicy::slice(  const TriangleIndices & trianglesForSlice,
					Scalar z,
					unsigned int sliceId,
					unsigned int extruderId,
					Scalar tubeSpacing,
					Scalar sliceAngle,
					unsigned int nbOfShells,
					Scalar infillShrinking,
					Scalar insetDistanceFactor,
					SliceData &slice)
{
    //cout << "<sliceId "  << sliceId << "/" <<  sliceCount << ">" << endl;

    progress.tick();


    // segmentology(allTriangles, trianglesForSlice, z, tol, outlinesSegments);
    std::vector<TriangleSegment2> segments;
    segmentationOfTriangles(trianglesForSlice, allTriangles, z, segments);
	// what we are left with is a series of segments (outline segments... triangle has beens)

    // get the "real" 2D paths for outline
	// lets order the segment into loops.
	SegmentTable outlinesSegments;
    loopsAndHoleOgy(segments, tol, outlinesSegments);

    // keep all segments of insets for each loop
    unsigned int outlineSegmentCount = outlinesSegments.size();
    if(outlineSegmentCount == 0)
    {

    	return false;
    }


	slice.extruderSlices.push_back(ExtruderSlice());

	std::vector<SegmentTable> insetsForLoops;

	if(nbOfShells > 0)
	{
		// create shells inside the outlines (and around holes)
		inshelligence(outlinesSegments,
						  nbOfShells,
						  layerW,
						  sliceId,
						  insetDistanceFactor,
						  scadFile,
						  insetsForLoops);

		assert(insetsForLoops.size() == outlineSegmentCount);

		//	dumpInsets(insetsForLoops)
		// create a vector of polygons for each shell.
		std::vector<Polygons> &insetsPolys = slice.extruderSlices[extruderId].insets;

		// we fill the structure "backwards" so that the inner shells come first
		for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
		{
			insetsPolys.push_back(Polygons());
		}
		unsigned int loopCount = insetsForLoops.size();
		for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
		{
			Polygons &polygons = insetsPolys[shellId];
			for(unsigned int outlineId=0; outlineId <  loopCount; outlineId++)
			{
				polygons.push_back(Polygon());
				Polygon &polygon = *polygons.rbegin();
				const std::vector<TriangleSegment2>& segmentLoop = insetsForLoops[outlineId][nbOfShells -1 - shellId];
				segments2polygon(segmentLoop, polygon);
			}
		}



//		for(unsigned int outlineId=0; outlineId <  insetsForLoops.size(); outlineId++)
//		{
//			// cout << "inset oultline processing: " << outlineId << " of " << outlinesSegments.size() << endl;
//			// cout << "inset size " << slice.extruderSlices[extruderId].insets.size() << endl;
//			slice.extruderSlices[extruderId].insets.push_back(Polygons());
//			Polygons &polygons = *slice.extruderSlices[extruderId].insets.rbegin();
//			// contains all the insets for a single loop
//			SegmentTable &insetTable = insetsForLoops[outlineId];
//			createPolysFromloopSegments(insetTable , polygons );
//		}
	}
	//cout << " ** " << outlinesSegments.size() << " ** " <<  insetsForLoops.size() << endl;



	// deep copy the the infill boundaries
	SegmentTable rotatedSegments = outlinesSegments; // insetsForLoops[0];

	//dbgs__("insetforloops size " << insetsForLoops.size() )
	// deep copy the smallest insets as the infill boundaries

	if(insetsForLoops.size() > 0)
	{
		rotatedSegments= outlinesSegments;
	}

	translateLoops(rotatedSegments, toRotationCenter);
	// rotate the outlines before generating the tubes...
	rotateLoops(rotatedSegments, sliceAngle);

	//cout << "<Pathology nb of loops ";
	//cout << rotatedSegments.size() << ">" << endl;

	Polygons& infills = slice.extruderSlices[extruderId].infills;
	infillPathology(rotatedSegments,
					tubularLimits,
					slice.z,
					tubeSpacing,
					infillShrinking,
					infills);
	// rotate and translate the TUBES so they fit with the ORIGINAL outlines
	rotatePolygons(infills, -sliceAngle);
	translatePolygons(infills, backToOrigin);

	//	cout << "</Pathology>" << endl;

	createPolysFromloopSegments(outlinesSegments, slice.extruderSlices[extruderId].loops);

	// write the scad file
	// only one thread at a time in here

	writeScadSlice( trianglesForSlice,
					slice.extruderSlices[extruderId].loops,
					slice.extruderSlices[extruderId].infills,
					slice.extruderSlices[extruderId].insets,
					z,
					sliceId);
	// cout << "</sliceId"  << sliceId <<  ">" << endl;
	return true;
}

//void Slicy::sliceAndPath(   double tubeSpacing,
//							double angle,
//							unsigned int nbOfShells,
//							Scalar infillShrinking,
//							Scalar insetDistanceFactor,
//							std::vector< SliceData >  &slices)
//{
//	size_t sliceCount = sliceTable.size();
//
//	unsigned int extruderId = 0;
//	assert(slices.size() == 0);
//	slices.reserve(sliceCount);
//
//	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
//	{
//		Scalar sliceAngle = sliceId * angle;
//	    slice(sliceId, extruderId, tubeSpacing, sliceAngle, nbOfShells, infillShrinking, insetDistanceFactor, slices);
//	}
//	cout << "Done with slicing" << endl;
//
//}



Slicy::~Slicy()
{
    closeScadFile();
}

