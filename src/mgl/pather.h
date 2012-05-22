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

namespace mgl
{



// slice data for an extruder
class ExtruderSlice
{
public:

	Polygons boundary;  // boundary loops for areas of this slice of a print.
	Polygons infills; // list of all lines that create infill for this layer

	PolygonsGroup  insetLoopsList;  /// a list, each entry of which is a Polygons
							/// object. Each inset[i] is all shell polygons
							/// for the matching loops[i] boundary for this layer

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

public:


	Pather(ProgressBar * progress = NULL)
		:Progressive(progress)
	{
	}


	void generatePaths(const Tomograph &tomograph,
						const Regions &skeleton,
						std::vector<SliceData> &slices,
						int sfirstSliceIdx=-1,
						int slastSliceIdx=-1)
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

	        if(i <  firstSliceIdx) continue;
	        if(i > lastSliceIdx) break;

			direction = !direction;
			SliceData& slice = slices[i];

			Scalar z = tomograph.layerMeasure.sliceIndexToHeight(currentSlice);
			currentSlice ++;

			slice.updatePosition(z, i);
			slice.extruderSlices.resize(1);

			ExtruderSlice &extruderSlice = slice.extruderSlices[0];

			const libthing::Insets &insetsSegments = skeleton.insets[i];
			const libthing::SegmentTable &outlineSegments = tomograph.outlines[i];

			outlines(outlineSegments, extruderSlice.boundary);

			PolygonsGroup &insetPolys = extruderSlice.insetLoopsList;
			this->insets(insetsSegments, insetPolys );

			const GridRanges &infillRanges = skeleton.infills[i];

			Polygons &infillsPolygons = extruderSlice.infills;
			this->infills(infillRanges, tomograph.grid, direction, infillsPolygons);
		}
	}

	void outlines(const libthing::SegmentTable& outlinesSegments, Polygons &boundary)
	{
		createPolysFromloopSegments(outlinesSegments, boundary);
	}

	void insets(const libthing::Insets& insetsForSlice, PolygonsGroup &insetPolys)
	{
		size_t nbOfShells = insetsForSlice.size();
		polygonsFromLoopSegmentTables(nbOfShells, insetsForSlice, insetPolys);
	}

	void infills(const GridRanges &infillRanges,
					const Grid &grid,
					bool direction,
					Polygons &infills)
	{
		grid.polygonsFromRanges(infillRanges, direction, infills);
	}

};



}

#endif
