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
#include "pather_optimizer_graph.h"

namespace mgl {
using namespace std;

Pather::Pather(const PatherConfig& pCfg, ProgressBar* progress) 
		: Progressive(progress), patherCfg(pCfg) {}

void Pather::generatePaths(const ExtruderConfig &extruderCfg,
		const RegionList &skeleton,
		const LayerMeasure &layerMeasure,
		const Grid &grid,
		LayerPaths &layerpaths,
		int sfirstSliceIdx, // =-1
		int slastSliceIdx) //
{
	size_t firstSliceIdx = 0;
	size_t lastSliceIdx = INT_MAX;

	if (sfirstSliceIdx > 0) {
		firstSliceIdx = (size_t) sfirstSliceIdx;
	}

	if (slastSliceIdx > 0) {
		lastSliceIdx = (size_t) slastSliceIdx;
	}

	bool direction = false;
	unsigned int currentSlice = 0;

	initProgress("Path generation", skeleton.size());

	for (RegionList::const_iterator layerRegions = skeleton.begin();
			layerRegions != skeleton.end(); ++layerRegions) {
		tick();

		if (currentSlice < firstSliceIdx) continue;
		if (currentSlice > lastSliceIdx) break;

		direction = !direction;
		const layer_measure_index_t layerMeasureId =
				layerRegions->layerMeasureId;

		//adding these should be handled in gcoder
		const Scalar z = layerMeasure.getLayerPosition(layerMeasureId);
		const Scalar h = layerMeasure.getLayerThickness(layerMeasureId);
		const Scalar w = layerMeasure.getLayerWidth(layerMeasureId);

		layerpaths.push_back(LayerPaths::Layer(z, h, w, layerMeasureId));

		LayerPaths::Layer& lp_layer = layerpaths.back();

		//TODO: this only handles the case where the user specifies the extruder
		// it does not handle a dualstrusion print
		lp_layer.extruders.push_back(
				LayerPaths::Layer::ExtruderLayer(extruderCfg.defaultExtruder));
		LayerPaths::Layer::ExtruderLayer& extruderlayer =
				lp_layer.extruders.back();
		
		
		pather_optimizer preoptimizer;
		pather_optimizer_graph optimizer;
		//optimizer.linkPaths = false;

		const std::list<LoopList>& insetLoops = layerRegions->insetLoops;
		
		preoptimizer.addPaths(layerRegions->outlines, 
				PathLabel(PathLabel::TYP_OUTLINE, PathLabel::OWN_MODEL));
		preoptimizer.addPaths(layerRegions->supportLoops, 
				PathLabel(PathLabel::TYP_OUTLINE, PathLabel::OWN_SUPPORT));
		preoptimizer.optimize(extruderlayer.outlinePaths);
		
		preoptimizer.addBoundaries(layerRegions->outlines);	
		
		int currentShell = 10;
		for(std::list<LoopList>::const_iterator listIter = insetLoops.begin(); 
				listIter != insetLoops.end(); 
				++listIter) {
			preoptimizer.addPaths(*listIter, 
					PathLabel(PathLabel::TYP_INSET, 
					PathLabel::OWN_MODEL, currentShell));
			++currentShell;
		}

		const GridRanges& infillRanges = layerRegions->infill;
		const GridRanges& supportRanges = layerRegions->support;

		const std::vector<Scalar>& values = 
				!direction ? grid.getXValues() : grid.getYValues();
		axis_e axis = direction ? X_AXIS : Y_AXIS;
		
		
		OpenPathList infillPaths;
		OpenPathList supportPaths;
		grid.gridRangesToOpenPaths(
				direction ? infillRanges.xRays : infillRanges.yRays,  
				values, 
				axis, 
				infillPaths);
		
		std::list<LabeledOpenPath> preoptimized;
		std::list<LabeledOpenPath> presupport;
		
		grid.gridRangesToOpenPaths(
				direction ? supportRanges.xRays : supportRanges.yRays, 
				values, 
				axis, 
				supportPaths);
		
		preoptimizer.addPaths(infillPaths, PathLabel(PathLabel::TYP_INFILL, 
				PathLabel::OWN_MODEL, 1));
		
		preoptimizer.optimize(preoptimized);
		
		preoptimizer.clearBoundaries();
		preoptimizer.clearPaths();
		
		preoptimizer.addBoundaries(layerRegions->supportLoops);
		
		preoptimizer.addPaths(supportPaths, PathLabel(PathLabel::TYP_INFILL, 
				PathLabel::OWN_SUPPORT, 0));
		
		preoptimizer.optimize(presupport);
		
		if(patherCfg.doGraphOptimization) {
			//run graph optimizations
			std::list<LabeledOpenPath> resultModel;
			std::list<LabeledOpenPath> resultSupport;
			optimizer.addBoundaries(layerRegions->outlines);
			optimizer.addPaths(preoptimized);
			optimizer.optimize(resultModel);
			optimizer.clearPaths();
			optimizer.clearBoundaries();
			optimizer.addBoundaries(layerRegions->supportLoops);
			optimizer.addPaths(presupport);
			optimizer.optimize(resultSupport);
			
			extruderlayer.paths.insert(extruderlayer.paths.end(), 
					resultModel.begin(), resultModel.end());
			
			extruderlayer.paths.insert(extruderlayer.paths.end(), 
					resultSupport.begin(), resultSupport.end());
		} else {
			//don't run graph optimizations
			//use naive result instead
			extruderlayer.paths.insert(extruderlayer.paths.end(), 
					preoptimized.begin(), preoptimized.end());
			extruderlayer.paths.insert(extruderlayer.paths.end(), 
					presupport.begin(), presupport.end());
		}
		directionalCoarsenessCleanup(extruderlayer.paths);

//		cout << currentSlice << ": \t" << layerMeasure.getLayerPosition(
//				layerRegions->layerMeasureId) << endl;

		++currentSlice;
	}
}

void Pather::outlines(const LoopList& outline_loops,
		LoopPathList &boundary_paths) {
	//using a indeterminate start point for the beginning of the LoopPathList
	//as that's what the old Polygon logic did

	for (LoopList::const_iterator i = outline_loops.begin();
			i != outline_loops.end(); ++i) {
		boundary_paths.push_back(LoopPath(*i, i->clockwise(),
				i->counterClockwise()));
	}
}

void Pather::insets(const list<LoopList>& inset_loops,
		list<LoopPathList> &inset_paths) {
	std::list<const Loop*> flat_insets;
	for (list<LoopList>::const_iterator i = inset_loops.begin();
			i != inset_loops.end(); ++i) {

//		inset_paths.push_back(LoopPathList());
//		LoopPathList& lp_list = inset_paths.back();

		for (LoopList::const_iterator j = i->begin(); j != i->end(); ++j) {
//			lp_list.push_back(LoopPath(*j, j->clockwise(),
//					j->counterClockwise()));
			flat_insets.push_back(&*j);
		}
	}
	inset_paths.push_back(LoopPathList());
	LoopPathList& onlyList = inset_paths.back();
	while(!flat_insets.empty()) {
		if(onlyList.empty()) {
			onlyList.push_back(LoopPath(*flat_insets.front(), 
					flat_insets.front()->clockwise(), 
					flat_insets.front()->counterClockwise(
					*(flat_insets.front()->clockwise()))));
			flat_insets.pop_front();
		} else {
			PointType current_exit = *onlyList.back().fromEnd();
			std::list<const Loop*>::iterator closestLoop = flat_insets.begin();
			Loop::entry_iterator closestEntry = flat_insets.front()->entryBegin();
			Scalar closestDistance = (closestEntry->getPoint() - 
					current_exit).magnitude();
			//find the closest entry
			for(std::list<const Loop*>::iterator loopIter = flat_insets.begin(); 
					loopIter != flat_insets.end(); 
					++loopIter) {
				for(Loop::entry_iterator entryIter = (*loopIter)->entryBegin(); 
						entryIter != (*loopIter)->entryEnd(); 
						++entryIter) {
					Scalar distance = (entryIter->getPoint() - 
							current_exit).magnitude();
					if(distance < closestDistance) {
						closestLoop = loopIter;
						closestEntry = entryIter;
						closestDistance = distance;
					}
				}
			}
			//add its loopPath
			onlyList.push_back(LoopPath(*(*closestLoop), 
					(*closestLoop)->clockwise(*closestEntry), 
					(*closestLoop)->counterClockwise(*closestEntry)));
			//remove from list
			flat_insets.erase(closestLoop);
		}
	}
}

void Pather::infills(const GridRanges &infillRanges,
		const Grid &grid,
		const LoopList &outlines,
		const bool direction,
		OpenPathList &infills) {
	grid.pathsFromRanges(infillRanges, outlines, direction, infills);
}

void Pather::directionalCoarsenessCleanup(
		LayerPaths::Layer::ExtruderLayer::LabeledPathList& labeledPaths){
	typedef LayerPaths::Layer::ExtruderLayer::LabeledPathList Paths;
	for(Paths::iterator iter = labeledPaths.begin(); 
			iter != labeledPaths.end(); 
			++iter) {
		directionalCoarsenessCleanup(*iter);
	}
}

void Pather::directionalCoarsenessCleanup(LabeledOpenPath& labeledPath) {
	if(patherCfg.coarseness == 0)
		return;
	OpenPath& path = labeledPath.myPath;
	if(path.size() < 3)
		return;
	OpenPath cleanPath;
	OpenPath::iterator current;
	current = path.fromStart();
	//insert the first two points
	cleanPath.appendPoint(*(current++));
	cleanPath.appendPoint(*(current++));
	Scalar cumulativeError = 0.0;
	for(; current != path.end(); ++current) {
		OpenPath::reverse_iterator last1 = cleanPath.fromEnd();
		OpenPath::reverse_iterator last2 = cleanPath.fromEnd();
		++last2;
		bool addPoint = true;
		
		PointType currentPoint = *current;
		PointType unit = PointType(*last1 - *last2).unit();
		Scalar component = (currentPoint - *last1).dotProduct(unit);
		Scalar deviation = abs((currentPoint - *last1).crossProduct(unit));
		PointType landingPoint = *last1 + unit*component;
		
		cumulativeError += deviation;
		
		addPoint = cumulativeError > patherCfg.coarseness;
		
		if(addPoint) {
			cleanPath.appendPoint(currentPoint);
			cumulativeError = 0;
		} else {
			*last1 = landingPoint * (1.0 - patherCfg.directionWeight) + 
					currentPoint * patherCfg.directionWeight;
		}
	}
	path = cleanPath;
}



}
