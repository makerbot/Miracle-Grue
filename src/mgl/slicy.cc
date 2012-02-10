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

Slicy::Slicy(const Meshy &mesh,
			double layerW,
			const char* scadFile)
		:tol(1e-6),
		 layerW(layerW),
		 scadFile(scadFile),
		 sliceTable(mesh.readSliceTable()),
		 allTriangles(mesh.readAllTriangles()),
		 limits(mesh.readLimits()),
		 zTapeMeasure(mesh.readLayerMeasure()),
		 progress(mesh.readSliceTable().size())
{
	layerH = mesh.readLayerMeasure().getLayerH();
	size_t sliceCount = sliceTable.size();

	if(scadFile != NULL)
	{
		outlineScad.open(scadFile);
		outlineScad.writePathViz(layerH, layerW, sliceCount);
	}

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

			outlineScad.writeTrianglesModule("tri_", allTriangles, trianglesForSlice, sliceId);
			outlineScad.writePolygons("outlines_", "color([0,0,1,1])outline", loopsPolys, zz, sliceId);
			outlineScad.writePolygons("infills_",   "color([1,0,0,1])infill" , infillsPolys, zz, sliceId);

			unsigned int insetCount = insetsPolys.size();
			for(unsigned int insetId=0; insetId <  insetCount; insetId++)
			{
				const Polygons &polygons = insetsPolys[insetId];
				stringstream ss;
				cout << "slice " << sliceId << " outline "<< insetId <<  " module: " << endl;
				ss << "inset_" << sliceId << "_";
				outlineScad.writePolygons(ss.str().c_str(), "color([0,1,0,1])infill",  polygons, zz, sliceId);

				cout << ss.str().c_str() << sliceId << endl;
			}
			// one function that calls all insets

			unsigned int maxNumberOfOutlines = insetsPolys.size() -1;
			if(maxNumberOfOutlines > 0)
			{
				stringstream ss;
				ss << "insets_" << sliceId;
				string insetsForSlice = ss.str();
				ss << "_";
				string insetsForSliceForLoop = ss.str();
				outlineScad.writeMinMax(insetsForSlice.c_str(), insetsForSliceForLoop.c_str(), maxNumberOfOutlines);
				// cout << "	</scad>" << endl;
			}
		}
}

void Slicy::slice(  unsigned int sliceId,
					unsigned int extruderId,
					Scalar tubeSpacing,
					Scalar sliceAngle,
					unsigned int nbOfShells,
					Scalar infillShrinking,
					Scalar insetDistanceFactor,
					std::vector<SliceData> & slices)
{
    //cout << "<sliceId "  << sliceId << "/" <<  sliceCount << ">" << endl;
    const TriangleIndices & trianglesForSlice = sliceTable[sliceId];
    progress.tick();
    Scalar z = zTapeMeasure.sliceIndexToHeight(sliceId);



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
    	cout << "WARNING: Layer " << sliceId << " has no outline!" << endl;
    	return;
    }


	slices.push_back( SliceData(z,sliceId));
	SliceData &slice = slices[sliceId];
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

		//	dumpInsets(insetsForLoops);
		// create a vector of polygons for each shell.
		// for (unsigned int shellId=0; shellId < shells; shellId++)
		for(unsigned int outlineId=0; outlineId <  insetsForLoops.size(); outlineId++)
		{
			// cout << "inset oultline processing: " << outlineId << " of " << outlinesSegments.size() << endl;
			// cout << "inset size " << slice.extruderSlices[extruderId].insets.size() << endl;
			slice.extruderSlices[extruderId].insets.push_back(Polygons());
			Polygons &polygons = *slice.extruderSlices[extruderId].insets.rbegin();
			// contains all the insets for a single loop
			SegmentTable &insetTable = insetsForLoops[outlineId];
			createPolysFromloopSegments(insetTable , polygons );
		}
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
					slices[sliceId].z,
					sliceId);
	// cout << "</sliceId"  << sliceId <<  ">" << endl;

}

void Slicy::sliceAndPath(double tubeSpacing,
							double angle,
							unsigned int nbOfShells,
							Scalar infillShrinking,
							Scalar insetDistanceFactor,
							std::vector< SliceData >  &slices)
{
	size_t sliceCount = sliceTable.size();

	unsigned int extruderId = 0;
	assert(slices.size() == 0);
	slices.reserve(sliceCount);

	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		Scalar sliceAngle = sliceId * angle;
	    slice(sliceId, extruderId, tubeSpacing, sliceAngle, nbOfShells, infillShrinking, insetDistanceFactor, slices);
	}
	cout << "Done with slicing" << endl;

}

Slicy::~Slicy()
{

	size_t sliceCount = sliceTable.size();
	// finalize the scad file
	if(scadFile != NULL)
	{
		outlineScad.writeMinMax("outlines", "outlines_", sliceCount);
		outlineScad.writeMinMax("triangles", "tri_", sliceCount);
		outlineScad.writeMinMax("infills", "infills_", sliceCount);
		outlineScad.writeMinMax("insets", "insets_", sliceCount);
		outlineScad.close();
	}
	std::cout << "Scad file " << outlineScad.getScadFileName() << std::endl;
}

