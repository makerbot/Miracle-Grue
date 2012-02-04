/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "slicy.h"
#include "shrinky.h"
#include "scadtubefile.h"

#include <stdint.h>
#include <cstring>

using namespace mgl;
using namespace std;




void inshelligence( const SegmentTable & outlinesSegments,
					unsigned int nbOfShells,
					double layerW,
					unsigned int sliceId,
					const char *scadFile,
					std::vector<SegmentTable> &insetsForLoops)
{

	assert(insetsForLoops.size() ==0);
	// dbgs__( "outlineSegmentCount " << outlineSegmentCount)
    for(unsigned int outlineId=0; outlineId < outlinesSegments.size(); outlineId++)
	{
    	const std::vector<LineSegment2> &outlineLoop = outlinesSegments[outlineId];
    	assert(outlineLoop.size() > 0);

		// dbgs__("outline " << outlineId << "/" <<  outlinesSegments.size())
		// prepare  a new vector of loops for insets of this outline
		insetsForLoops.push_back(SegmentTable());
		assert(insetsForLoops.size() == outlineId + 1);

		SegmentTable &insetTable = *insetsForLoops.rbegin(); // inset curves for a single loop

		MyComputer myComputer;
		stringstream ss;
		ss << "_slice_" << sliceId << "_loop_" << outlineId << ".scad";
		string loopScadFile = myComputer.fileSystem.ChangeExtension(scadFile, ss.str());
		unsigned int shellId = 0;
		Shrinky shrinky(loopScadFile.c_str());
		try
		{
			unsigned int segmentCountBefore =0;
			unsigned int segmentCountAfter =0;

			for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
			{
				insetTable.push_back(std::vector<LineSegment2>());
			}
			for (unsigned int shellId=0; shellId < nbOfShells; shellId++)
			{

//				cout << "\n\n" << endl;
//				cout << "sliceId: "   << sliceId << endl;
//				cout << "	loop: " << outlineId << endl;
//				cout << "	shellId: " <<  shellId << endl;

				if(shellId == 0)
				{
					// first inset: inset only by the radius of the extrusion
					// (half the extrusion width)
					Scalar insetDistance = 0.5*layerW;
					assert(outlineLoop.size());

					std::vector<LineSegment2> &insets = insetTable[0];
					segmentCountBefore = outlineLoop.size();
					shrinky.inset(outlineLoop, insetDistance, insets);
					segmentCountAfter = insets.size();

					// assert(segmentCountAfter);
					assert(insetTable[0].size() == segmentCountAfter);
				}
				else
				{
					// normal inset: inset by the diameter of the extrusion
					// but leave interference for extrusions to stick together
					Scalar insetDistance = 0.8*layerW;
					assert(shellId > 0);
					unsigned int previousShell = shellId -1;
					assert(insetTable.size() > previousShell);

					std::vector<LineSegment2> &lastInsets = insetTable[previousShell];

					if(lastInsets.size())
					{
						std::vector<LineSegment2> &insets = insetTable[shellId];
						segmentCountBefore = lastInsets.size();
						assert(segmentCountBefore == segmentCountAfter);
						assert(insets.size() == 0);

						shrinky.inset(lastInsets, insetDistance, insets);
						segmentCountAfter = insets.size();
						assert(insetTable[shellId].size() == insets.size());
					}
				}
				//cout << "	inset nb of segments before: " << segmentCountBefore << endl;
				//cout << "	inset nb of segments after: "  << segmentCountAfter  << endl;
			}
		}
		catch(ShrinkyMess &messup)
		{
			cout << "sliceId: "   << sliceId << endl;
			cout << "loopId: " << outlineId << endl;
			cout << "shellId: " <<  shellId << endl;

			assert(0);
		}
	}
}

