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
}

void Regioner::generateSkeleton(const LayerLoops& layerloops,
		LayerMeasure& layerMeasure,
		RegionList& regionlist,
		Limits& limits,
		Grid& grid) {
//	int debuglayer = 0;
//	for(LayerLoops::const_layer_iterator layerIter = layerloops.begin(); 
//			layerIter != layerloops.end(); 
//			++layerIter, ++debuglayer) {
//		std::cout << "Layer: " << debuglayer << " \tLoops: \t" 
//				<< layerIter->readLoops().size() << std::endl;
//	}
	layerMeasure.setLayerWidthRatio(regionerCfg.layerWidthRatio);
	RegionList::iterator firstmodellayer;
	int sliceCount = initRegionList(layerloops, regionlist, layerMeasure,
			firstmodellayer);
	roofLengthCutOff = 0.5 * layerMeasure.getLayerW();

	if (regionerCfg.doSupport) {
		initProgress("support", sliceCount*2);
		support(firstmodellayer, regionlist.end(), layerMeasure);
	}

	limits.inflate(regionerCfg.raftOutset + 10,
			regionerCfg.raftOutset + 10,
			0);
	//optionally inflate if rafts present
	if (regionerCfg.raftLayers > 0) {
		limits.inflate(0, 0,
				regionerCfg.raftBaseThickness +
				regionerCfg.raftInterfaceThickness *
				(regionerCfg.raftLayers - 1));
	}

	grid.init(limits, layerMeasure.getLayerW() *
			regionerCfg.gridSpacingMultiplier);

	if (regionerCfg.raftLayers > 0) {
		initProgress("rafts", regionerCfg.raftLayers + 4);
		rafts(*firstmodellayer, layerMeasure, regionlist);
	}

	//LayerRegions &raftlayer = regionlist.front();

	RegionList::iterator firstModelRegion =
			regionlist.begin() + regionerCfg.raftLayers;

	initProgress("insets", sliceCount);
	insets(layerloops.begin(), layerloops.end(),
			firstModelRegion, regionlist.end(),
			layerMeasure);

	initProgress("flat surfaces", sliceCount);
	flatSurfaces(regionlist.begin(), regionlist.end(), grid);

	initProgress("roofing", sliceCount);
	roofing(firstModelRegion, regionlist.end(), grid);

	initProgress("flooring", sliceCount);
	flooring(firstModelRegion, regionlist.end(), grid);

	initProgress("infills", sliceCount);
	infills(regionlist.begin(), regionlist.end(), grid);
}

size_t Regioner::initRegionList(const LayerLoops& layerloops,
		RegionList &regionlist,
		LayerMeasure& layermeasure,
		RegionList::iterator& firstmodellayer) {
	//copy over data from layerloops
	for (LayerLoops::const_layer_iterator iter = layerloops.begin();
			iter != layerloops.end();
			++iter) {
		LayerRegions currentRegions;
		currentRegions.outlines = iter->readLoops();
		currentRegions.layerMeasureId = iter->getIndex();

		LayerMeasure::LayerAttributes& currentAttribs =
				layermeasure.getLayerAttributes(currentRegions.layerMeasureId);

		//set an appropriate ratio
		currentAttribs.widthRatio = layermeasure.getLayerWidthRatio();

		if (iter != layerloops.begin()) {
			//this is not the first layer, make it relative to first
			currentAttribs.base = regionlist.front().layerMeasureId;
		}

		regionlist.push_back(currentRegions);
	}

	firstmodellayer = regionlist.begin();

	//if we do rafts
	if (regionerCfg.raftLayers) {
		//insert appropriate number of raft layers at start
		regionlist.insert(regionlist.begin(), regionerCfg.raftLayers,
				LayerRegions());
		//for each raft create an entry in layermeasure
		RegionList::iterator iter = regionlist.begin();
		for (size_t raftidx = 0; raftidx < regionerCfg.raftLayers;
				++raftidx, ++iter) {
			iter->layerMeasureId = layermeasure.createAttributes(
					LayerMeasure::LayerAttributes(0, 0,
					layermeasure.getLayerWidthRatio()));
		}
		//make all model layers be relative to top raft
		--iter;
		RegionList::iterator iterModel = iter;
		++iterModel;
		firstmodellayer = iterModel;
		//make the bottom model layer relative to top raft
		LayerMeasure::LayerAttributes& bottomAttribs =
				layermeasure.getLayerAttributes(iterModel->layerMeasureId);
		bottomAttribs.base = iter->layerMeasureId;
		bottomAttribs.delta = regionerCfg.raftInterfaceThickness +
				regionerCfg.raftModelSpacing;
		//and the rest relative to it
		//the rest are already relative to it
		//		++iter;
		//		++iterModel;
		//		for (; iterModel != regionlist.end(); ++iterModel) {
		//			layermeasure.getLayerAttributes(iterModel->layerMeasureId).base =
		//					iter->layerMeasureId;
		//		}
	}

	return regionlist.size();
}

