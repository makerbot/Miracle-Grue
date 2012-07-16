/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

 **/

#include <list>
#include <vector>

#include "regioner.h"
#include "loop_utils.h"

using namespace mgl;
using namespace std;
using namespace libthing;


Regioner::Regioner(const RegionerConfig& regionerConf, ProgressBar* progress)
		: Progressive(progress), regionerCfg(regionerConf) {
	roofLengthCutOff = 0.5 * regionerCfg.layerW;
}

void Regioner::generateSkeleton(const LayerLoops& layerloops,
								LayerMeasure &layerMeasure,
								RegionList& regionlist) {
	int sliceCount = initRegionList(layerloops, regionlist);


	initProgress("rafts", regionerCfg.raftLayers + 2);
	rafts(*(layerloops.begin()), layerMeasure, regionlist);

	RegionList::iterator firstModelRegion =
		regionlist.begin() + regionerCfg.raftLayers;

	initProgress("insets", sliceCount);
	insets(layerloops.begin(), layerloops.end(),
		   firstModelRegion, regionlist.end());

	initProgress("flat surfaces", sliceCount);
	flatSurfaces(regionlist.begin(), regionlist.end(), layerloops.grid);

	initProgress("roofing", sliceCount);
	roofing(firstModelRegion, regionlist.end(), layerloops.grid);

	initProgress("flooring", sliceCount);
	flooring(firstModelRegion, regionlist.end(), layerloops.grid);

	initProgress("infills", sliceCount);
	infills(regionlist.begin(), regionlist.end(),	layerloops.grid);
}

size_t Regioner::initRegionList(const LayerLoops& layerloops,
							  RegionList &regionlist) {
	//TODO: take into account raft layers in the number of regions
	regionlist.resize(layerloops.size());
	return regionlist.size();
}

void Regioner::rafts(const LayerLoops::Layer &bottomLayer,
					 LayerMeasure &layerMeasure,
					 RegionList &regionlist) {
	//make convex hull of the bottom layer
	Loop convexLoop = createConvexLoop(bottomLayer.readLoops());
	tick();

	SegmentTable convexSegs;
	convexSegs.push_back(std::vector<LineSegment2>());
	
	for(Loop::finite_cw_iterator iter = convexLoop.clockwiseFinite(); 
		iter != convexLoop.clockwiseEnd(); ++iter) {

		convexSegs.back().push_back(convexLoop.segmentAfterPoint(iter));
	}

	SegmentTable outsetSegs;
	outsetSegs.push_back(std::vector<LineSegment2>());

	//outset the convex hull by the configured distance
	ClipperInsetter().inset(convexSegs, -regionerCfg.raftOutset, outsetSegs);
	tick();

	Loop raftLoop;
	for(std::vector<LineSegment2>::const_iterator iter = 
			outsetSegs.back().begin(); 
		iter != outsetSegs.back().end(); 
		++iter) {
		raftLoop.insertPointBefore(iter->b, convexLoop.clockwiseEnd());
	}
	tick();

	//create a first layer measure with absolute positioning
    layer_measure_index_t baseIndex = layerMeasure.createAttributes();
	LayerMeasure::LayerAttributes &baseAttr =
		layerMeasure.getLayerAttributes(baseIndex);
	baseAttr.delta = 0;
	baseAttr.thickness = regionerCfg.raftBaseThickness;
	baseAttr.base = -1;

	//add interface raft layers in reverse order to the beginning of the list
	for (int raftnum = regionerCfg.raftLayers - 1; raftnum > 0; raftnum--) {
		layer_measure_index_t raftIndex = layerMeasure.createAttributes();
		LayerMeasure::LayerAttributes &raftAttr =
			layerMeasure.getLayerAttributes(raftIndex);
		raftAttr.delta = (raftnum - 1) * regionerCfg.raftInterfaceThickness;
		raftAttr.thickness = regionerCfg.raftInterfaceThickness;
		raftAttr.base = baseIndex;

		regionlist.insert(regionlist.begin(), LayerRegions());
		LayerRegions &raftRegions = regionlist.front();
		raftRegions.supportLoops.push_back(raftLoop);
		raftRegions.layerMeasureId = raftIndex;

		tick();
	}		
	
	//add the actual regionlist for the base layer
	regionlist.insert(regionlist.begin(), LayerRegions());
	LayerRegions &baseRegions = regionlist.front();
	baseRegions.supportLoops.push_back(raftLoop);
	baseRegions.layerMeasureId = baseIndex;

	//make the first layer of the model relative to the last raft layer
	layerMeasure.getLayerAttributes(bottomLayer.getIndex()).base =
		regionlist[regionerCfg.raftLayers - 1].layerMeasureId;

	tick();
}

