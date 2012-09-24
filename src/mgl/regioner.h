
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

class RegionerConfig {
public:
	RegionerConfig() 
			: tubeSpacing(1),
			//angle(1.570796326794897),
			nbOfShells(2),
			layerWidthRatio(1.7),
			insetDistanceMultiplier(0.9),
			roofLayerCount(0),
			floorLayerCount(0),
			gridSpacingMultiplier(0.95), 
			raftLayers(0), 
			raftBaseThickness(0.5), 
			raftInterfaceThickness(0.27), 
			raftOutset(6),
			raftModelSpacing(0),
			doSupport(false),
			supportMargin(1.0) {}

	// These are relevant to regioner
	Scalar tubeSpacing; //< distance in between infill (mm)
	Scalar angle; //< angle of infill
	unsigned int nbOfShells; //< shell count of model
	Scalar layerWidthRatio; //< TBD
	Scalar insetDistanceMultiplier; //< TBD
	unsigned int roofLayerCount; // number of solid layers for roofs
	unsigned int floorLayerCount; // number of solid layers for floors
	//unsigned int infillSkipCount; //< TBD
	Scalar infillDensity; // the density of the infill patterns (0 to 1)
	Scalar gridSpacingMultiplier;	// interference between 2 grid lines 
									//( 0 to 1, for adhesion)
    bool doRaft;
	unsigned int raftLayers; //< nb of raft layers
	Scalar raftBaseThickness; //< thickness of first raft layer (mm)
	Scalar raftInterfaceThickness; //< thickness of other raft layers (mm)
	Scalar raftOutset; //< How far to outset rafts (mm)
	Scalar raftDensity;
	Scalar raftModelSpacing; //< Distance between top raft and model

	bool doSupport;  //< do we generate support
	Scalar supportMargin; //< distance between side wall and support
	Scalar supportDensity;
};

class LayerRegions {
public:
	LoopList outlines;
	std::list<LoopList> insetLoops;
	LoopList supportLoops;
	LoopList interiorLoops;

	GridRanges flatSurface; // # number of slices + roofCount * 2
	GridRanges supportSurface; //flat surface outside of the actual object

	GridRanges roofing;
	GridRanges flooring;
	GridRanges support;

	GridRanges infill;

	GridRanges solid;
	GridRanges sparse;

	layer_measure_size_t layerMeasureId;
};

typedef std::vector<LayerRegions> RegionList;

//// Class to calculate regions of a model
///

class Regioner : public Progressive {
	Scalar roofLengthCutOff;
public:
	RegionerConfig regionerCfg;

	Regioner(const RegionerConfig &regionerCfg, 
			ProgressBar *progress = NULL);

	void generateSkeleton(const LayerLoops& layerloops, 
						  LayerMeasure &layerMeasure, 
						  RegionList &regionlist, 
						  Limits& limits, //updated to reflect outsets
						  Grid& grid);	//initialized here

	size_t initRegionList(const LayerLoops& layerloops,
						  RegionList &regionlist, 
						  LayerMeasure& layermeasure,
						  RegionList::iterator& firstmodellayer);

	void rafts(const LayerRegions& bottomLayer,
			   LayerMeasure &layerMeasure,
			   RegionList &regionlist);

	void insetsForSlice(const SegmentTable &sliceOutlines,
			InsetsCollection &sliceInsets,
			const char* scadFile = NULL);
	void insetsForSlice(const LoopList& sliceOutlines,
			std::list<LoopList>& sliceInsets,
			LayerMeasure& layermeasure, 
			const char* scadFile = NULL);

	void insets(const LayerLoops::const_layer_iterator outlinesBegin,
				const LayerLoops::const_layer_iterator outlinesEnd,
				RegionList::iterator regionsBegin,
				RegionList::iterator regionsEnd,
				LayerMeasure& layermeasure);

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

	void support(RegionList::iterator regionsBegin,
				 RegionList::iterator regionsEnd ,
				 LayerMeasure& layermeasure);


	void infills(RegionList::iterator regionsBegin,
				 RegionList::iterator regionsEnd,
				 const Grid &grid);

	void gridRangesForSlice(const std::list<LoopList>& allInsetsForSlice, 
							const Grid& grid, 
							GridRanges& surface);
	void gridRangesForSlice(const LoopList& innerMostLoops, 
							const Grid& grid, 
							GridRanges& surface);

private:




};

}

#endif /* SKELETOR_H_ */