void Regioner::rafts(const LayerRegions& bottomLayer,
		LayerMeasure &layerMeasure,
		RegionList &regionlist) {
	//assemble a list of all the loops to consider
	LoopList raftSrcLoops;
	//fill it with model loops and support loops
	raftSrcLoops.insert(raftSrcLoops.end(), bottomLayer.outlines.begin(),
			bottomLayer.outlines.end());
	raftSrcLoops.insert(raftSrcLoops.end(), bottomLayer.supportLoops.begin(),
			bottomLayer.supportLoops.end());
	//make convex hull from all the things we consider
	Loop convexLoop = createConvexLoop(raftSrcLoops);
	tick();

	SegmentTable convexSegs;
	convexSegs.push_back(std::vector<LineSegment2 > ());

	for (Loop::finite_cw_iterator iter = convexLoop.clockwiseFinite();
			iter != convexLoop.clockwiseEnd(); ++iter) {

		convexSegs.back().push_back(convexLoop.segmentAfterPoint(iter));
	}

	SegmentTable outsetSegs;
	outsetSegs.push_back(std::vector<LineSegment2 > ());

	//outset the convex hull by the configured distance
	ClipperInsetter().inset(convexSegs, -regionerCfg.raftOutset, outsetSegs);
	tick();

	Loop raftLoop;
	for (std::vector<LineSegment2>::const_iterator iter =
			outsetSegs.back().begin();
			iter != outsetSegs.back().end();
			++iter) {
		raftLoop.insertPointBefore(iter->b, raftLoop.clockwiseEnd());
	}
	tick();

	//create a first layer measure with absolute positioning
	//already done when regionlist initialized
	//layer_measure_index_t baseIndex = layerMeasure.createAttributes();
	layer_measure_index_t baseIndex = regionlist.front().layerMeasureId;
	LayerMeasure::LayerAttributes &baseAttr =
			layerMeasure.getLayerAttributes(baseIndex);
	baseAttr.delta = 0;
	baseAttr.thickness = regionerCfg.raftBaseThickness;

	tick();
	//add interface raft layers in correct order to the beginning of the list
	for (unsigned raftnum = 1; raftnum < regionerCfg.raftLayers; ++raftnum) {
		layer_measure_index_t raftIndex = regionlist[raftnum].layerMeasureId;
		LayerMeasure::LayerAttributes &raftAttr =
				layerMeasure.getLayerAttributes(raftIndex);
		raftAttr.delta = regionerCfg.raftBaseThickness +
				(raftnum - 1) * regionerCfg.raftInterfaceThickness;
		raftAttr.thickness = regionerCfg.raftInterfaceThickness;
		raftAttr.base = baseIndex;

		LayerRegions &raftRegions = regionlist[raftnum];

		raftRegions.supportLoops.push_back(raftLoop);

		tick();
	}

	//add the actual regionlist for the base layer
	//already done in init of regionlist
	//	regionlist.insert(regionlist.begin(), LayerRegions());
	//	LayerRegions &baseRegions = regionlist.front();
	//	baseRegions.supportLoops.push_back(raftLoop);
	//	baseRegions.layerMeasureId = baseIndex;

	//make the first layer of the model relative to the last raft layer
	//already done in init of regionlist
	//	layerMeasure.getLayerAttributes(bottomLayer.getIndex()).base =
	//		regionlist[regionerCfg.raftLayers - 1].layerMeasureId;

	//to the rafts, add the raftloop
	RegionList::iterator iter = regionlist.begin();
	for (size_t raftidx = 0; raftidx < regionerCfg.raftLayers;
			++raftidx, ++iter) {
		iter->supportLoops.push_back(raftLoop);
	}

	tick();
}

