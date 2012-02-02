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


void mgl::sliceAndPath(	Meshy &mesh,
						double layerW,
						double tubeSpacing,
						double angle,
						const char* scadFile,
						std::vector< SliceData >  &slices)
{
	unsigned int extruderId = 0;
	unsigned int shells = 3;
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
		cout << "<sliceId "  << sliceId << "/" <<  sliceCount << ">" << endl;
		const TriangleIndices &trianglesForSlice = sliceTable[sliceId];

		progress.tick();
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);
		slices.push_back( SliceData(z,sliceId));
		SliceData &slice = slices[sliceId];
		slice.extruderSlices.push_back(ExtruderSlice());

		// get the "real" 2D paths for outline
		SegmentTable outlinesSegments;
		segmentology(allTriangles, trianglesForSlice, slice.z, tol, outlinesSegments);
		// keep all segments of insets for each loop
		std::vector<SegmentTable> insetsForLoops;
		unsigned int outlineSegmentCount = outlinesSegments.size();
		assert(outlineSegmentCount > 0);

		// dbgs__( "outlineSegmentCount " << outlineSegmentCount)
		for(unsigned int outlineId=0; outlineId < outlineSegmentCount; outlineId++)
		{
			// dbgs__("outline " << outlineId << "/" <<  outlinesSegments.size())
			// prepare  a new vector of loops for insets of this outline
			insetsForLoops.push_back(SegmentTable());
			SegmentTable &insetTable = *insetsForLoops.rbegin(); // inset curves for a single loop

			std::vector<LineSegment2> &loop =  outlinesSegments[outlineId];
			assert(loop.size() > 0);


			MyComputer myComputer;
			stringstream ss;

			ss << "_slice_" << sliceId << "_loop_" << outlineId << ".scad";
			string loopScadFile = myComputer.fileSystem.ChangeExtension(scadFile, ss.str());
			unsigned int shellId = 0;
			Shrinky shrinky(loopScadFile.c_str());
			try
			{
				for (shellId=0; shellId < shells; shellId++)
				{
					cout << "\n\n" << endl;
					cout << "sliceId: "   << sliceId << endl;
					cout << "	loop: " << outlineId << endl;
					cout << "	shellId: " <<  shellId << endl;
					cout << "	nb of segments: " << loop.size() << endl;

					insetTable.push_back(std::vector<LineSegment2>());
					std::vector<LineSegment2> &insets = *insetTable.rbegin();

					Scalar insetDistance = shellId==0? 0.5*layerW : layerW * 0.8;

					unsigned int segmentCountBefore = loop.size();
					shrinky.inset(loop, insetDistance, insets);
					unsigned int segmentCountAfter = insets.size();
					cout << "	inset nb of segments before: " << segmentCountBefore << endl;
					cout << "	inset nb of segments after: "  << segmentCountAfter  << endl;
					// next, we'll inset from the new polygon
					loop = insets;
				}
			}
			catch(ShrinkyMess &messup)
			{
				cout << "sliceId: "   << sliceId << endl;
				cout << "loop: " << outlineId << endl;
				cout << "shellId: " <<  shellId << endl;
				cout << "nb of segments: " << loop.size() << endl;
				assert(0);
			}
		}

//		dumpInsets(insetsForLoops);
		// create a vector of polygons for each shell.
		// for (unsigned int shellId=0; shellId < shells; shellId++)
		for(unsigned int outlineId=0; outlineId <  outlineSegmentCount; outlineId++)
		{
			cout << "oultline processing: " << outlineId << " of " << outlinesSegments.size() << endl;
			SegmentTable &insetTable = insetsForLoops[outlineId];

			// cout << "inset size " << slice.extruderSlices[extruderId].insets.size() << endl;
			slice.extruderSlices[extruderId].insets.push_back(Polygons());
			Polygons &polygons = *slice.extruderSlices[extruderId].insets.rbegin();
			createPolysFromloopSegments(insetTable , polygons );
		}
//dbg__
		Scalar layerAngle = sliceId * angle;
//dbg__
		// deep copy the the infill boundaries
		//std::vector<std::vector<LineSegment2> > rotatedSegments = outlinesSegments; // insetsForLoops[0];
//dbgs__("insetforloops size " << insetsForLoops.size() )
		// deep copy the smallest insets as the infill boundaries
		std::vector<std::vector<LineSegment2> > rotatedSegments = outlinesSegments;
		if(insetsForLoops.size() > 0)
		{
			rotatedSegments= insetsForLoops[0]; // outlinesSegments;
		}
//dbg__
		mgl::translateLoops(rotatedSegments, toRotationCenter);
		// rotate the outlines before generating the tubes...
		mgl::rotateLoops(rotatedSegments, layerAngle);

		cout << "<Pathology>" << endl;
		std::vector<LineSegment2> infillSegments;
		infillPathology(rotatedSegments,
						tubularLimits,
						slice.z,
						tubeSpacing,
						infillSegments);
		cout << "</Pathology>" << endl;
dbg__
		createPolysFromloopSegments(outlinesSegments, slice.extruderSlices[extruderId].loops);
dbg__
		// rotate and translate the TUBES so they fit with the ORIGINAL outlines
		mgl::rotateSegments(infillSegments, -layerAngle);
		mgl::translateSegments(infillSegments, backToOrigin);
		createPolysFromInfillSegments(infillSegments, slice.extruderSlices[extruderId].infills);
dbg__
		// write the scad file
		// only one thread at a time in here
		if(scadFile != NULL)
		{
			#ifdef OMPFF
			OmpGuard lock (my_lock);
			cout << "slice "<< i << "/" << sliceCount << " thread: " << "thread id " << omp_get_thread_num() << " (pool size: " << omp_get_num_threads() << ")"<< endl;
			#endif
dbg__
			outlineScad.writeTrianglesModule("tri_", mesh, sliceId);
			//outlineScad.writeOutlines(slice.extruderSlices[0].loops,  slices[i].z , i);
			cout << "	<scad>" << endl;
			outlineScad.writePolygons("outlines_", "outline", slice.extruderSlices[extruderId].loops, slices[sliceId].z, sliceId);
			outlineScad.writePolygons("infill_",   "infill" , slice.extruderSlices[extruderId].infills, slices[sliceId].z, sliceId);
			cout << "	</scad>" << endl;
		}
		cout << "</sliceId"  << sliceId <<  ">" << endl;
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



