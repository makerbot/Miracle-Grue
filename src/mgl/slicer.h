/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef __SLICER_H_
#define __SLICER_H_

#include "abstractable.h"
#include "grid.h"
#include "configuration.h"
#include "insets.h"
#include "segmenter.h"

namespace mgl
{

//// Slicer configuration data
struct SlicerConfig
{
	SlicerConfig()
	:layerH(0.27),
	 firstLayerZ(0.1),
	 tubeSpacing(1),
	 //angle(1.570796326794897),
	 nbOfShells(2),
	 layerW(0.4),
	 infillShrinkingMultiplier(0.25),
	 insetDistanceMultiplier(0.9),
	 insetCuttOffMultiplier(0.01),
	 writeDebugScadFiles(false),
	 roofLayerCount(0),
	 floorLayerCount(0),
         // infillSkipCount(2),
	 gridSpacingMultiplier(0.95)
	{}

	Scalar layerH; //< z height of layers 1+ 9(mm)
	Scalar firstLayerZ; //< z height of 0th layer (mm)
	Scalar tubeSpacing; //< distance in between infill (mm)
	Scalar angle;	//< angle of infill
	unsigned int nbOfShells; //< shell count of model
	Scalar layerW; //< TBD
	Scalar infillShrinkingMultiplier; //< TBD
	Scalar insetDistanceMultiplier; //< TBD
	Scalar insetCuttOffMultiplier; //< TBD
	bool writeDebugScadFiles; //< true if we want to output debug scad files

	unsigned int roofLayerCount; // number of solid layers for roofs
	unsigned int floorLayerCount; // number of solid layers for floors
	//unsigned int infillSkipCount; //< TBD
	Scalar infillDensity; // the density of the infill patterns (0 to 1)
	Scalar gridSpacingMultiplier; // interference between 2 grid lines ( 0 to 1, for adhesion)
};

struct LayerConfig
{
	Scalar firstLayerZ; //z height of 0th layer(mm)
	Scalar layerH; //z height of 1+ layer (mm)
	Scalar layerW; // width of layer (mm)
	Scalar gridSpacingMultiplier; /// TBD:w
};

///
/// Structure to contain model-wide slicing data used to generate regions of
/// infill, etc.  All of the outlines of the model for every slice,
/// slicer output.
///
struct Tomograph
{
	Tomograph():layerMeasure(0,0){}

	std::vector<libthing::SegmentTable>   outlines;
	Grid grid;
	LayerMeasure layerMeasure;
};


/// This class contains the slice processes that are run on
/// input
class Slicer : public Progressive
{
	LayerConfig layerCfg;

public:
	/// Constructor for a slicer
	/// @param slicerCfg slicer config!
	/// @param progress Optional Progress Bar
	Slicer(const SlicerConfig &slicerCfg, ProgressBar *progress =NULL);

	/// TBD
	void tomographyze( Segmenter& seg, Tomograph &tomograph);

	/// TBD
    void outlinesForSlice(const Segmenter& seg, size_t sliceId, libthing::SegmentTable & segments);

	/// TBD
	void loopsFromLineSegments(const std::vector<libthing::LineSegment2>& unorderedSegments,
			Scalar tol, libthing::SegmentTable & segments);
};

}

#endif