void Regioner::insetsForSlice(const LoopList& sliceOutlines,
		std::list<LoopList>& sliceInsets,
		LayerMeasure& layermeasure,
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
			layermeasure.getLayerW(),
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
		RegionList::iterator regionsEnd,
		LayerMeasure& layermeasure) {

	LayerLoops::const_layer_iterator outline = outlinesBegin;
	RegionList::iterator region = regionsBegin;
	while (outline != outlinesEnd && region != regionsEnd) {
		tick();
		const LoopList& currentOutlines = outline->readLoops();

		insetsForSlice(currentOutlines, region->insetLoops,
				layermeasure, NULL);

		++outline;
		++region;
	}
}

void Regioner::flatSurfaces(RegionList::iterator regionsBegin,
		RegionList::iterator regionsEnd,
		const Grid& grid) {
	for (; regionsBegin != regionsEnd; ++regionsBegin) {
		tick();
		//GridRanges currentSurface;
		gridRangesForSlice(regionsBegin->insetLoops, grid,
				regionsBegin->flatSurface);
		//inset supportloops by a fraction of supportmargin
		LoopList insetSupportLoops;
		loopsOffset(insetSupportLoops, regionsBegin->supportLoops, 
				-0.1 * regionerCfg.supportMargin);
		gridRangesForSlice(insetSupportLoops, grid,
				regionsBegin->supportSurface);
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
	grid.trimGridRange(roof, roofLengthCutOff, roofing);
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

void Regioner::support(RegionList::iterator regionsBegin,
		RegionList::iterator regionsEnd, 
		LayerMeasure& /*layermeasure*/) {
	std::list<LoopList> marginsList;
	
	for(RegionList::const_iterator iter = regionsBegin; 
			iter != regionsEnd; 
			++iter) {
		LoopList currentMargins;
		loopsOffset(currentMargins, iter->outlines, 
				regionerCfg.supportMargin);
		marginsList.push_back(currentMargins);
	}
	int layerskip = 1;
	RegionList::iterator above = regionsEnd;
	std::list<LoopList>::const_iterator aboveMargins = marginsList.end();
	--above; //work from the highest layer down
	--aboveMargins;
	
	RegionList::iterator current = above;
	std::list<LoopList>::const_iterator currentMargins = aboveMargins;
	
	while (above != regionsBegin && 
			aboveMargins != marginsList.end()) {
		--current;
		--currentMargins;
		
		LoopList &support = current->supportLoops;

		if (above->supportLoops.empty()) {
			//beginning of new support
			support = *aboveMargins;
		} else {
			//start with a projection of support from the layer above
			support = above->supportLoops;

			//add the outlines of layer above
			loopsUnion(support, *aboveMargins);
		}

		//subtract current outlines from the support loops to keep support
		//from overlapping the object

		//use margins computed up front
		loopsDifference(support, *currentMargins);

		--above;
		--aboveMargins;
		tick();
	}
	
	current = regionsBegin;
	currentMargins = marginsList.begin();
	above = current;
	aboveMargins = currentMargins;
	++above;
	++aboveMargins;
	
	while(current != regionsEnd && 
			currentMargins != marginsList.end()) {
		int curskip = 0;
		for(; curskip < layerskip && 
				above != regionsEnd && aboveMargins != marginsList.end(); 
				++above, ++aboveMargins, ++curskip) {
			loopsDifference(above->supportLoops, current->outlines);
			loopsDifference(current->supportLoops, above->outlines);
		}
		++current;
		++currentMargins;
		above = current;
		aboveMargins = currentMargins;
		++above;
		++aboveMargins;
		tick();
	}
	
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
		for (unsigned int i = 0; i < regionerCfg.floorLayerCount &&
				firstFloor != regionsBegin; ++i)
			--firstFloor;

		RegionList::iterator lastRoof = current;
		for (unsigned int i = 0; i < regionerCfg.roofLayerCount &&
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
		size_t infillSkipCount = (int) (1 / regionerCfg.infillDensity) - 1;

		grid.subSample(surface, infillSkipCount, sparseInfill);
		grid.subSample(current->supportSurface, infillSkipCount,
				current->support);

		grid.gridRangeUnion(current->solid, sparseInfill, current->infill);
	}

}

void Regioner::gridRangesForSlice(const std::list<LoopList>& allInsetsForSlice,
		const Grid& grid,
		GridRanges& surface) {
	if (allInsetsForSlice.size() == 0)
		return;

	const LoopList& innerMostLoops = allInsetsForSlice.back();
	grid.createGridRanges(innerMostLoops, surface);
}

void Regioner::gridRangesForSlice(const LoopList& innerMostLoops,
		const Grid& grid,
		GridRanges & surface) {
	grid.createGridRanges(innerMostLoops, surface);
}



