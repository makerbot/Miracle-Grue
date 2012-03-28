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
#include "scadtubefile.h"
#include "shrinky.h"
#include "infill.h"

namespace mgl // Miracle-Grue's geometry library
{

// Slicer configuration data
struct Slicer
{
	Slicer()
	:layerH(0.27), firstLayerZ(0.1),
	 tubeSpacing(1), angle(1.570796326794897),
	 nbOfShells(2), layerW(0.4),
	 infillShrinkingMultiplier(0.25), insetDistanceMultiplier(0.9),
	 insetCuttOffMultiplier(0.01), writeDebugScadFiles(false)
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

	Polygons boundary;  // boundary loops for areas of this slice of a print.
	Polygons infills; // list of all lines that create infill for this layer

	PolygonsGroup  insetLoopsList;  /// a list, each entry of which is a Polygons
							/// object. Each inset[i] is all shell polygons
							/// for the matching loops[i] boundary for this layer

};

::std::ostream& operator<<(::std::ostream& os, const ExtruderSlice& x);

typedef std::vector<ExtruderSlice > ExtruderSlices;

/// The Slice data is contains polygons
/// for each extruder, for a slice.
/// there are multiple polygons for each extruder
class SliceData
{
private:
	Scalar zHeight;
	size_t index;

public:
	ExtruderSlices extruderSlices;

	/// @param inHeight: z height of this layer. Middle of the specified layer
	/// @param inIndex: layer number in this  model, positive in the 'up' direction
	SliceData(Scalar inHeight=0, size_t inIndex=0):zHeight(inHeight), index(inIndex)
	{
	}

	/// Updates position of slice in a model
	/// @param inHeight: z height of this layer. Middle of the specified layer
	/// @param inIndex: layer number in this  model, positive in the 'up' direction
	void updatePosition(Scalar inHeight,size_t inIndex){
		zHeight = inHeight;
		index = inIndex ;
	}

	Scalar getZHeight() const { return zHeight;}
	size_t getIndex()const  { return index;}


};

::std::ostream& operator<<(::std::ostream& os, const SliceData& x);


class Slicy
{
	// config info
	Scalar layerW;
	const char* scadFile;
	unsigned int sliceCount;
	// Tolerance for assembling LineSegment2s into a loop
	Scalar tol;

	// we'll record that in a scad file for you
	ScadTubeFile fscad;

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
