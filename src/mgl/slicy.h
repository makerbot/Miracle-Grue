/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

 */


#ifndef SLICY_H_
#define SLICY_H_

#include <ostream>
#include <algorithm>
#include <list>
#include <set>

#ifdef OMPFF
#include <omp.h>
#endif

#include "mgl.h"
#include "meshy.h"
#include "segment.h"
#include "shrinky.h"
#include "infill.h"
#include "pather.h"

namespace mgl // Miracle-Grue's geometry library
{

class Slicy {
	// config info
	Scalar layerW;
	const char* scadFile;
	unsigned int sliceCount;
	// Tolerance for assembling LineSegment2s into a loop
	Scalar tol;

	// we'll record that in a scad file for you
	ScadDebugFile fscad;

	//Mesh info
	const std::vector<libthing::Triangle3> &allTriangles;
	const Limits& limits;
	//-const LayerMeasure& zTapeMeasure;

	// state
	Scalar layerH;
	libthing::Vector2 toRotationCenter;
	libthing::Vector2 backToOrigin;
	Limits tubularLimits;



	void openScadFile(const char *scadFile, Scalar layerW, Scalar layerH, size_t sliceCount);
	void writeScadSlice(const TriangleIndices & trianglesForSlice,
			const Polygons & loopsPolys,
			const Polygons & infillsPolys,
			const std::vector<Polygons> & insetsPolys,
			Scalar zz,
			unsigned int sliceId);

	void closeScadFile();



public:
	Slicy(const std::vector<libthing::Triangle3> &allTriangles,
			const Limits& limits,
			Scalar layerW,
			Scalar layerH,
			unsigned int sliceCount,
			const char* scadFile = NULL);

	~Slicy();


	bool slice(const TriangleIndices & trianglesForSlice,
			unsigned int sliceId,
			unsigned int extruderId,
			Scalar tubeSpacing,
			Scalar sliceAngle,
			unsigned int nbOfShells,
			Scalar cutoffLength,
			Scalar infillShrinking,
			Scalar insetDistanceFactor,
			bool writeDebugScadFiles,
			SliceData &slice);



};



}

#endif /* SLICY_H_ */
