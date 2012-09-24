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

#include "log.h"
#include "slicy.h"
#include "insets.h"
#include "ScadDebugFile.h"



using namespace mgl;
using namespace std;

Slicy::Slicy(const std::vector<TriangleType> &allTriangles,
		const Limits& limits,
		Scalar layerW,
		Scalar layerH,
		unsigned int sliceCount,
		const char* scadFile)
		: layerW(layerW),
		scadFile(scadFile),
		sliceCount(sliceCount), 
		tol(1e-6),
		allTriangles(allTriangles),
		limits(limits), 
		layerH(layerH) {


	openScadFile(scadFile, layerW, layerH, sliceCount);

	tubularLimits = limits.centeredLimits();
	tubularLimits.inflate(1.0, 1.0, 0.0);
	// make it square along z so that rotation happens inside the limits
	// hence, tubular limits around z
	tubularLimits.tubularZ();

	Point3Type c = limits.center();

	toRotationCenter[0] = -c[0];
	toRotationCenter[1] = -c[1];
	backToOrigin[0] = c[0];
	backToOrigin[1] = c[1];

	Point3Type rotationCenter = limits.center();
}

Slicy::~Slicy() {
	closeScadFile();
}

void Slicy::openScadFile(const char *scadFile, double layerW, Scalar layerH, size_t sliceCount) {
	if (scadFile != NULL) {
		fscad.open(scadFile);

		std::ostream &out = fscad.getOut();

		out << "// use min and max to see individual layers " << std::endl;
		out << "min = 0;" << std::endl;
		out << "max = " << sliceCount << ";" << std::endl;
		out << "// triangles(min,max);" << std::endl;
		out << "// outlines(min,max);" << std::endl;
		out << "// infills(min,max);" << std::endl;
		out << "insets(min,max);" << std::endl;

		out << std::endl;
		out << "stl_color = [0,1,0, 0.025];" << std::endl;

		out << endl;
		out << "// x = [ [ points[s[0]], points[s[1]] ] for s in segments]" << endl;
		out << endl;

		out << "module out_line(x1, y1, z1, x2, y2, z2)" << std::endl;
		out << "{" << std::endl;
		out << "    tube(x1, y1, z1, x2, y2, z2, diameter1=0.4, diameter2=0, faces=4, thickness_over_width=1);" << std::endl;
		out << "}" << std::endl;

		out << std::endl;
		out << "module extrusion(x1, y1, z1, x2, y2, z2)" << std::endl;
		out << "{" << std::endl;
		out << "    d = " << layerH << ";" << std::endl;
		out << "    f = 6;" << std::endl;
		out << "    t =  " << layerH / layerW << ";" << std::endl;
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

		fscad.writeHeader();
	}
}

void Slicy::writeScadSlice(const TriangleIndices & trianglesForSlice,
		const Polygons & loopsPolys,
		const Polygons & infillsPolys,
		const vector<Polygons> & insetsPolys,
		Scalar zz,
		unsigned int sliceId) {
	if (scadFile != NULL) {
#ifdef OMPFF
		OmpGuard lock(my_lock);
		Log::info() << "slice " << sliceId << "/" << sliceCount << " thread: " << "thread id " << omp_get_thread_num() << " (pool size: " << omp_get_num_threads() << ")" << endl;
#endif

		fscad.writeTrianglesModule("tri_", allTriangles, trianglesForSlice, sliceId);
		fscad.writePolygons("outlines_", "color([0,0,1,1])outline", loopsPolys, zz, sliceId);
		fscad.writePolygons("infills_", "color([1,0,0,1])infill", infillsPolys, zz, sliceId);


		unsigned int insetCount = insetsPolys.size();
		for (unsigned int shellId = 0; shellId < insetCount; shellId++) {
			const Polygons &polygons = insetsPolys[shellId];
			stringstream ss;
			ss << "insets_" << sliceId << "_";

			fscad.writePolygons(ss.str().c_str(), "color([0,1,0,1])infill", polygons, zz, shellId);

		}

		// one function that calls all insets
		if (insetCount > 0) {
			stringstream ss;
			ss << "insets_" << sliceId;
			string insetsForSlice = ss.str();
			ss << "_";
			fscad.writeMinMax(insetsForSlice.c_str(), ss.str().c_str(), insetCount);
			//Log::often() << " SCAD: " << insetsForSlice.c_str() << endl;
		}
	}
}

