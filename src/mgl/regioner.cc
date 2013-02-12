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
#include "dump_restore.h"

using namespace mgl;
using namespace std;
using namespace libthing;


Regioner::Regioner(const GrueConfig& grueConf, ProgressBar* progress)
        : Progressive(progress), grueCfg(grueConf) {}

static const Scalar LOOP_ERROR_FUDGE_FACTOR = 0.05;
static const Scalar SUPPORT_FUDGE_FACTOR = 0.02;

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
	layerMeasure.setLayerWidthRatio(grueCfg.get_layerWidthRatio());
	RegionList::iterator firstmodellayer;
	int sliceCount = initRegionList(layerloops, regionlist, layerMeasure,
			firstmodellayer);
	roofLengthCutOff = 0.5 * layerMeasure.getLayerW();

	if (grueCfg.get_doSupport()) {
		initProgress("support", sliceCount*2);
		support(firstmodellayer, regionlist.end(), layerMeasure);
	}

	//optionally inflate if rafts present
	if (grueCfg.get_doRaft() && grueCfg.get_raftLayers() > 0) {
        Scalar raftHeight = grueCfg.get_raftBaseThickness() + 
                grueCfg.get_raftInterfaceThickness() * 
                (grueCfg.get_raftLayers() - 1);
        Scalar raftOutsetOverhead = grueCfg.get_raftOutset() * 4;
        //increase height by raft height
        limits.grow(Point3Type(
                limits.center().x, limits.center().y, 
                limits.zMax + raftHeight));
        //grow sides by raft outset
        limits.inflate(raftOutsetOverhead, raftOutsetOverhead, 0);
	}
    if(grueCfg.get_doSupport()) {
        Scalar supportOverhead = grueCfg.get_supportMargin() * 4;
        limits.inflate(supportOverhead, supportOverhead, 0);
    }

	grid.init(limits, layerMeasure.getLayerW() *
			grueCfg.get_gridSpacingMultiplier());

	if (grueCfg.get_doRaft()) {
		initProgress("rafts", grueCfg.get_raftLayers() + 4);
		rafts(*firstmodellayer, layerMeasure, regionlist);
	}

	//LayerRegions &raftlayer = regionlist.front();

	RegionList::iterator firstModelRegion =
			regionlist.begin() + (grueCfg.get_doRaft() ? 
                grueCfg.get_raftLayers() : 0);

	initProgress("insets", sliceCount);
	insets(layerloops.begin(), layerloops.end(),
			firstModelRegion, regionlist.end(),
			layerMeasure);

    initProgress("spurs", sliceCount);
    spurs(firstModelRegion, regionlist.end(), layerMeasure);

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
	if (grueCfg.get_doRaft()) {
		//insert appropriate number of raft layers at start
		regionlist.insert(regionlist.begin(), grueCfg.get_raftLayers(),
				LayerRegions());
		//for each raft create an entry in layermeasure
		RegionList::iterator iter = regionlist.begin();
		for (size_t raftidx = 0; raftidx < grueCfg.get_raftLayers();
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
		bottomAttribs.delta = grueCfg.get_raftInterfaceThickness() +
				grueCfg.get_raftModelSpacing();
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
	convexSegs.push_back(std::vector<Segment2Type > ());

	for (Loop::finite_cw_iterator iter = convexLoop.clockwiseFinite();
			iter != convexLoop.clockwiseEnd(); ++iter) {

		convexSegs.back().push_back(convexLoop.segmentAfterPoint(iter));
	}

	SegmentTable outsetSegs;
	outsetSegs.push_back(std::vector<Segment2Type > ());

	//outset the convex hull by the configured distance
	ClipperInsetter().inset(convexSegs, -grueCfg.get_raftOutset(), outsetSegs);
	tick();

	Loop raftLoop;
	for (std::vector<Segment2Type>::const_iterator iter =
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
	baseAttr.thickness = grueCfg.get_raftBaseThickness();

	tick();
	//add interface raft layers in correct order to the beginning of the list
	for (unsigned raftnum = 1; raftnum < grueCfg.get_raftLayers(); ++raftnum) {
		layer_measure_index_t raftIndex = regionlist[raftnum].layerMeasureId;
		LayerMeasure::LayerAttributes &raftAttr =
				layerMeasure.getLayerAttributes(raftIndex);
		raftAttr.delta = grueCfg.get_raftBaseThickness() +
				(raftnum - 1) * grueCfg.get_raftInterfaceThickness();
		raftAttr.thickness = grueCfg.get_raftInterfaceThickness();
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
	//		regionlist[grueCfg.get_raftLayers() - 1].layerMeasureId;

	//to the rafts, add the raftloop
	RegionList::iterator iter = regionlist.begin();
	for (size_t raftidx = 0; raftidx < grueCfg.get_raftLayers();
			++raftidx, ++iter) {
		iter->supportLoops.push_back(raftLoop);
	}

	tick();
}

void Regioner::insetsForSlice(const LoopList& sliceOutlines,
							  const LayerMeasure& layermeasure,
							  std::list<LoopList>& sliceInsets,
							  LoopList &interiors) {
	const Scalar base_distance = 0.5 * layermeasure.getLayerW();
    const Scalar adjust_distance = 0.5 * layermeasure.getLayerW();
	for (unsigned int shell = 0; shell < grueCfg.get_nbOfShells(); ++shell) {
		sliceInsets.push_back(LoopList());
		LoopList &shells = sliceInsets.back();

		Scalar distance = base_distance + grueCfg.get_insetDistanceMultiplier()
			* layermeasure.getLayerW() * shell;
		loopsOffset(shells, sliceOutlines, -distance - adjust_distance);
        loopsOffset(shells, shells, adjust_distance);
	}

	// calculate the interior of a loop, temporarily hardcode the distance to
	// half layerW

	loopsOffset(interiors, sliceInsets.back(), -base_distance);
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
//		const SegmentTable & sliceOutlines = outlinesSegments[i];
//		Insets & sliceInsets = insets[i];
//
//		insetsForSlice(sliceOutlines, sliceInsets);
//	}
//}

/**
 @brief Get an upper bound on the area inside a loop (independent of winding).
 @param loop input loop
 @return upper bound on area
 The approximation is done by building an AABBox from the lo0p, and returning
 its area. This will generally overestimate by large amounts, but will 
 correctly return small values for tight, spiky loops
 */
Scalar loopAreaApproximation(const Loop& loop) {
    if(loop.empty())
        return 0;
    Loop::const_finite_cw_iterator iter = loop.clockwiseFinite();
    AABBox box(*iter);
    for(++iter; iter != loop.clockwiseEnd(); ++iter) {
        box.expandTo(*iter);
    }
    return box.area();
}
/**
 @brier erase all loops from @a loops with area less than @a minArea
 @param loops list of loops to be filtered
 @param minArea loops with area less than this are removed
 */
void filterLoops(LoopList& loops, Scalar minArea) {
    std::vector<LoopList::iterator> victims;
    for(LoopList::iterator iter = loops.begin(); 
            iter != loops.end(); 
            ++iter) {
        Scalar currentArea = -1.0;;
        if(iter->size() < 3 || 
                (currentArea = loopAreaApproximation(*iter))< minArea) {
            victims.push_back(iter);
        }
    }
    while(!victims.empty()) {
        loops.erase(victims.back());
        victims.pop_back();
    }
}

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

		insetsForSlice(currentOutlines, layermeasure, region->insetLoops, 
					   region->interiorLoops);
        for(std::list<LoopList>::iterator depthIter = region->insetLoops.begin(); 
                depthIter != region->insetLoops.end(); 
                ++depthIter) {
            smoothCollection(*depthIter, grueCfg.get_coarseness(), 
                    grueCfg.get_directionWeight());
            filterLoops(*depthIter, layermeasure.getLayerW() * 
                    layermeasure.getLayerW());
        }

        if(!region->insetLoops.empty()) {
            loopsOffset(region->interiorLoops, region->insetLoops.back(), 
                    -grueCfg.get_infillShellSpacingMultiplier() * 
                    layermeasure.getLayerWidth(region->layerMeasureId));
        }
		++outline;
		++region;
	}

    tick();
}

void Regioner::flatSurfaces(RegionList::iterator regionsBegin,
		RegionList::iterator regionsEnd,
		const Grid& grid) {
	for (; regionsBegin != regionsEnd; ++regionsBegin) {
		tick();
		//GridRanges currentSurface;

//		gridRangesForSlice(regionsBegin->insetLoops, grid,
//				regionsBegin->flatSurface);
        regionsBegin->flatSurface.yRays.resize(grid.getXValues().size());
        regionsBegin->flatSurface.xRays.resize(grid.getYValues().size());
		//inset supportloops by a fraction of supportmargin
		LoopList insetSupportLoops;
		loopsOffset(insetSupportLoops, regionsBegin->supportLoops, 
				-0.01);
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
		const Grid& //grid
        ) {
    if(regionsBegin == regionsEnd)
        return;
	RegionList::iterator current = regionsBegin;
	RegionList::iterator above = current;
	++above;

	while (above != regionsEnd) {
		tick();
//		const GridRanges & currentSurface = current->flatSurface;
//		const GridRanges & surfaceAbove = above->flatSurface;
//		GridRanges & roofing = current->roofing;
        LoopList& roofLoops = current->roofLoops;

//		GridRanges roof;
//		roofForSlice(currentSurface, surfaceAbove, grid, roof);
//
//		grid.trimGridRange(roof, roofLengthCutOff, roofing);
        LoopList diffResult;
        if(!above->insetLoops.empty()) {
            loopsDifference(diffResult, current->interiorLoops, 
                    above->insetLoops.back());
        }
        //compensate for errors in the difference by a fudge factor
        loopsOffset(roofLoops, diffResult, LOOP_ERROR_FUDGE_FACTOR);

		++current;
		++above;
	}

	tick();
	current->roofing = current->flatSurface;
}

void Regioner::flooring(RegionList::iterator regionsBegin,
		RegionList::iterator regionsEnd,
		const Grid& //grid
        ) {
    if(regionsBegin == regionsEnd)
        return;
	RegionList::iterator below = regionsBegin;
	RegionList::iterator current = below;
	current++;

	while (current != regionsEnd) {
        LoopList& floorLoops = current->floorLoops;
		tick();
//		const GridRanges & currentSurface = current->flatSurface;
//		const GridRanges & surfaceBelow = below->flatSurface;
//		GridRanges & flooring = current->flooring;

//		floorForSlice(currentSurface, surfaceBelow, grid, flooring);
        LoopList diffResult;
        if(!below->insetLoops.empty()) {
            loopsDifference(diffResult, current->interiorLoops, 
                    below->insetLoops.back());
        }
        //compensate for errors in the difference by a fudge factor
        loopsOffset(floorLoops, diffResult, LOOP_ERROR_FUDGE_FACTOR);

		++below;
		++current;
	}

	tick();
//	regionsBegin->flooring = regionsBegin->flatSurface;
    regionsBegin->floorLoops = regionsBegin->interiorLoops;

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
				grueCfg.get_supportMargin());
		marginsList.push_back(currentMargins);
	}
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
        
        //offset aboveMargins by a fudge factor
        //to compensate for error when we subtracted them from layer above
        LoopList aboveMarginsOffset;
        loopsOffset(aboveMarginsOffset, *aboveMargins, SUPPORT_FUDGE_FACTOR);
        
		if (above->supportLoops.empty()) {
			//beginning of new support
			support = aboveMarginsOffset;
		} else {
			//start with a projection of support from the layer above
			support = above->supportLoops;
			//add the outlines of layer above
			loopsUnion(support, aboveMarginsOffset);
		}
        tick();
		//subtract current outlines from the support loops to keep support
		//from overlapping the object

		//use margins computed up front
		loopsDifference(support, *currentMargins);
		--above;
		--aboveMargins;
		//tick();
	}
	current = regionsBegin;
	currentMargins = marginsList.begin();
	above = current;
	aboveMargins = currentMargins;
	++above;
	++aboveMargins;
	
    //this part is the hack that erases support from vertical walls
    //after the fact
	while(current != regionsEnd && 
			currentMargins != marginsList.end()) {
        LoopList currentMarginsOffset;
        loopsOffset(currentMarginsOffset, *currentMargins, 5 * SUPPORT_FUDGE_FACTOR);
        loopsDifference(current->supportLoops, currentMarginsOffset);
		++current;
		++currentMargins;
		tick();
	}
	
}

