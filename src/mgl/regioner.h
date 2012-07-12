
/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

 */


#ifndef REGIONER_H_
#define REGIONER_H_ (1)

#include "slicer.h"
#include "slicer_loops.h"
#include "loop_path.h"

namespace mgl {

class LayerRegions {
public:
	std::list<LoopList> insetLoops;
	LoopList supportLoops;
	LoopList interiorLoops;

	GridRanges flatSurface; // # number of slices + roofCount * 2
	GridRanges roofing;
	GridRanges flooring;
	GridRanges support;

	GridRanges infill;

	GridRanges solid;
	GridRanges sparse;

	size_t layerMeasureId;
};

typedef std::vector<LayerRegions> RegionList;

//// Class to calculate regions of a model
///

class Regioner : public Progressive {
	Scalar roofLengthCutOff;
public:
	SlicerConfig slicerCfg;


	Regioner(const SlicerConfig &slicerCfg,
			ProgressBar *progress = NULL);

	void generateSkeleton(const LayerLoops& layerloops, RegionList &regions);

	size_t initRegionList(const LayerLoops& layerloops,
						  RegionList &regionlist);

	void insetsForSlice(const libthing::SegmentTable &sliceOutlines,
			libthing::Insets &sliceInsets,
			const char* scadFile = NULL);
	void insetsForSlice(const LoopList& sliceOutlines,
			std::list<LoopList>& sliceInsets,
			const char* scadFile = NULL);

	void insets(const LayerLoops::const_layer_iterator outlinesBegin,
				const LayerLoops::const_layer_iterator outlinesEnd,
				RegionList::iterator regionsBegin,
				RegionList::iterator regionsEnd);

	void flatSurfaces(RegionList::iterator regionsBegin,
					  RegionList::iterator regionsEnd,
					  const Grid& grid);

	void floorForSlice(const GridRanges & currentSurface, 
					   const GridRanges & surfaceBelow, 
					   const Grid & grid,
					   GridRanges & flooring);

	void roofing(RegionList::iterator regionsBegin,
				 RegionList::iterator regionsEnd,
				 const Grid& grid);

	void roofForSlice(const GridRanges & currentSurface, 
			const GridRanges & surfaceAbove, 
			const Grid & grid, 
			GridRanges & roofing);

	void flooring(RegionList::iterator regionsBegin,
				  RegionList::iterator regionsEnd,
				  const Grid &grid);

	void infills(RegionList::iterator regionsBegin,
				 RegionList::iterator regionsEnd,
				 const Grid &grid);

	void gridRangesForSlice(const std::list<LoopList>& allInsetsForSlice, 
			const Grid& grid, 
			GridRanges& surface);

private:




};

}

#endif /* SKELETOR_H_ */