void Slicy::closeScadFile() {
	// finalize the scad file
	if (scadFile != NULL) {
		fscad.writeMinMax("outlines", "outlines_", sliceCount);
		fscad.writeMinMax("triangles", "tri_", sliceCount);
		fscad.writeMinMax("infills", "infills_", sliceCount);
		fscad.writeMinMax("insets", "insets_", sliceCount);

		std::ostream &out = fscad.getOut();
		out << "// python snippets to make segments from polygon points" << endl;
		out << "// segments = [[ points[i], points[i+1]] for i in range(len(points)-1 ) ]" << endl;
		out << "// s = [\"segs.push_back(LineSegment2(Vector2(%s, %s), Vector2(%s, %s)));\" %(x[0][0], x[0][1], x[1][0], x[1][1]) for x in segments]" << std::endl;
		const char* scadfn = fscad.getScadFileName().c_str();
		Log::info() << "closing OpenSCad file: " << scadfn;
		fscad.close();
	}

}

bool Slicy::slice(const TriangleIndices & trianglesForSlice,
		unsigned int sliceId,
		unsigned int extruderId,
		Scalar, // tubeSpacing,
		Scalar, // sliceAngle,
		unsigned int nbOfShells,
		Scalar, // cutoffLength,
		Scalar infillShrinking,
		Scalar insetDistanceFactor,
		bool writeDebugScadFiles,
		SliceData &slice) {
	size_t skipCount = 1;

	Scalar z = slice.getZHeight();
	std::vector<SegmentType> segments;
	segmentationOfTriangles(trianglesForSlice, allTriangles, z, segments);
	// what we are left with is a series of segments (outline segments... triangle has beens)

	unsigned int cuts = segments.size();
	if (cuts == 0) {
		return false; // no segments for this slice.
	}
	slice.extruderSlices.push_back(ExtruderSlice());

	// get the "real" 2D paths for outline
	// lets order the segment into loops.
	SegmentTable outlinesSegments;
	loopsAndHoleOgy(segments, tol, outlinesSegments);
//	unsigned int outlineSegmentCount = 
//			outlinesSegments.size();
	createPolysFromloopSegments(outlinesSegments, 
			slice.extruderSlices[extruderId].boundary);


	//	dumpInsets(insetsForLoops)
	// create a vector of polygons for each shell.
	std::vector<Polygons> &insetsPolys = slice.extruderSlices[extruderId].insetLoopsList;

	// deep copy the the infill boundaries
	// because we are going to rotate them
	// We pick the innermost succesful inset for each loop
	SegmentTable innerOutlinesSegments;

	if (nbOfShells == 0) {
		innerOutlinesSegments = outlinesSegments;
	} else {
		InsetsCollection insetsForSlice;
		// create shells inside the outlines (and around holes)
		inshelligence(outlinesSegments,
				nbOfShells,
				layerW,
				// sliceId,
				insetDistanceFactor,
				scadFile,
				writeDebugScadFiles,
				insetsForSlice);
		// insetsPolys,
		// innerOutlinesSegments);
		polygonsFromLoopSegmentTables(nbOfShells, insetsForSlice, insetsPolys);
		innerOutlinesSegments = insetsForSlice.back();
	}



	Polygons& infills = slice.extruderSlices[extruderId].infills;

	bool infillDirection = sliceId % 2 == 0;

	infillosophy(innerOutlinesSegments,
			tubularLimits,
			z,
			layerW,
			skipCount,
			infillDirection,
			infillShrinking,
			infills);

	// write the scad file
	// only one thread at a time in here
	writeScadSlice(trianglesForSlice,
			slice.extruderSlices[extruderId].boundary,
			slice.extruderSlices[extruderId].infills,
			slice.extruderSlices[extruderId].insetLoopsList,
			z,
			sliceId);
	// Log::often() << "</sliceId"  << sliceId <<  ">" << endl;
	return true;
}

std::ostream& mgl::operator<<(::std::ostream& os, const ExtruderSlice& x) {
	os << " infill polygons: " << x.infills.size() << endl;
	os << " perimeter loop count: " << x.boundary.size() << endl;
	for (unsigned int i = 0; i < x.boundary.size(); i++) {
		const Polygon &poly = x.boundary[i];
		os << "    " << i << " polygon: " << poly.size() << 
				" points" << endl;
	}
	os << " nb of shells: " << x.insetLoopsList.size() << endl;
	for (unsigned int i = 0; i < x.insetLoopsList.size(); i++) {
		const Polygons &polys = x.insetLoopsList[i];
		os << "  shell " << i << endl;
		for (unsigned i = 0; i < polys.size(); i++) {
			const Polygon &poly = polys[i];
			os << "    " << i << " polygon: " << poly.size() << 
					" points" << endl;
		}
	}
	return os;
}

std::ostream& mgl::operator<<(::std::ostream& os, const SliceData& x) {
	os << "Slice " /*<< x.sliceIndex << ", z=" << x.z */ << ", " << 
			x.extruderSlices.size() << " extruders" << endl;
	for (unsigned int i = 0; i < x.extruderSlices.size(); i++) {
		os << "Extruder " << i << endl;
		const ExtruderSlice& xt = x.extruderSlices[i];
		os << xt;
	}
	return os;
}

