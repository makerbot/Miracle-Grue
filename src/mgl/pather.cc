/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include <list>
#include <vector>

#include "pather.h"
#include "limits.h"

using namespace mgl;
using namespace std;

Pather::Pather(ProgressBar * progress)
		:Progressive(progress)
{
}


void Pather::generatePaths(const ExtruderConfig &extruderCfg,
						   const RegionList &skeleton,
						   const LayerMeasure &layerMeasure,
						   const Grid &grid,
						   LayerPaths &layerpaths,
						   int sfirstSliceIdx,  // =-1
						   int slastSliceIdx )  //
{
	size_t firstSliceIdx = 0;
	size_t lastSliceIdx = INT_MAX;

	if(sfirstSliceIdx > 0 )
	{
		firstSliceIdx = (size_t)sfirstSliceIdx;
	}

	if(slastSliceIdx  > 0 )
	{
		lastSliceIdx = (size_t)slastSliceIdx;
	}

	bool direction = false;
	unsigned int currentSlice = 0;

	initProgress("Path generation", skeleton.size());

	for(RegionList::const_iterator layerRegions = skeleton.begin();
		layerRegions != skeleton.end(); ++layerRegions)
	{
		tick();

		if(currentSlice < firstSliceIdx) continue;
		if(currentSlice > lastSliceIdx) break;

		direction = !direction;
		const layer_measure_index_t layerMeasureId =
			layerRegions->layerMeasureId;

		//adding these should be handled in gcoder
		const Scalar h = layerMeasure.getLayerThickness(layerMeasureId);
		const Scalar z = layerMeasure.getLayerPosition(layerMeasureId) + h;

		layerpaths.push_back(LayerPaths::Layer(z, h, layerMeasureId));

		LayerPaths::Layer& lp_layer = layerpaths.back();
		
		//TODO: this only handles the case where the user specifies the extruder
		// it does not handle a dualstrusion print
		lp_layer.extruders.push_back(
				 LayerPaths::Layer::ExtruderLayer(extruderCfg.defaultExtruder));
		LayerPaths::Layer::ExtruderLayer& extruderlayer =
			lp_layer.extruders.back();

		const std::list<LoopList>& insetLoops = layerRegions->insetLoops;

		outlines(layerRegions->outlines, extruderlayer.outlinePaths);

		insets(insetLoops, extruderlayer.insetPaths);

		const GridRanges& infillRanges = layerRegions->infill;
		const GridRanges& supportRanges = layerRegions->support;
		
		size_t sx = supportRanges.xRaysCount(), 
				sy = supportRanges.yRaysCount();
		
		//cout << "[" << sx << ", " << sy <<"]" << endl;

		infills(infillRanges, grid, layerRegions->outlines,
				direction, extruderlayer.infillPaths);
		
		infills(supportRanges, grid, layerRegions->supportLoops,
				direction, extruderlayer.supportPaths);
		
		++currentSlice;
	}
}

void Pather::outlines(const LoopList& outline_loops,
					  LoopPathList &boundary_paths)
{
	//using a indeterminate start point for the beginning of the LoopPathList
	//as that's what the old Polygon logic did

	for (LoopList::const_iterator i = outline_loops.begin();
		 i != outline_loops.end(); ++i) {
		boundary_paths.push_back(LoopPath(*i, i->clockwise(),
										  i->counterClockwise()));
	}
}


void Pather::insets(const list<LoopList>& inset_loops,
					list<LoopPathList> &inset_paths)
{
	for (list<LoopList>::const_iterator i = inset_loops.begin();
		 i != inset_loops.end(); ++i) {

		inset_paths.push_back(LoopPathList());
		LoopPathList& lp_list = inset_paths.back();

		for (LoopList::const_iterator j = i->begin(); j != i->end(); ++j) {
			lp_list.push_back(LoopPath(*j, j->clockwise(),
									   j->counterClockwise()));
		}
	}
}


void Pather::infills(const GridRanges &infillRanges,
					 const Grid &grid,
					 const LoopList &outlines,
					 const bool direction,
					 OpenPathList &infills)
{
	grid.pathsFromRanges(infillRanges, outlines, direction, infills);
}
