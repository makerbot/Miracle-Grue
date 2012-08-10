/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef PATHER_H_
#define PATHER_H_


#include "insets.h"
#include "regioner.h"
#include "loop_path.h"
#include "labeled_path.h"

#include <list>

namespace mgl {

class PatherConfig {
public:
	PatherConfig() 
			: doGraphOptimization(true), 
			coarseness(0.05), 
			directionWeight(0.0){}
	bool doGraphOptimization;
	Scalar coarseness;
	Scalar directionWeight;
};

typedef std::vector<LoopList> InsetVector; // TODO: make this a smarter object

// slice data for an extruder
class ExtruderConfig {
public:
	unsigned int defaultExtruder;
};


class ExtruderSlice {
public:

	Polygons boundary; // boundary loops for areas of this slice of a print.
	Polygons infills; // list of all lines that create infill for this layer
	PolygonsGroup insetLoopsList; /// a list, each entry of which is a Polygons
	/// object. Each inset[i] is all shell polygons
	/// for the matching loops[i] boundary for this layer
};

class LayerPaths{
public:
	class Layer;
	typedef std::list<Layer> LayerList;
	typedef LayerList::iterator layer_iterator;
	typedef LayerList::const_iterator const_layer_iterator;
	
	class Layer{
	public:
		class ExtruderLayer;
		typedef std::list<ExtruderLayer> ExtruderList;
		typedef ExtruderList::iterator extruder_iterator;
		typedef ExtruderList::const_iterator const_extruder_iterator;
		class ExtruderLayer{
		public:
			typedef std::list<OpenPathList> InsetList;
			typedef std::list<OpenPath> InfillList;
			typedef std::list<OpenPath> OutlineList;
			typedef std::list<LabeledOpenPath> LabeledPathList;
			typedef InsetList::iterator inset_iterator;
			typedef InfillList::iterator infill_iterator;
			typedef OutlineList::iterator outline_iterator;
			typedef LabeledPathList::iterator path_iterator;
			typedef InsetList::const_iterator const_inset_iterator;
			typedef InfillList::const_iterator const_infill_iterator;
			typedef OutlineList::const_iterator const_outline_iterator;
			typedef LabeledPathList::const_iterator const_path_iterator;
			ExtruderLayer(size_t exId = 0) : extruderId(exId) {}
			InsetList insetPaths;
			InfillList infillPaths;
			InfillList supportPaths;
			OutlineList outlinePaths;
			LabeledPathList paths;
			size_t extruderId;
		};
		Layer(Scalar z, Scalar layerh, Scalar layerw, layer_measure_index_t mind)
				: layerZ(z), layerHeight(layerh), layerW(layerw), 
				measure_index(mind) {}
		Layer(layer_measure_index_t mind = 0) 
				: layerZ(0), layerHeight(0.27), layerW(0.43), measure_index(mind) {}
		ExtruderList extruders;
		Scalar layerZ;		//vertical coordinate
		Scalar layerHeight;	//thickness
		Scalar layerW;		//width of filament
		layer_measure_index_t measure_index;
	};
	
	layer_iterator begin();
	const_layer_iterator begin() const;
	layer_iterator end();
	const_layer_iterator end() const;
	void push_back(const Layer& value);
	void push_front(const Layer& value);
	void pop_back();
	void pop_front();
	layer_iterator insert(layer_iterator at, const Layer& value);
	layer_iterator erase(layer_iterator at);
	layer_iterator erase(layer_iterator from, layer_iterator to);
	bool empty() const;
	size_t layerCount() const;
	Layer& back();
private:
	LayerList layers;
};

::std::ostream& operator<<(::std::ostream& os, const ExtruderSlice& x);

typedef std::vector<ExtruderSlice > ExtruderSlices;

/// The Slice data is contains polygons
/// for each extruder, for a slice.
/// there are multiple polygons for each extruder
class SliceData
{
private:
	Scalar zHeight;
	size_t index;

public:
	ExtruderSlices extruderSlices;

	/// @param inHeight: z height of this layer. Middle of the specified layer
	/// @param inIndex: layer number in this  model, positive in the 'up' direction
	SliceData(Scalar inHeight=0, size_t inIndex=0):zHeight(inHeight), index(inIndex)
	{
	}

	/// Updates position of slice in a model
	/// @param inHeight: z height of this layer. Middle of the specified layer
	/// @param inIndex: layer number in this  model, positive in the 'up' direction
	void updatePosition(Scalar inHeight,size_t inIndex){
		zHeight = inHeight;
		index = inIndex ;
	}

	Scalar getZHeight() const { return zHeight;}
	size_t getIndex()const  { return index;}


};

::std::ostream& operator<<(::std::ostream& os, const SliceData& x);


class Pather : public Progressive
{
private:
	PatherConfig patherCfg;

public:


	Pather(const PatherConfig& pCfg, ProgressBar * progress = NULL);


	void generatePaths(const ExtruderConfig &extruderCfg,
					   const RegionList &skeleton,
					   const LayerMeasure &layerMeasure,
					   const Grid &grid,
					   LayerPaths &slices,
					   int sfirstSliceIdx=-1,
					   int slastSliceIdx=-1);


	void outlines(const LoopList& outline_loops,
				  LoopPathList &boundary_paths);

	void insets(const std::list<LoopList>& insetsForSlice,
				std::list<LoopPathList> &insetPaths);

	void infills(const GridRanges &infillRanges,
				 const Grid &grid,
				 const LoopList& outlines,
				 bool direction,
				 OpenPathList &infills);
	
	void directionalCoarsenessCleanup(
		LayerPaths::Layer::ExtruderLayer::LabeledPathList& labeledPaths);
	void directionalCoarsenessCleanup(LabeledOpenPath& labeledPath);
	

};



}

#endif
