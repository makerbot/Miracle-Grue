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
#include "slicer_loops.h"

namespace mgl {

//// Slicer configuration data

class SlicerConfig {
public:
	SlicerConfig()
			: layerH(0.27),
			firstLayerZ(0.1) {}

	// These are relevant to slicer
	Scalar layerH; //< z height of layers 1+ 9(mm)
	Scalar firstLayerZ; //< z height of 0th layer (mm)
};

struct LayerConfig {
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

struct Tomograph {

	Tomograph() : layerMeasure(0, 0, 1) {
	}

	std::vector<SegmentTable> outlines;
	Grid grid;
	LayerMeasure layerMeasure;
};


/// This class contains the slice processes that are run on
/// input

class Slicer : public Progressive {
	LayerConfig layerCfg;

public:
	/// Constructor for a slicer
	/// @param slicerCfg slicer config!
	/// @param progress Optional Progress Bar
	Slicer(const SlicerConfig &slicerCfg, ProgressBar *progress = NULL);
    Slicer(const GrueConfig& grueCfg, ProgressBar* progress = NULL);

	/// TBD
	void generateLoops(const Segmenter& seg, LayerLoops& layerloops);

	/// TBD
	void outlinesForSlice(const Segmenter& seg,
			size_t sliceId,
			SegmentTable & segments);

	/// TBD
	void loopsFromLineSegments(const std::vector<Segment2Type>&
			unorderedSegments,
			Scalar tol,
			SegmentTable & segments);
};

}

#endif

