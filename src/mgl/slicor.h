/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef SLICOR_H_
#define SLICOR_H_

#include "gcoder.h"
#include "insets.h"

namespace mgl
{

class Pather
{
	ProgressBar *progress;
public:
	// GCoder gcoder;

	Pather()
		:progress(NULL)
	{
	}

//	void init(const GCoder &gcoderCfg, ProgressBar *progress = NULL)
	void init(ProgressBar *progress = NULL)
	{
		this->progress = progress;
		// this->gcoder = gcoderCfg;
	}

	// allow progress callback
	void initProgress(const char*task, size_t tickCount)
	{
		if(progress)
		{
			progress->reset(tickCount, task);
		}
	}

	// update parent
	void tick()
	{
		if(progress)
		{
			progress->tick();
		}
	}

	void generatePaths(const ModelSkeleton &skeleton, std::vector<SliceData> &slices, size_t firstSliceIdx=-1, size_t lastSliceIdx=-1)
	{
		size_t sliceCount = skeleton.outlines.size();
	    if(firstSliceIdx == -1) firstSliceIdx = 0;
	    if(lastSliceIdx  == -1) lastSliceIdx = sliceCount-1;

		slices.resize(sliceCount);
		bool direction = false;
		unsigned int currentSlice = 0;

		if(progress){progress->reset(sliceCount, "Path generation");}

		for(size_t i=0; i < sliceCount; i++)
		{
			if(progress){progress->tick();}

	        if(i <  firstSliceIdx) continue;
	        if(i > lastSliceIdx) break;

			direction = !direction;
			SliceData& slice = slices[i];

			Scalar z = skeleton.layerMeasure.sliceIndexToHeight(currentSlice);
			currentSlice ++;

			slice.updatePosition(z, i);
			slice.extruderSlices.resize(1);

			ExtruderSlice &extruderSlice = slice.extruderSlices[0];

			const libthing::Insets &insetsSegments = skeleton.insets[i];
			const libthing::SegmentTable &outlineSegments = skeleton.outlines[i];

			outlines(outlineSegments, extruderSlice.boundary);

			PolygonsGroup &insetPolys = extruderSlice.insetLoopsList;
			this->insets(insetsSegments, insetPolys );

			const GridRanges &infillRanges = skeleton.infills[i];

			Polygons &infillsPolygons = extruderSlice.infills;
			this->infills(infillRanges, skeleton.grid, direction, infillsPolygons);
		}
	}

	void outlines(const libthing::SegmentTable& outlinesSegments, Polygons &boundary)
	{
		createPolysFromloopSegments(outlinesSegments, boundary);
	}

	void insets(const libthing::Insets& insetsForSlice, PolygonsGroup &insetPolys)
	{
		size_t nbOfShells = insetsForSlice.size();
		polygonsFromLoopSegmentTables(nbOfShells, insetsForSlice, insetPolys);
	}

	void infills(const GridRanges &infillRanges,
					const Grid &grid,
					bool direction,
					Polygons &infills)
	{
		grid.polygonsFromRanges(infillRanges, direction, infills);
	}

	/*
	void writeGcode(const char *gcodeFile, const char* modelSource, const std::vector<SliceData> &slices)
	{
		std::ofstream gout(gcodeFile);
		gcoder.writeStartOfFile(gout, modelSource);
		size_t sliceCount = slices.size();
		initProgress("gcode", sliceCount);
		for(size_t sliceId=0; sliceId < sliceCount; sliceId++)
		{
			tick();
			const SliceData &slice = slices[sliceId];
			gcoder.writeSlice(gout, slice);
		}
		gout.close();
	}
	*/
};



}

#endif /* SLICOR_H_ */