void Regioner::infills(RegionList::iterator regionsBegin,
		RegionList::iterator regionsEnd,
		const Grid &grid) {
    size_t sequenceNumber = 0;
	for (RegionList::iterator current = regionsBegin;
			current != regionsEnd; ++current, ++sequenceNumber) {

		const GridRanges &surface = current->flatSurface;
		tick();

		// Solids
		//GridRanges combinedSolid;
        LoopList combinedLoops;

//		combinedSolid.xRays.resize(surface.xRays.size());
//		combinedSolid.yRays.resize(surface.yRays.size());

		//TODO: no reason to get bounds separately from the combination

		//find the bounds we will be combinging regions across
		RegionList::iterator firstFloor = current;
        RegionList::iterator floorEnd = current;
		for (unsigned int i = 1; i < grueCfg.get_floorLayerCount() &&
				firstFloor != regionsBegin; ++i)
			--firstFloor;

		RegionList::iterator lastRoof = current;
        RegionList::iterator roofEnd = current;
		for (unsigned int i = 1; i < grueCfg.get_roofLayerCount() &&
				lastRoof != regionsEnd - 1; ++i)
			++lastRoof;
        
        if(grueCfg.get_floorLayerCount() > 0)
            ++floorEnd;
        if(grueCfg.get_roofLayerCount() > 0)
            --roofEnd;
		//combine floors
		for (RegionList::iterator floor = firstFloor;
				floor != floorEnd; ++floor) {
//			GridRanges multiFloor;

//			grid.gridRangeUnion(combinedSolid, floor->flooring, multiFloor);
//			combinedSolid = multiFloor;
            loopsUnion(combinedLoops, floor->floorLoops);
		}

		//combine roofs
		for (RegionList::iterator roof = lastRoof;
				roof != roofEnd; --roof) {
//			GridRanges multiRoof;

//			grid.gridRangeUnion(combinedSolid, roof->roofing, multiRoof);
//			combinedSolid = multiRoof;
            loopsUnion(combinedLoops, roof->roofLoops);
		}

		// solid now contains the combination of combinedSolid regions from
		// multiple slices. We need to extract the perimeter from it

//		grid.gridRangeIntersection(surface, combinedSolid, current->solid);
        loopsIntersection(combinedLoops, current->interiorLoops);
        LoopList sparseLoops;
        loopsDifference(sparseLoops, current->interiorLoops, combinedLoops);
        

		// TODO: move me to the slicer
		GridRanges sparseInfill, sparsePreInfill, solidInfill;
        
        gridRangesForSlice(combinedLoops, grid, solidInfill);
        gridRangesForSlice(sparseLoops, grid, sparsePreInfill);
        
		size_t infillSkipCount = (int) (1 / grueCfg.get_infillDensity()) - 1;

		grid.subSample(sparsePreInfill, infillSkipCount, sparseInfill);
        
        if(grueCfg.get_doSupport() || grueCfg.get_doRaft()) {
            size_t supportSkipCount = 0;
            if(grueCfg.get_doRaft() && sequenceNumber < grueCfg.get_raftLayers()) {
                supportSkipCount = (int) (1 / grueCfg.get_raftDensity()) - 1;
                grid.subSample(current->supportSurface, supportSkipCount,
                        current->support);
            } else if(grueCfg.get_doSupport()) {
                supportSkipCount = (int) (1 / grueCfg.get_supportDensity()) - 1;
                grid.subSample(current->supportSurface, supportSkipCount,
                        current->support);
            }
        }

		//grid.gridRangeUnion(current->solid, sparseInfill, current->infill);
        current->infill.xRays.resize(surface.xRays.size());
        current->infill.yRays.resize(surface.yRays.size());
        
        for(size_t x = 0; x < surface.xRays.size(); ++x) {
            current->infill.xRays[x].insert(
                    current->infill.xRays[x].end(), 
                    sparseInfill.xRays[x].begin(), 
                    sparseInfill.xRays[x].end());
            current->infill.xRays[x].insert(
                    current->infill.xRays[x].end(), 
                    solidInfill.xRays[x].begin(), 
                    solidInfill.xRays[x].end());
        }
        for(size_t y = 0; y < surface.yRays.size(); ++y) {
            current->infill.yRays[y].insert(
                    current->infill.yRays[y].end(), 
                    sparseInfill.yRays[y].begin(), 
                    sparseInfill.yRays[y].end());
            current->infill.yRays[y].insert(
                    current->infill.yRays[y].end(), 
                    solidInfill.yRays[y].begin(), 
                    solidInfill.yRays[y].end());
        }
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


/**
 Spurs code -- eventually this will be in a separate stage of the pipeline
*/

#include "intersection_index.h"
#include "basic_boxlist.h"

#include <algorithm>
#include <math.h>
#include <Eigen/Geometry>

/*******
 * convenience types
 *******/

typedef pair<LineSegment2, LineSegment2> SegmentPair;

typedef Eigen::ParametrizedLine<Scalar, 2> ELine;
typedef Eigen::Vector2d EVector;
typedef Eigen::Hyperplane<Scalar, 2> EHyperplane;

typedef enum {BASE_MIN, BASE_MAX} BaseLimitType;

/**********
 * EigenLib support functions
 **********/

/**
   @brief Convert a Vector2 to an Eigen vector
 */
EVector toEVector(const Vector2 &orig) {
	return EVector(orig.x, orig.y);
}

/**
   @brief Convert an Eigne vector to a Vector2
 */
Vector2 toVector2(const EVector &orig) {
	return Vector2(orig(0), orig(1));
}

/**
   @brief find the intersection point and angle of two Eigen lines
 */
void lineIntersection(const ELine first, const ELine second,
					  EVector &point, Scalar &angle) {
	point = first.intersectionPoint(EHyperplane(second));
	
	EVector firstUnit = first.direction();
	EVector secondUnit = second.direction();

    //Make sure our lines are pointed in generally the same direction
    Scalar dot = firstUnit.dot(secondUnit);
    if (dot < 0) {
        secondUnit = -secondUnit;
        dot = firstUnit.dot(secondUnit);
    }

	angle = acos(dot);
}

/**
   @brief Find the distance between two eigen vector points
*/
Scalar pointDistance(const EVector &a, const EVector &b) {
    return sqrt((b - a)(0) * (b - a)(0) +
                (b - a)(1) * (b - a)(1));
}

/********
 * utility functions
 ********/

/**
   @brief A less comparator for Vector2 objects.  Doesn't put things into any
   real order, but its internally consistent.  Better for uniqueness than actual
   sorting
 */
bool VectorLess(const Vector2 &first, const Vector2 &second) {
	if (first.x < second.x)
		return true;
	else if (first.x > second.x)
		return false;
	else if (first.y < second.y)
		return true;
	else
		return false;
}

/**
   @brief Less comparator for LineSegment2 objects.  Similar to VectorLess.  Use
   for uniqueness.
 */
bool SegLess(const LineSegment2 &first, const LineSegment2 &second) {
	if (VectorLess(first.a, second.a))
		return true;
	else if (VectorLess(second.a, first.a))
		return false;
	else if (VectorLess(first.b, second.b))
		return true;
	else
		return false;
}

/**
   @brief Less comparator for SegmentPairs.  See VectorLess for guidelines
 */
struct SegPairLess {
	bool operator()(const SegmentPair first, const SegmentPair second) {
		if (SegLess(first.first, second.first))
			return true;
		else if (SegLess(second.first, first.first))
			return false;
		else if (SegLess(first.second, second.second))
			return true;
		else 
			return false;
	}
};

typedef set<SegmentPair, SegPairLess> SegmentPairSet;

/**
   @brief Make sure SegmentPairs that are similar are actually the same.
   Makes uniquing pairs easier.
 */
SegmentPair normalizeWalls(const LineSegment2 &first,
						   const LineSegment2 &second) {

	SegmentPair segs;
	if (SegLess(first, second)) {
		segs.first = first;
		segs.second = second;
	}
	else if (SegLess(second, first)) {
		segs.first = second;
		segs.second = first;
	}
    else if (first.a == second.a && first.b == second.b) {
        segs.first = second;
        segs.second = first;
    }
    else {
        // This should never happen
        stringstream msg;
        msg << "don't know which way to order pair:\nfirst.a.x " << first.a.x
            << "\nfirst.a.y " << first.a.y
            << "\nfirst.b.x " << first.b.x
            << "\nfirst.b.y " << first.b.y << endl;
        throw runtime_error(msg.str());
    }

	return segs;
}

/**
   @brief Get the heading vector for a line segment
 */
Vector2 segmentDirection(const LineSegment2 &seg) {
    return seg.a - seg.b;
}
/**
   @brief Get a segment of a given length and starting point that is normal to a
   base segment
   @param orig The segment your perpendicular to
   @param startingPoint the first endpoint of the resulting segment will be equal
   to this. Assumed that this is on the original segment
   @param length length of the normal segment
*/
LineSegment2 getSegmentNormal(const LineSegment2 &orig,
							  const Vector2 &startingPoint,
							  const Scalar length) {
	Vector2 heading = segmentDirection(orig);
	heading.normalise();

	Vector2 normalVector;
	normalVector.x = -heading.y;
	normalVector.y = heading.x;

	normalVector *= length;

	Vector2 endingPoint = startingPoint + normalVector;

	return LineSegment2(startingPoint, endingPoint);
}

/**
   @brief Find the midpoint of a line segment
 */
Vector2 midPoint(const LineSegment2 &seg) {
	return seg.a + ((seg.b - seg.a) * 0.5);
}


/**
   @brief Find the intersection point of two line segments
   @param point Intersection point between two segments
   @return true if the segments intersect
 */
bool segmentIntersection(const LineSegment2 &first,const LineSegment2 &second,
                         Vector2 &point) {
    if (!first.intersects(second))
        return false;

    //use Eigen because they've already figured this one out
    ELine firstline = ELine::Through(toEVector(first.a), toEVector(first.b));
    ELine secondline = ELine::Through(toEVector(second.a), toEVector(second.b));

    point = toVector2(firstline.intersectionPoint(EHyperplane(secondline)));

    //don't need to deal with the situation where they don't intersect yet
    //Will need to deal with this soon
    return true;
} 

/**
   @brief Convenience function for finding intersecting lines in a spacial index
*/
void findIntersecting(SegmentIndex &index, const LineSegment2 &subject,
					  SegmentList &intersecting) {
	SegmentList found;
	index.search(found, LineSegmentFilter(subject));

	for (SegmentList::const_iterator possible = found.begin();
		 possible != found.end(); ++possible) {
		if (possible->intersects(subject)) 
			intersecting.push_back(*possible);
	}
}

/**
   @brief Convenience function for finding intersection poitns in a spacial index
 */
void findIntersectPoints(SegmentIndex &index, const LineSegment2 &subject,
                         PointList &intersections) {
	SegmentList found;
	index.search(found, LineSegmentFilter(subject));

	for (SegmentList::const_iterator possible = found.begin();
		 possible != found.end(); ++possible) {
        Vector2 point;
        if (segmentIntersection(subject, *possible, point))
            intersections.push_back(point);
    }
}

/**
   @brief Distance comparator for points.  To allow you to sort points based on
   their distance to some anchor point.  Initialize with the anchor to compare to
*/
class DistanceCmp {
public:
    DistanceCmp(const Vector2 &anchor) : m_anchor(anchor) {}
    bool operator()(const Vector2 &a, const Vector2 &b) {
        return LineSegment2(m_anchor, a).squaredLength() <
               LineSegment2(m_anchor, b).squaredLength();
    }
private:
    Vector2 m_anchor;
};

/**
   @brief Find a point in a list closest to another point
 */
Vector2 closestPoint(const PointList points, const Vector2 orig) {
    Vector2 closest = points.front();

    for (PointList::const_iterator point = points.begin();
         point != points.end(); ++point) {
        
        if (LineSegment2(orig, *point).squaredLength() <
            LineSegment2(orig, closest).squaredLength()) {
            closest = *point;
        }
    }

    return closest;
}

/**
   @brief Expand a segment by a given amount in both directions
 */
void expandSeg(LineSegment2 &seg, Scalar len) {
    Vector2 dir = segmentDirection(seg);
    Vector2 extra = dir * len;
    seg.a += extra;
    seg.b -= extra;
}

/*******
 * main code for spurs
 *******/

void Regioner::spurLoopsForSlice(const LoopList& sliceOutlines,
								 const std::list<LoopList>& sliceInsets,
								 const LayerMeasure &layermeasure,
 								 std::list<LoopList>& spurLoops) {

    //this is a negligible value to make sure loops overlap
	const Scalar fudgefactor = 0.05;

	// the outer shell is a special case
	std::list<LoopList>::const_iterator inner = sliceInsets.begin();
	LoopList outset;

    if (grueCfg.get_doExternalSpurs()) {
        LoopList tmp;
        loopsOffset(tmp, *inner, -0.5 * layermeasure.getLayerW(),
                false);
        loopsOffset(outset, tmp, fudgefactor + layermeasure.getLayerW(), 
                false);
	
        spurLoops.push_back(LoopList());
        LoopList &outerspurs = spurLoops.back();

        loopsDifference(outerspurs, sliceOutlines, outset);
        //loopsDifference(outerspurs, *inner);
    }

    if (grueCfg.get_doInternalSpurs()) {
        std::list<LoopList>::const_iterator outer = inner;
        ++inner;

        while (inner != sliceInsets.end()) {
            spurLoops.push_back(LoopList());
            LoopList &spurs = spurLoops.back();

            if (inner->size() > 0) {
                outset.clear();
                //take an inner shell and outset it the same amount that it was inset
                loopsOffset(outset, *inner,
                            grueCfg.get_insetDistanceMultiplier() * 
                            layermeasure.getLayerW()
                            + fudgefactor, false);

                //subtract the outset loop from its outer loop, what's left is a spur
                loopsDifference(spurs, *outer, outset);
            }

            outer = inner;
            ++inner;
        }
    }
}

void Regioner::fillSpursForSlice(const std::list<LoopList>& spurLoopsPerShell,
								 const LayerMeasure &layermeasure,
								 std::list<OpenPathList> &spursPerShell) {
	for (std::list<LoopList>::const_iterator spurLoops =
			 spurLoopsPerShell.begin();
		 spurLoops != spurLoopsPerShell.end(); ++spurLoops) {

		spursPerShell.push_back(OpenPathList());
		OpenPathList &spurs = spursPerShell.back();

        //just call fillSpurLoops on every grouping of spurs
		fillSpurLoops(*spurLoops, layermeasure, spurs);
	}
}

/**
   @brief Given a SegmentPair that is assumed parallel, find spans for spurs
 */
SegmentPair completeParallel(const Scalar, const Scalar bottomlen,
                            const SegmentPair &sides) {
    LineSegment2 span = getSegmentNormal(sides.first, sides.first.a,
                                           bottomlen);
    //find an endpoint to one of the segments where you can draw a normal across
    //and intersect the other line
    Vector2 intersection;
    if (segmentIntersection(span, sides.second, intersection))
        span.b = intersection;
    else {
        span = getSegmentNormal(sides.first, sides.first.b, bottomlen);

        if (segmentIntersection(span, sides.second, intersection))
            span.b = intersection;
        else {
            span = getSegmentNormal(sides.second, sides.second.a, bottomlen);

            if (segmentIntersection(span, sides.first, intersection))
                span.b = intersection;
            else {
                span = getSegmentNormal(sides.second, sides.second.b,
                                        bottomlen);

                if (segmentIntersection(span, sides.first, intersection))
                    span.b = intersection;
                else {
                    //shouldn't get here if the pair is valid
                    stringstream msg;
                    msg << "Parallel pair doesn't have valid span:\na.x "
                        << span.a.x
                        << "\na.y " << span.a.y
                        << "\nb.x " << span.b.x
                        << "\nb.y " << span.b.y << endl;
                    throw runtime_error(msg.str());
                }
            }
        }
    }

    //should be checking for paralels too close here

    Vector2 center = midPoint(span);

    Vector2 leftPoint;
    LineSegment2 leftSide;
    Scalar leftDistSq;

    Vector2 rightPoint;
    LineSegment2 rightSide;
    Scalar rightDistSq;

    //find outermost endpoints

    leftPoint = sides.first.a;
    leftDistSq = LineSegment2(center, sides.first.a).squaredLength();
    leftSide = sides.first;
    
    if (LineSegment2(center, sides.second.b).squaredLength() > leftDistSq) {
        leftPoint = sides.second.b;
        leftSide = sides.second;
    }

    rightPoint = sides.first.b;
    rightDistSq = LineSegment2(center, sides.first.b).squaredLength();
    rightSide = sides.first;

    if (LineSegment2(center, sides.second.a).squaredLength() > rightDistSq) {
        rightPoint = sides.second.a;
        rightSide = sides.second;
    }

    Scalar spanlen = span.length();

    //get span segments for outermost points
    return SegmentPair(getSegmentNormal(leftSide, leftPoint, spanlen),
                       getSegmentNormal(rightSide, rightPoint, spanlen));
}

/**
   @brief Given two intersecting lines, form an iscoceles triangle with a specific
   base length
   @param firstUnit Unit vector direction of the first line
   @param secondUnit Unit vector direction of the second line
   @param point Intersection points between the two lines
   @param angle Angle of intersection, passed in to avoid having to recalculate
   @param baseLen Desired length for the triangle base
   @param limit Maximum/minimum side length for the triangle
   @param lt Whether the limit is max or min
 */
LineSegment2 triangleBase(const EVector firstUnit,
						  const EVector secondUnit,
						  const EVector point,
						  const Scalar angle,
						  const Scalar baseLen,
                          const Scalar limit,
                          const BaseLimitType lt) {
	Scalar triangleSide = baseLen / (2 * sin(angle / 2));

    //so that nearly parallel lines don't create lines off into infinity
    if (lt == BASE_MIN && triangleSide < limit)
        triangleSide = limit;
    else if (lt == BASE_MAX && triangleSide > limit)
        triangleSide = limit;

	EVector firstSide = firstUnit * triangleSide;
	EVector secondSide = secondUnit * triangleSide;

	return LineSegment2(toVector2(point + firstSide),
						toVector2(point + secondSide));
}	

/**
   @brief Given two line segments, form a trapezoid with a given small top and
   long bottom.  Dispatches to completeParallel if the lines are reasonably
   parallel.
   @param toplen Length of the shorter parallel side
   @param bottomlen Length of the longer parallel side
   @param sides Line segments trapezoid sides are projecting from
   @param parallels Output, the top and bottom
 */
bool completeTrapezoid(const Scalar toplen, const Scalar bottomlen,
                       const SegmentPair &sides, SegmentPair &parallels) {
    //Eigen gives us all the stuff we need for this operation
    EVector firsta = toEVector(sides.first.a);
    EVector firstb = toEVector(sides.first.b);
    EVector seconda = toEVector(sides.second.a);
    EVector secondb = toEVector(sides.second.b);

    //infinite lines projected through the segments
	ELine firstLine = ELine::Through(firsta, firstb);
	ELine secondLine = ELine::Through(seconda, secondb);

	EVector firstUnit = firstLine.direction();
	EVector secondUnit = secondLine.direction();

    //if parallel within floating point rounding error
    Scalar dirdot = firstUnit.dot(secondUnit);
    if (dirdot > 0)
        dirdot -= 1;
    else
        dirdot +=1;

    if (dirdot < 0)
        dirdot = -dirdot;

    if (dirdot < 0.00001) {
        parallels = completeParallel(toplen, bottomlen, sides);
        return true;
    }

    //make sure lines are pointed in generally the same direction
    if (firstUnit.dot(secondUnit) < 0) {
        secondLine = ELine::Through(secondb, seconda);
        secondUnit = secondLine.direction();
    }

	EVector intersection;
	Scalar angle;
	lineIntersection(firstLine, secondLine, intersection, angle);

    //find an endpoint that isn't the intersection point to use as a comparison
    EVector testpoint = firsta;
    if (testpoint == intersection)
        testpoint = firstb;

    //make sure the direction vectors point away from the intersection
    ELine testline = ELine::Through(testpoint, intersection);
    if (testline.direction().dot(firstUnit) > 0) {
        firstUnit = -firstUnit;
        secondUnit = -secondUnit;
    }

    //get min and max distances from the intersection
    vector<Scalar> endpoints;
    endpoints.push_back(pointDistance(intersection, firsta));
    endpoints.push_back(pointDistance(intersection, firstb));
    endpoints.push_back(pointDistance(intersection, seconda));
    endpoints.push_back(pointDistance(intersection, secondb));

    sort(endpoints.begin(), endpoints.end());

    //Find the parallels from triangles formed with bases of the right length
	parallels.first = triangleBase(firstUnit, secondUnit, intersection,
								   angle, toplen, endpoints.front(),
                                   BASE_MIN);
	parallels.second = triangleBase(firstUnit, secondUnit, intersection,
									angle, bottomlen, endpoints.back(),
                                    BASE_MAX);

    return true;
}


/**
   @brief Given a list of outline segments, find opposite pairs within the right
   range to draw a spur between them
   @param span How close the pairs need to be from each other
   @param segs List of outline segments, unordered
   @param index Pre-build spacial index of segs
   @param walls Output, resulting wall pairs
 */
void findWallPairs(const Scalar span, const SegmentList segs,
				   SegmentIndex &index, SegmentPairSet &walls) {

	for (SegmentList::const_iterator curSeg = segs.begin();
		 curSeg != segs.end(); ++curSeg) {

        //find segments that intersect with a normal drawn from endpoint a
		LineSegment2 normal = getSegmentNormal(*curSeg, curSeg->a, span);
		SegmentList intersecting;

		findIntersecting(index, normal, intersecting);
        for(SegmentList::const_iterator iter = intersecting.begin(); 
                iter != intersecting.end();
                ++iter) {
			//we only care about the closest intersection
			SegmentPair curWalls =
				normalizeWalls(*curSeg, *iter);
			walls.insert(curWalls);
		}

        //find segments that intersect with a normal drawn from endpoint b
		normal = getSegmentNormal(*curSeg, curSeg->b, span);
        intersecting.clear();
        
		findIntersecting(index, normal, intersecting);
		for(SegmentList::const_iterator iter = intersecting.begin(); 
                iter != intersecting.end();
                ++iter) {
			//we only care about the closest intersection
			SegmentPair curWalls =
				normalizeWalls(*curSeg, intersecting.front());
			walls.insert(curWalls);
		}
	}
}

//for debugging
/*void segToSVG(const LineSegment2 seg, const string &color,
  const Scalar xoff, const Scalar yoff);*/

/**
   @brief Given a pair of opposite walls, find the line that bisects them,
   ending at a minimum and maximum distance between the walls
   @param minSpurWidth Minimum distance between walls the line can pass through
   @param maxSpurWidth Maximum distance between walls the line can pass through
   @param walls Walls to bisect
   @param bisect Output, bisection line segment, not set if return is false
   @return true if walls can be bisected
 */
bool bisectWalls(Scalar minSpurWidth, Scalar maxSpurWidth,
                 const SegmentPair &walls, LineSegment2 &bisect) {
    SegmentPair spans;
    if (completeTrapezoid(minSpurWidth, maxSpurWidth, walls, spans)) {
        bisect = LineSegment2(midPoint(spans.first), midPoint(spans.second));

        //for debugging
        /*segToSVG(spans.first, "green", 0, 0);
          segToSVG(spans.second, "green", 0, 0);*/
        return true;
    }
    else
        return false;

}

/**
   @brief Find the portion of a segment that is within a margin of an outline
   @param index Pre-built spacial index of outline segments
   @param margin how far from the outline can a segment be
   @param orig Original segment
   @param cut Output, cut internal segment, not set if return is false
   @return True if some piece of the segment is inside the outline
 */
bool cutInteriorSegment(SegmentIndex &index, const Scalar margin,
                        const LineSegment2 &orig, LineSegment2 &cut) {
    cut = orig;
    SegmentList intersecting;
    findIntersecting(index, cut, intersecting);

    if (intersecting.size() == 0) {
        //segment doesn't intersect the outline, check if its inside or out
        //using the even/odd test
        Vector2 bigpoint(9999, 9999);
        LineSegment2 ray(cut.a, bigpoint);

        findIntersecting(index, ray, intersecting);

        return intersecting.size() % 2 != 0;
    }

    for (SegmentList::const_iterator outline = intersecting.begin();
         outline != intersecting.end(); ++outline) {
        Vector2 intersectPoint;
        if (segmentIntersection(cut, *outline, intersectPoint)) {
            Vector2 normal = segmentDirection(getSegmentNormal(*outline, 
                                                               outline->a, 1));

            //cut the segment in half at the intersection point
            Vector2 direction = segmentDirection(cut);
            direction.normalise();
            
            LineSegment2 left = LineSegment2(intersectPoint,
                                             cut.a);
            LineSegment2 right = LineSegment2(intersectPoint,
                                              cut.b );

            //figure out which half is inside and which is outside
            if (segmentDirection(left).dotProduct(normal) > 0) {
                cut = left;
                cut.a = cut.a + direction * margin;
            }
            else if (segmentDirection(right).dotProduct(normal) > 0) {
                cut = right;
                cut.a = cut.a - direction * margin;
            }
        }
        //if the segment doesn't intersect with the outline we don't need to do
        //anything
    }

    return true;
}

/**
   @brief Check if the endpoint of a segment is too close to an outline
 */
bool isEndPointClose(SegmentIndex &index, const LineSegment2 &segment,
                   const Vector2 &endpoint, const Scalar margin) {
    LineSegment2 left = getSegmentNormal(segment, endpoint, margin);
    LineSegment2 right = getSegmentNormal(segment, endpoint, -margin);
    LineSegment2 tangent(left.b, right.b);

    SegmentList intersecting;

    findIntersecting(index, tangent, intersecting);
    return intersecting.size() > 0;
}

/**
   @brief Holds information about a spur segment being clipped.
   first and last tell wheither the beginning or ending dangling section (from the
   endpoint to the first intersection) are valid.  all tells whether the spur
   segment as a whole is valid.
 */

void Regioner::clipNearOutline(SegmentIndex &outline, SegmentIndex &pieceIndex,
                               SegmentList &pieces, const Scalar margin,
                               PointTable &piecePoints, FlagsList &flagsList) {
    piecePoints.clear();
    flagsList.clear();

    SegmentList newPieces;
    
    // a parallel vector to pieces tracking all the intersection points
    // on each segment 
    for (SegmentList::const_iterator piece = pieces.begin();
         piece != pieces.end(); piece++) {
        piecePoints.push_back(PointList());
        PointList &cur = piecePoints.back();
        findIntersectPoints(pieceIndex, *piece, cur);
    }

    //First add the endpoints to the points list so each points vector has
    //every point on a spur segment, then sort that list
    SegmentList::const_iterator orig = pieces.begin();
    vector<PointList>::iterator points = piecePoints.begin();
    while (orig != pieces.end()) {
        points->push_back(orig->a);
        points->push_back(orig->b);
        sort(points->begin(), points->end(), DistanceCmp(orig->a));

        //end points all start valid
        flagsList.push_back(SpurPieceFlags());

        ++orig;
        ++points;
    }
    
    //next remove endpoints that are too close to the outline and make a new
    //index
    SegmentIndex edgeClipped;
    points = piecePoints.begin();
    FlagsList::iterator flags = flagsList.begin();
    while (points != piecePoints.end()) {
        LineSegment2 seg(points->front(), points->back());
        int numpoints = points->size();

        if (isEndPointClose(outline, seg, seg.a, margin/2)) {
            flags->first = false;
            --numpoints;
            seg.a = *(points->begin() + 1);
        }

        if (isEndPointClose(outline, seg, seg.b, margin/2)) {
            flags->last = false;
            --numpoints;
            seg.b = *(points->end() - 2);
        }

        //if a dangling piece is too short, cut it off.  This will cause
        //pieces that intersect almost at their ends to actually chain at the
        //ends
        if (flags->first) {
            LineSegment2 end(points->front(), *(points->begin() + 1));
            if (end.length() < grueCfg.get_minSpurLength()) {
                flags->first = false;
                --numpoints;
                seg.a = *(points->begin() + 1);
            }
        }

        if (flags->last) {
            LineSegment2 end(*(points->end() - 1), *(points->end() - 2));
            if (end.length() < grueCfg.get_minSpurLength()) {
                flags->last = false;
                --numpoints;
                seg.b = *(points->end() - 2);
            }
        }

        //can't have a segment with less than one point
        if (numpoints < 2)
            flags->all = false;
        else if (seg.length() < grueCfg.get_minSpurLength())
            flags->all = false;
        else {
            expandSeg(seg, grueCfg.get_spurOverlap());
            edgeClipped.insert(seg);
            newPieces.push_back(seg);
        }

        ++points;
        ++flags;
    }

    //update the piece list and index for future runs
    pieceIndex = edgeClipped;
    pieces = newPieces;
}

void Regioner::chainSpurSegments(SegmentIndex &outline, const Scalar margin,
                                 const SegmentList &origPieces,
                                 OpenPathList &chained) {

    //Build an index of the spur segments
    SegmentIndex pieceIndex;

    FlagsList flagsList;
    PointTable piecePoints;
    SegmentList pieces = origPieces;
    
    for (SegmentList::iterator piece = pieces.begin();
        piece != pieces.end(); ++piece) {
        //expandSeg(*piece, grueCfg.get_spurOverlap());
        //this needs to be a different value for mystifying reasons
        *piece = piece->elongate(0.05).prelongate(0.05);
        pieceIndex.insert(*piece);
    }

    //do this multiple times to catch dangling segments after other intersecting
    //segments have been dropped
    for (int count = 0; count < 2; ++count)
        clipNearOutline(outline, pieceIndex, pieces, margin,
                        piecePoints, flagsList);

    //remove remaining endpoints that are too close to another spur
    PointTable::iterator points = piecePoints.begin();
    FlagsList::iterator flags = flagsList.begin();
    while (points != piecePoints.end()) {
        if (flags->all) {
            LineSegment2 seg(points->front(), points->back());
            expandSeg(seg, grueCfg.get_spurOverlap());
            int numpoints = points->size();
            bool changed = false;
            LineSegment2 seg1(flags->first ? points->at(0) : points->at(1), 
                    flags->last ? points->back() : points->at(points->size() - 2));
            LineSegment2 seg2 = seg;
            if (flags->first 
                && isEndPointClose(pieceIndex, seg, seg.a, margin/2)) {
                flags->first = false;
                changed = true;
            }
                

            if (flags->last
                && isEndPointClose(pieceIndex, seg, seg.b, margin/2)) {
                flags->last = false;
                changed = true;
            }

            if (!flags->first) {
                --numpoints;
                seg2.a = points->at(1);
            }
            if (!flags->last) {
                --numpoints;
                seg2.b = points->at(points->size() - 2);
            }

            if (numpoints < 2) {
                flags->all = false;
                pieceIndex.erase(seg1);
            } else if(changed) {
                pieceIndex.erase(seg1);
                pieceIndex.insert(seg2);
            }
        }

        ++points;
        ++flags;
    }

    //finally add fully clipped paths to the list
    points = piecePoints.begin();
    flags = flagsList.begin();
    while (points != piecePoints.end()) {
        if (flags->all) {
            PointList::const_iterator begin = points->begin();
            if (!flags->first)
                ++begin;

            PointList::const_iterator end = points->end();
            if (!flags->last) {
                --end;

            }
            chained.push_back(OpenPath());
            OpenPath &chain = chained.back();
            
            for (PointList::const_iterator point = begin;
                 point != end; ++point)
                chain.appendPoint(*point);
        }

        ++points;
        ++flags;
    }
}

void Regioner::fillSpurLoops(const LoopList &spurLoops,
							 const LayerMeasure &layermeasure,
							 OpenPathList &spurs) {

    Scalar minSpurWidth = grueCfg.get_minSpurWidth();
    Scalar maxSpurWidth = grueCfg.get_maxSpurWidth();
	
	//get loop line segments
	SegmentList segs;
	SegmentIndex index;
	for (LoopList::const_iterator loop = spurLoops.begin();
		 loop != spurLoops.end(); ++loop) {

		for (Loop::const_finite_cw_iterator pn = loop->clockwiseFinite();
			 pn != loop->clockwiseEnd(); ++pn) {
            LineSegment2 seg;
            seg = loop->segmentAfterPoint(pn);

            if (seg.squaredLength() > 0.00001) {
                segs.push_back(seg);
                index.insert(seg);
            }
		}

	}

	//find wall pairs
	SegmentPairSet allWalls;
	findWallPairs(maxSpurWidth, segs, index, allWalls);

	SegmentList pieces;

	// Bisect each pair
	for (SegmentPairSet::const_iterator walls = allWalls.begin();
		 walls != allWalls.end(); ++walls) {
		LineSegment2 bisect;
        if (bisectWalls(minSpurWidth, maxSpurWidth, *walls, bisect))
            pieces.push_back(bisect);
	}

    //cut each segment so it fits in the outline
    SegmentList interiorPieces;
    for (SegmentList::iterator piece = pieces.begin();
         piece != pieces.end(); ++piece) {
        LineSegment2 cutpiece;
        if (cutInteriorSegment(index, minSpurWidth / 2, *piece, cutpiece))
            interiorPieces.push_back(cutpiece);
    }


    chainSpurSegments(index, minSpurWidth, interiorPieces, spurs);

    //for testing without segments chained
    /*for (SegmentList::const_iterator piece = pieces.begin();
         piece != pieces.end(); ++piece) {
        spurs.push_back(OpenPath());
        spurs.back().appendPoint(piece->a);
        spurs.back().appendPoint(piece->b);
        }*/
}

void Regioner::spurs(RegionList::iterator regionsBegin,
                     RegionList::iterator regionsEnd,
                     LayerMeasure &layermeasure) {
    for (RegionList::iterator region = regionsBegin;
         region != regionsEnd; ++region) {
        tick();

        //get spur loops, then fill them
        spurLoopsForSlice(region->outlines, region->insetLoops,
                           layermeasure, region->spurLoops);
        fillSpursForSlice(region->spurLoops, layermeasure, region->spurs);
    }

    tick();
}



