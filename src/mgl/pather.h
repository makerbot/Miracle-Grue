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


	Pather(ProgressBar * progress = NULL);


	void generatePaths(const Tomograph &tomograph,
						const Regions &skeleton,
						std::vector<SliceData> &slices,
						int sfirstSliceIdx=-1,
						int slastSliceIdx=-1);


	void outlines(const libthing::SegmentTable& outlinesSegments, Polygons &boundary);

	void insets(const libthing::Insets& insetsForSlice, PolygonsGroup &insetPolys);

	void infills(const GridRanges &infillRanges,
				 const Grid &grid,
				 const std::vector<libthing::SegmentTable> &outlines,
				 bool direction,
				 Polygons &infills);


};



}

#endif
