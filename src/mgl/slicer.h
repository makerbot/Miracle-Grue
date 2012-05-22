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
#include "meshy.h"

namespace mgl
{

// Slicer configuration data
struct SlicerConfig
{
	SlicerConfig()
	:layerH(0.27),
	 firstLayerZ(0.1),
	 tubeSpacing(1),
	 angle(1.570796326794897),
	 nbOfShells(2),
	 layerW(0.4),
	 infillShrinkingMultiplier(0.25),
	 insetDistanceMultiplier(0.9),
	 insetCuttOffMultiplier(0.01),
	 writeDebugScadFiles(false),
	 roofLayerCount(1),
	 floorLayerCount(1),
	 infillSkipCount(2),
	 gridSpacingMultiplier(0.95)
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

	unsigned int roofLayerCount;
	unsigned int floorLayerCount;
	unsigned int infillSkipCount;
	double infillDensity;
	Scalar gridSpacingMultiplier;
};

struct LayerConfig
{
	Scalar firstLayerZ;
	Scalar layerH;
	Scalar layerW;
	Scalar gridSpacingMultiplier;
};

struct Tomograph
{
	Tomograph():layerMeasure(0,0){}

	std::vector<libthing::SegmentTable>   outlines;
	Grid grid;
	LayerMeasure layerMeasure;
};

class Slicer : public Progressive
{
	LayerConfig layerCfg;

public:
	Slicer(const SlicerConfig &slicerCfg, ProgressBar *progress =NULL);
	void tomographyze( const char* modelFile, Tomograph &tomograph);

    void outlinesForSlice(const Meshy & mesh, size_t sliceId, libthing::SegmentTable & segments);

	void loopsFromLineSegments(const std::vector<libthing::LineSegment2>& unorderedSegments,
			Scalar tol, libthing::SegmentTable & segments);
};

}

#endif