void mgl::sliceAndPath(	Meshy &mesh,
						double layerW,
						double tubeSpacing,
						double angle,
						unsigned int nbOfShells,
						const char* scadFile,
						std::vector< SliceData >  &slices)
{
	unsigned int extruderId = 0;
	assert(slices.size() == 0);

	Scalar tol = 1e-6; // Tolerance for assembling LineSegment2s into a loop

	// gather context info
	const std::vector<Triangle3> &allTriangles = mesh.readAllTriangles();
	const SliceTable &sliceTable = mesh.readSliceTable();
	const Limits& limits = mesh.readLimits();
	// cout << "Limits: " << limits << endl;
	Limits tubularLimits = limits.centeredLimits();
	tubularLimits.inflate(1.0, 1.0, 0.0);
	// make it square along z so that rotation happens inside the limits
	// hence, tubular limits around z
	tubularLimits.tubularZ();

	Vector3 c = limits.center();
	Vector2 toRotationCenter(-c.x, -c.y);
	Vector2 backToOrigin(c.x, c.y);

	size_t sliceCount = sliceTable.size();

	Vector3 rotationCenter = limits.center();
	Scalar layerH = mesh.readLayerMeasure().getLayerH();

	// we'll record that in a scad file for you
	ScadTubeFile outlineScad;
	if(scadFile != NULL)
	{
		outlineScad.open(scadFile);
		outlineScad.writePathViz(layerH, layerW, sliceCount);
	}
	slices.reserve(sliceCount);

	// multi thread stuff
#ifdef OMPFF
	omp_lock_t my_lock;
	omp_init_lock (&my_lock);
	#pragma omp parallel for
#endif


	ProgressBar progress(sliceCount);
	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		//cout << "<sliceId "  << sliceId << "/" <<  sliceCount << ">" << endl;
		const TriangleIndices &trianglesForSlice = sliceTable[sliceId];

		progress.tick();
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);


		// get the "real" 2D paths for outline
		SegmentTable outlinesSegments;
		segmentology(allTriangles, trianglesForSlice, z, tol, outlinesSegments);
		// keep all segments of insets for each loop

		unsigned int outlineSegmentCount = outlinesSegments.size();
		if(outlineSegmentCount > 0)
		{
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

			Scalar layerAngle = sliceId * angle;

			// deep copy the the infill boundaries
			SegmentTable rotatedSegments = outlinesSegments; // insetsForLoops[0];

			//dbgs__("insetforloops size " << insetsForLoops.size() )
			// deep copy the smallest insets as the infill boundaries

			if(insetsForLoops.size() > 0)
			{
				rotatedSegments= outlinesSegments;
			}

			mgl::translateLoops(rotatedSegments, toRotationCenter);
			// rotate the outlines before generating the tubes...
			mgl::rotateLoops(rotatedSegments, layerAngle);

			//cout << "<Pathology nb of loops ";
			//cout << rotatedSegments.size() << ">" << endl;
			std::vector<LineSegment2> infillSegments;
			infillPathology(rotatedSegments,
							tubularLimits,
							slice.z,
							tubeSpacing,
							infillSegments);
//			cout << "</Pathology>" << endl;

			createPolysFromloopSegments(outlinesSegments, slice.extruderSlices[extruderId].loops);

			// rotate and translate the TUBES so they fit with the ORIGINAL outlines
			mgl::rotateSegments(infillSegments, -layerAngle);
			mgl::translateSegments(infillSegments, backToOrigin);
			createPolysFromInfillSegments(infillSegments, slice.extruderSlices[extruderId].infills);

			// write the scad file
			// only one thread at a time in here
			if(scadFile != NULL)
			{
				#ifdef OMPFF
				OmpGuard lock (my_lock);
				cout << "slice "<< i << "/" << sliceCount << " thread: " << "thread id " << omp_get_thread_num() << " (pool size: " << omp_get_num_threads() << ")"<< endl;
				#endif

				outlineScad.writeTrianglesModule("tri_", mesh, sliceId);
				//outlineScad.writeOutlines(slice.extruderSlices[0].loops,  slices[i].z , i);
				// cout << "	<scad>" << endl;
				outlineScad.writePolygons("outlines_", "color([1,0,0,1])outline", slice.extruderSlices[extruderId].loops, slices[sliceId].z, sliceId);
				outlineScad.writePolygons("infill_",   "color([0,0,1,1])infill" , slice.extruderSlices[extruderId].infills, slices[sliceId].z, sliceId);

				for(unsigned int outlineId=0; outlineId <  insetsForLoops.size(); outlineId++)
				{
					stringstream ss;
					ss << "color([0,1,0,1])inset_" << outlineId << "_";
					outlineScad.writePolygons("infill_",   ss.str().c_str(), slice.extruderSlices[extruderId].insets[outlineId], slices[sliceId].z, sliceId);
				}

				// cout << "	</scad>" << endl;
			}
			// cout << "</sliceId"  << sliceId <<  ">" << endl;
		}
	}
	cout << "Done with slicing" << endl;
	// finalize the scad file
	if(scadFile != NULL)
	{
		cout << "finalize the scad file" << endl;
		outlineScad.writeSwitcher(sliceTable.size());
		outlineScad.close();
	}
}

index_t mgl::findOrCreateVertexIndex(std::vector<Vertex>& vertices ,
								const Vector3 &coords,
								Scalar tolerence)
{

	for(std::vector<Vertex>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		//const Vector3 &p = (*it).point;
		Vector3 &p = (*it).point;
		Scalar dx = coords.x - p.x;
		Scalar dy = coords.y - p.y;
		Scalar dz = coords.z - p.z;

		Scalar dd =  dx * dx + dy * dy + dz * dz;
		if( dd < tolerence )
		{
			//std::cout << "Found VERTEX" << std::endl;
			index_t vertexIndex = std::distance(vertices.begin(), it);
			return vertexIndex;
		}
	}

	index_t vertexIndex;
	// std::cout << "NEW VERTEX " << coords << std::endl;
	Vertex vertex;
	vertex.point = coords;
	vertices.push_back(vertex);
	vertexIndex = vertices.size() -1;
	return vertexIndex;
}


std::ostream& mgl::operator<<(std::ostream& os, const Edge& e)
{
	os << " " << e.vertexIndices[0] << "\t" << e.vertexIndices[1] << "\t" << e.face0 << "\t" << e.face1;
	return os;
}

std::ostream& mgl::operator<<(std::ostream& os, const Vertex& v)
{
	os << " " << v.point << "\t[ ";
	for (int i=0; i< v.faces.size(); i++)
	{
		if (i>0)  os << ", ";
		os << v.faces[i];
	}
	os << "]";
	return os;
}

std::ostream& mgl::operator << (std::ostream &os, const Connexity &s)
{
	s.dump(os);
	return os;
}