void Regioner::insetsForSlice(const LoopList& sliceOutlines,
		std::list<LoopList>& sliceInsets,
		const char* scadFile) {

	libthing::SegmentTable sliceOutlinesOld;
	libthing::Insets sliceInsetsOld;
	
	/*
	 This function makes use of inshelligence, which indirectly makes use of 
	 clipper.cc, a machine translated Delphi library designed to make use of 
	 SegmentTables. As it is currently not practical to quickly convert this 
	 library to use the new Loop type, we instead elected to simply convert 
	 the resulting SegmentTables into Loops
	 */

	//convert outline Loops into equivalent SegmentTables
	for (LoopList::const_iterator iter = sliceOutlines.begin();
			iter != sliceOutlines.end();
			++iter) {
		sliceOutlinesOld.push_back(std::vector<libthing::LineSegment2 > ());
		const Loop& currentLoop = *iter;
		//Convert current loop to a vector of segments
		for (Loop::const_finite_cw_iterator loopiter =
				currentLoop.clockwiseFinite();
				loopiter != currentLoop.clockwiseEnd();
				++loopiter) {
			sliceOutlinesOld.back().push_back(currentLoop.segmentAfterPoint(loopiter));
		}
	}

	//call the function with the equivalent SegmentTables
	bool writeDebugScadFiles = false;
	inshelligence(sliceOutlinesOld,
			regionerCfg.nbOfShells,
			regionerCfg.layerW,
			regionerCfg.insetDistanceMultiplier,
			scadFile,
			writeDebugScadFiles,
			sliceInsetsOld);

	//Recover loops from the resulting SegmentTable
	for (libthing::Insets::const_iterator iter = sliceInsetsOld.begin();
			iter != sliceInsetsOld.end();
			++iter) {
		sliceInsets.push_back(LoopList());
		LoopList& currentLoopList = sliceInsets.back();
		//Convert each group of insets into a list of Loops
		for (libthing::SegmentTable::const_iterator setIter = iter->begin();
				setIter != iter->end();
				++setIter) {
			currentLoopList.push_back(Loop());
			const std::vector<libthing::LineSegment2>& currentPoly = *setIter;
			Loop& currentLoop = currentLoopList.back();
			Loop::cw_iterator loopIter = currentLoop.clockwiseEnd();
			//Convert each individual inset to a Loop
			//Insert points 1 - N
			for (std::vector<libthing::LineSegment2>::const_iterator polyIter =
					currentPoly.begin();
					polyIter != currentPoly.end();
					++polyIter) {
				loopIter = currentLoop.insertPointAfter(polyIter->b, loopIter);
			}
			//Insert point 0
			if (!currentPoly.empty())
				loopIter = currentLoop.insertPointAfter(currentPoly.begin()->a, loopIter);
		}
	}
}

//void Regioner::insets(const std::vector<libthing::SegmentTable> & outlinesSegments,
//		std::vector<libthing::Insets> & insets) {
//
//	unsigned int sliceCount = outlinesSegments.size();
//	initProgress("insets", sliceCount);
//	insets.resize(sliceCount);
//
//	// slice id must be adjusted for
//	for (size_t i = 0; i < sliceCount; i++) {
//		tick();
//		const libthing::SegmentTable & sliceOutlines = outlinesSegments[i];
//		libthing::Insets & sliceInsets = insets[i];
//
//		insetsForSlice(sliceOutlines, sliceInsets);
//	}
//}

void Regioner::insets(const LayerLoops::const_layer_iterator outlinesBegin,
					  const LayerLoops::const_layer_iterator outlinesEnd,
					  RegionList::iterator regionsBegin,
					  RegionList::iterator regionsEnd) {

	LayerLoops::const_layer_iterator outline = outlinesBegin;
	RegionList::iterator region = regionsBegin;
	while(outline != outlinesEnd && region != regionsEnd) {
		tick();
		const LoopList& currentOutlines = outline->readLoops();

		insetsForSlice(currentOutlines, region->insetLoops, NULL);

		++outline;
		++region;
	}
}

void Regioner::flatSurfaces(RegionList::iterator regionsBegin,
							RegionList::iterator regionsEnd,
							const Grid& grid) {
	for (RegionList::iterator regions = regionsBegin;
		 regions != regionsEnd; regions++) {
		tick();
		const std::list<LoopList>& currentInsets = regions->insetLoops;
		GridRanges currentSurface;
		gridRangesForSlice(currentInsets, grid, regions->flatSurface);
		gridRangesForSlice(regions->supportLoops, grid, regions->support);
	}
}

