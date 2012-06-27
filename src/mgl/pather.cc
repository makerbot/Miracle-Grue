/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include "pather.h"

using namespace mgl;
using namespace std;

Pather::Pather(ProgressBar * progress)
		:Progressive(progress)
{
}


void Pather::generatePaths(const LayerLoops &layerloops,
					const Regions &skeleton,
					std::vector<SliceData> &slices,
					int sfirstSliceIdx,  // =-1
					int slastSliceIdx )  //
{

	size_t sliceCount = tomograph.outlines.size();
	size_t firstSliceIdx = 0;
	size_t lastSliceIdx  = sliceCount-1;

	if(sfirstSliceIdx > 0 )
	{
		firstSliceIdx = (size_t)sfirstSliceIdx;
	}

	if(slastSliceIdx  > 0 )
	{
		lastSliceIdx = (size_t)slastSliceIdx;
	}

	slices.resize(sliceCount);
	bool direction = false;
	unsigned int currentSlice = 0;

	initProgress("Path generation", sliceCount);

	for(size_t i=0; i < sliceCount; i++)
	{
		tick();

        if(i < firstSliceIdx) continue;
		if(i > lastSliceIdx) break;

		direction = !direction;
		SliceData& slice = slices[i];

		Scalar z = layerloops.layerMeasure.sliceIndexToHeight(currentSlice);
		currentSlice ++;

		slice.updatePosition(z, i);
		slice.extruderSlices.resize(1); //TODO: this blocks dualstrusion

		ExtruderSlice &extruderSlice = slice.extruderSlices[0];

		vector<LoopList> &insetLoops = skeleton.insets[i];
		LoopList &outlineLoops = tomograph.outlines[i];

		outlines(outlineLoops, extruderSlice.boundaryPaths);

        vector<LoopPathList> &insetPaths = extruderSlice.insetPaths;
		this->insets(insetLoops, insetPaths );

		const GridRanges &infillRanges = skeleton.infills[i];

		OpenPathList &infillPaths = extruderSlice.infills;
		this->infills(infillRanges, tomograph.grid, outlineLoops,
					  direction, infillPaths);
	}
}


void Pather::outlines(LoopList& outlineLoops, LoopPathList &boundaryPaths)
{
	//using a indeterminate start point for the beginning of the LoopPathList
	//as that's what the old Polygon logic did

	for (LoopList::iterator i = outlineLoops.begin();
		 i != outlineLoops.end(); ++i) {
		boundary.push_back(LoopPath(*i, i->clockwise(), i->counterClockwise()));
	}
}

void Pather::insets(vector<LoopList>& insetsForSlice,
					vector<LoopPathList> &insetPaths)
{
	for (vector<LoopList>::iterator i = insetsForSlice.begin();
		 i != insetsForSlice.end(); ++i) {

		insetPaths.push_back(LoopPathList());
		LoopPathList &lp_list = insetPathList.back();

		for (LoopList::iterator j = i->begin(); j != i->end(); ++j) {
			lp_list.push_back(LoopPath(*j, j->clockwise(),
									   j->counterClockwise()));
		}
	}
}


void Pather::infills(const GridRanges &infillRanges,
					 const Grid &grid,
					 const LoopList &outlines,
					 bool direction,
					 OpenPathList &infills)
{
	grid.pathsFromRanges(infillRanges, outlines, direction, infills);
}
