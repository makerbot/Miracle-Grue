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

#include "mgl.h"
#include "meshy.h"
#include "segment.h"
#include "scadtubefile.h"
#include "shrinky.h"
#include "infill.h"

namespace mgl // Miracle-Grue's geometry library
{

// Slicer configuration data
struct Slicer
{
	Slicer()
	:layerH(0.27),
	 firstLayerZ(0.1),
	 tubeSpacing(1),
	 angle(1.570796326794897),
	 nbOfShells(2),
	 layerW(0.4),
	 infillShrinkingMultiplier(0.25),
	 insetDistanceMultiplier(0.9),
	 insetCuttOffMultiplier(0.01),
	 writeDebugScadFiles(false)
	{}

	Scalar layerH;
	Scalar firstLayerZ;
	Scalar tubeSpacing;
	Scalar angle;
	unsigned int nbOfShells;
	Scalar layerW;
	Scalar infillShrinkingMultiplier;
	Scalar insetDistanceMultiplier;
	Scalar insetCuttOffMultiplier;
	bool writeDebugScadFiles;
};

// slice data for an extruder
class ExtruderSlice
{
public:

	Polygons loops;  // outer perimeter loop
	Polygons infills;
	std::vector<Polygons> insets;
//	Polygons roofing;
};

::std::ostream& operator<<(::std::ostream& os, const ExtruderSlice& x);

//
// The Slice data is contains polygons
// for each extruder, for a slice.
// there are multiple polygons for each extruder
class SliceData
{
public:
	// TubesInSlice tubes;
	std::vector<ExtruderSlice > extruderSlices;

	double z;
	index_t sliceIndex;

	SliceData (double z, index_t sliceIndex)
		:z(z), sliceIndex(sliceIndex)//, tubes(z)
	{

	}

};

::std::ostream& operator<<(::std::ostream& os, const SliceData& x);


class Slicy
{
	// config info
	double layerW;
	const char* scadFile;
	unsigned int sliceCount;
	// Tolerance for assembling LineSegment2s into a loop
	Scalar tol;

	// we'll record that in a scad file for you
	ScadTubeFile fscad  ;

	//Mesh info
	const std::vector<Triangle3> &allTriangles;
	const Limits& limits;
	//-const LayerMeasure& zTapeMeasure;

	// state
	Scalar layerH;
	Vector2 toRotationCenter;
	Vector2 backToOrigin;
	Limits tubularLimits;



    void openScadFile(const char *scadFile, Scalar layerW, Scalar layerH, size_t sliceCount);
	void writeScadSlice(const TriangleIndices & trianglesForSlice,
						const Polygons & loopsPolys,
						const Polygons & infillsPolys,
						const std::vector<Polygons> & insetsPolys,
						Scalar zz,
						unsigned int sliceId );

	void closeScadFile();



public:
	Slicy(	const std::vector<Triangle3> &allTriangles,
			const Limits& limits,
			Scalar layerW,
			Scalar layerH,
			unsigned int sliceCount,
			const char* scadFile=NULL);

	~Slicy();


	bool slice( const TriangleIndices & trianglesForSlice,
				Scalar z,
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