void Regioner::floorForSlice(const GridRanges & currentSurface,
		const GridRanges & surfaceBelow,
		const Grid & grid,
		GridRanges & flooring) {
	GridRanges floor;
	grid.gridRangeDifference(currentSurface, surfaceBelow, floor);
	grid.trimGridRange(floor, roofLengthCutOff, flooring);
}

void Regioner::roofForSlice(const GridRanges & currentSurface, const GridRanges & surfaceAbove, const Grid & grid, GridRanges & roofing) {
	GridRanges roof;
	grid.gridRangeDifference(currentSurface, surfaceAbove, roof);
	grid.trimGridRange(roof, this->roofLengthCutOff, roofing);
}

void Regioner::roofing(RegionList::iterator regionsBegin,
					   RegionList::iterator regionsEnd,
					   const Grid& grid) {

	RegionList::iterator current = regionsBegin;
	RegionList::iterator above = current;
	++above;

	while (above != regionsEnd) {
		tick();
		const GridRanges & currentSurface = current->flatSurface;
		const GridRanges & surfaceAbove = above->flatSurface;
		GridRanges & roofing = current->roofing;

		GridRanges roof;
		roofForSlice(currentSurface, surfaceAbove, grid, roof);

		grid.trimGridRange(roof, roofLengthCutOff, roofing);

		++current;
		++above;
	}

	tick();
	current->roofing = current->flatSurface;
}

void Regioner::flooring(RegionList::iterator regionsBegin,
						RegionList::iterator regionsEnd,
						const Grid &grid) {
	RegionList::iterator below = regionsBegin;
	RegionList::iterator current = below;
	current++;

	while (current != regionsEnd) {
		tick();
		const GridRanges & currentSurface = current->flatSurface;
		const GridRanges & surfaceBelow = below->flatSurface;
		GridRanges & flooring = current->flooring;

		floorForSlice(currentSurface, surfaceBelow, grid, flooring);

		++below;
		++current;
	}

	tick();
	regionsBegin->flooring = regionsBegin->flatSurface;

}

void Regioner::infills(RegionList::iterator regionsBegin,
					   RegionList::iterator regionsEnd,
					   const Grid &grid) {
	
	for (RegionList::iterator current = regionsBegin;
		 current != regionsEnd; current++) {

		const GridRanges &surface = current->flatSurface;
		tick();

		// Solids
		GridRanges combinedSolid;

		combinedSolid.xRays.resize(surface.xRays.size());
		combinedSolid.yRays.resize(surface.yRays.size());

		//TODO: no reason to get bounds separately from the combination

		//find the bounds we will be combinging regions across
		RegionList::iterator firstFloor = current;
		for (int i = 0; i < regionerCfg.floorLayerCount &&
   				        firstFloor != regionsBegin; ++i)
			--firstFloor;

		RegionList::iterator lastRoof = current;
		for (int i = 0; i < regionerCfg.roofLayerCount &&
				        lastRoof != regionsEnd - 1; ++i)
			++lastRoof;

		//combine floors
		for (RegionList::iterator floor = firstFloor;
			 floor <= current; ++floor) {
			GridRanges multiFloor;

			grid.gridRangeUnion(combinedSolid, floor->flooring, multiFloor);
			combinedSolid = multiFloor;
		}

		//combine roofs
		for (RegionList::iterator roof = current;
			 roof <= lastRoof; ++roof) {
			GridRanges multiRoof;

			grid.gridRangeUnion(combinedSolid, roof->roofing, multiRoof);
			combinedSolid = multiRoof;
		}

		// solid now contains the combination of combinedSolid regions from
		// multiple slices. We need to extract the perimeter from it

		grid.gridRangeIntersection(surface, combinedSolid, current->solid);

		// TODO: move me to the slicer
		GridRanges sparseSupport;
		GridRanges sparseInfill;
		GridRanges all;
		size_t infillSkipCount = (int) (1 / regionerCfg.infillDensity) - 1;

		grid.subSample(surface, infillSkipCount, sparseInfill);
		grid.subSample(current->support, infillSkipCount, sparseSupport);

		grid.gridRangeUnion(current->solid, sparseInfill, all);
		grid.gridRangeUnion(current->support, all, current->infill);

		//TODO: this doesn't seem right, but its what it was doing before I
		//converted to iterators
		current->sparse = current->infill;

	}

}

void Regioner::gridRangesForSlice(const std::list<LoopList>& allInsetsForSlice,
		const Grid& grid,
		GridRanges & surface) {
	const LoopList& innerMostLoops = allInsetsForSlice.back();
	grid.createGridRanges(innerMostLoops, surface);
}

void Regioner::gridRangesForSlice(const LoopList& innerMostLoops,
		const Grid& grid,
		GridRanges & surface) {
	grid.createGridRanges(innerMostLoops, surface);
}



