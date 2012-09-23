/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

 */

#ifndef MIRACLE_H_
#define MIRACLE_H_

#include "gcoder.h"
#include "regioner.h"
#include "pather.h"
#include "log.h"
#include <iostream>

namespace mgl {



void miracleGrue(const GCoderConfig &gcoderCfg,
				 const SlicerConfig &slicerCfg,
				 const RegionerConfig& regionerCfg, 
				 const PatherConfig& patherCfg, 
				 const ExtruderConfig &extruderCfg,
				 const char *modelFile,
				 const char *scadFile,
				 std::ostream& gcodeFile,
				 int firstSliceIdx,
				 int lastSliceIdx,
				 RegionList &regions,
				 std::vector< SliceData > &slices,
				 ProgressBar* progress = NULL);

void slicesFromSlicerAndMesh(
		std::vector< SliceData > &slices,
		const SlicerConfig &slicer,
		Meshy& mesh,
		const char *scadFile,
		int firstSliceIdx = -1,
		int lastSliceIdx = -1,
		ProgressBar* progress = NULL);

void adjustSlicesToPlate(
		std::vector<SliceData>& slices,
		const LayerMeasure& layerMeasure,
		int firstSliceIdx = -1,
		int lastSliceIdx = -1);

void writeGcodeFromSlicesAndParams(
		const char *gcodeFile,
		GCoder &gcoder,
		std::vector<SliceData >& slices,
		const char *modelSource = "unknown source",
		ProgressBar* progress = NULL);

/// log the passed vector of slices to a directory
void slicesLogToDir(std::vector<SliceData>& slices, const char* logDirName);



};


#endif
