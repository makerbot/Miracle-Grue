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
#include "pather_optimizer_fastgraph.h"

namespace mgl {
using namespace std;

Pather::Pather(const PatherConfig& pCfg, ProgressBar* progress) 
		: Progressive(progress), patherCfg(pCfg) {}
Pather::Pather(const GrueConfig& grueConf, ProgressBar* progress)
        : Progressive(progress) {
    patherCfg.doGraphOptimization = grueConf.get_doGraphOptimization();
    patherCfg.coarseness = grueConf.get_coarseness();
    patherCfg.directionWeight = grueConf.get_directionWeight();
}

void Pather::generatePaths(const GrueConfig& grueCfg,
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
    
    abstract_optimizer* optimizer = NULL;
    if(grueCfg.get_doGraphOptimization()) {
        optimizer = new pather_optimizer_fastgraph(grueCfg);
    } else {
        optimizer = new pather_optimizer();
    }

	for (RegionList::const_iterator layerRegions = skeleton.begin();
			layerRegions != skeleton.end(); ++layerRegions) {
		tick();
        try {
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
				LayerPaths::Layer::ExtruderLayer(grueCfg.get_defaultExtruder()));
		LayerPaths::Layer::ExtruderLayer& extruderlayer =
				lp_layer.extruders.back();
		
		
		optimizer->clearBoundaries();
        optimizer->clearPaths();

		const std::list<LoopList>& insetLoops = layerRegions->insetLoops;
		
        if(grueCfg.get_doOutlines()) {
            for(LoopList::const_iterator iter = layerRegions->outlines.begin(); 
                    iter != layerRegions->outlines.end(); 
                    ++iter) {
                const LoopPath outlinePath(*iter, iter->clockwise(), 
                        iter->counterClockwise());
                extruderlayer.paths.push_back(PathLabel(PathLabel::TYP_OUTLINE, 
                        PathLabel::OWN_MODEL));
                OpenPath& path = extruderlayer.paths.back().myPath;
                for(LoopPath::const_iterator pointIter = outlinePath.fromStart(); 
                        pointIter != outlinePath.end(); 
                        ++pointIter) {
                    path.appendPoint(*pointIter);
                }
            }
            for(LoopList::const_iterator iter = layerRegions->supportLoops.begin(); 
                    iter != layerRegions->supportLoops.end(); 
                    ++iter) {
                const LoopPath outlinePath(*iter, iter->clockwise(), 
                        iter->counterClockwise());
                extruderlayer.paths.push_back(PathLabel(PathLabel::TYP_OUTLINE, 
                        PathLabel::OWN_SUPPORT));
                OpenPath& path = extruderlayer.paths.back().myPath;
                for(LoopPath::const_iterator pointIter = outlinePath.fromStart(); 
                        pointIter != outlinePath.end(); 
                        ++pointIter) {
                    path.appendPoint(*pointIter);
                }
            }
        }
		
		optimizer->addBoundaries(layerRegions->outlines);	
        
		if(grueCfg.get_doInsets()) {
            int currentShell = LayerPaths::Layer::ExtruderLayer::INSET_LABEL_VALUE;
            int shellSequence = 0;
            for(std::list<LoopList>::const_iterator listIter = insetLoops.begin(); 
                    listIter != insetLoops.end(); 
                    ++listIter, ++shellSequence) {
                int shellVal = currentShell;
//                shellVal = shellSequence !=0 && grueCfg.get_nbOfShells() > 1 ? 
//                        shellVal : 
//                        LayerPaths::Layer::ExtruderLayer::OUTLINE_LABEL_VALUE;
                optimizer->addPaths(*listIter, 
                        PathLabel(PathLabel::TYP_INSET, 
                        PathLabel::OWN_MODEL, shellVal));
                ++currentShell;
            }
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
		
        if(grueCfg.get_doInfills()) {
            optimizer->addPaths(infillPaths, PathLabel(PathLabel::TYP_INFILL, 
                    PathLabel::OWN_MODEL, 
                    LayerPaths::Layer::ExtruderLayer::INFILL_LABEL_VALUE));
        }
		
		optimizer->optimize(preoptimized);
		
		optimizer->clearBoundaries();
		optimizer->clearPaths();
		
        if(grueCfg.get_doRaft() || grueCfg.get_doSupport()) {
            LoopList outsetSupportLoops;
            loopsOffset(outsetSupportLoops, layerRegions->supportLoops, 
                    0.01);
            optimizer->addBoundaries(outsetSupportLoops);

            optimizer->addPaths(supportPaths, PathLabel(PathLabel::TYP_INFILL, 
                    PathLabel::OWN_SUPPORT, 0));


            optimizer->optimize(presupport); 
        }
		
        extruderlayer.paths.insert(extruderlayer.paths.end(), 
                preoptimized.begin(), preoptimized.end());
        extruderlayer.paths.insert(extruderlayer.paths.end(), 
                presupport.begin(), presupport.end());
		//directionalCoarsenessCleanup(extruderlayer.paths);

//		cout << currentSlice << ": \t" << layerMeasure.getLayerPosition(
//				layerRegions->layerMeasureId) << endl;
        }catch (const std::exception& our) {
            std::cout << "Error " << our.what() << " on layer " << 
                    currentSlice << std::endl;
        }
		++currentSlice;
	}
    delete optimizer;
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
        PointType currentPoint = *current;
        PointType landingPoint = currentPoint;
		++last2;
		bool addPoint = true;
        try {
            PointType unit = PointType(*last1 - *last2).unit();
            Scalar component = (currentPoint - *last1).dotProduct(unit);
            Scalar deviation = abs((currentPoint - *last1).crossProduct(unit));
            landingPoint = *last1 + unit*component;

            cumulativeError += deviation;

            addPoint = cumulativeError > patherCfg.coarseness;
        } catch(libthing::Exception mixup) {
            //we expect this to be something like a bad normalization
            Log::severe() << "ERROR: " << mixup.what() << std::endl;
        }
		
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
