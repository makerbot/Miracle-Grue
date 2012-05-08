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


#include "insets.h"
#include "regioner.h"

namespace mgl
{

class Pather : public Progressive
{

public:


	Pather(ProgressBar * progress = NULL)
		:Progressive(progress)
	{
	}


	void generatePaths(const Regions &skeleton,
						std::vector<SliceData> &slices,
						size_t firstSliceIdx=-1,
						size_t lastSliceIdx=-1)
	{
		size_t sliceCount = skeleton.outlines.size();
	    if(firstSliceIdx == -1) firstSliceIdx = 0;
	    if(lastSliceIdx  == -1) lastSliceIdx = sliceCount-1;

		slices.resize(sliceCount);
		bool direction = false;
		unsigned int currentSlice = 0;

		initProgress("Path generation", sliceCount);

		for(size_t i=0; i < sliceCount; i++)
		{
			tick();

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

};



}

#endif
