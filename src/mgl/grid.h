/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef GRID_H_
#define GRID_H_

#include "mgl.h"
#include "limits.h"
#include "libthing/LineSegment2.h"
#include "segment.h"

namespace mgl
{

class GridException : public mgl::Exception {	public: GridException(const char *msg) :Exception(msg){} };


struct ScalarRange
{
	Scalar min;
	Scalar max;
	ScalarRange()
	:min(0), max(0)
	{

	}

	ScalarRange(Scalar a, Scalar b)
	:min(a), max(b)
	{
		// assert(b>a);
	}

	ScalarRange(const ScalarRange& original)
	{
		this->min = original.min;
		this->max = original.max;
	}

	// default assignment operator (min and max are copied)
	void operator = (const ScalarRange& next)
	{
		if( &next != this)
		{
			this->min = next.min;
			this->max = next.max;
		}
	}
};

std::ostream& operator << (std::ostream &os,const ScalarRange &pt);

typedef std::vector< std::vector<ScalarRange> > ScalarRangeTable;


struct GridRanges
{
    ScalarRangeTable xRays;
    ScalarRangeTable yRays;
};


///  A grid is a set of intersecting lines that is subsampled to create
/// infill patterns.
///
///
class Grid
{
    std::vector<Scalar> xValues; ///< list of spacing between lines along y axis(mm)
    std::vector<Scalar> yValues; ///< list of spacing between lines along x axis(mm)
    libthing::Vector2 gridOrigin; ///< origin of our grid system

public:
    Grid();

    /// Generates grid during construction
    /// @param limits: x,y size of grid
    /// @param gridSpacing: idealized space between generated infill lines at 100%
	///		coverage. Maps directly to filament witdh in final print.
    Grid(const Limits &limits, Scalar gridSpacing);

	/// re-initialize the grid, so you can recycle grid and save electrons. (jk)
    void init(const Limits &limits, Scalar gridSpacing);


    libthing::Vector2 getOrigin() const {return gridOrigin;}

    const std::vector<Scalar>& getXValues() const{return xValues;}

    const std::vector<Scalar>& getYValues() const{return yValues;}

    /// Creates range of beginning to end of gridlines
    /// @param returns a list of GridRanges 'cut out' of the underlying
    /// idealized grid based on our segments in segments.
    /// @param loops: a SegmentTable containing segments specifying
    ///		exactly one layer outline to use to 'cookie cutter' out gridlines
    void createGridRanges(const libthing::SegmentTable &loops, GridRanges &outGridRanges) const;

    /// The grid starts out at 100% infill, this function selectlviy removes filament
    /// based on a skip count to reduce density
    /// @param srcGridRanges: input grid range to sub-sample
    /// @param skipCount : number if infill lines to skip. infill_ration = (1/skipCount) -1
    /// @param result: returned grid post-subsampling
    void subSample(const GridRanges &srcGridRanges, size_t skipCount, GridRanges &result) const;

    /// Takes a gridRange and converts that into Polygons that can be used to generate
    /// gcode.
    void polygonsFromRanges(const GridRanges &gridRanges,
							const libthing::SegmentTable &outline,
							bool xDirection, Polygons &polys) const;

    /// joins a/b grid ranges into
    void gridRangeUnion(const GridRanges& a, const GridRanges &b, GridRanges &result) const;

	/// subtracts GridRagne diff from GridRange src to return result grid range
    void gridRangeDifference(const GridRanges& src, const GridRanges &diff, GridRanges &result) const;

	/// returns a gridrange that is the interesction of a/b
    void gridRangeIntersection(const GridRanges& a, const GridRanges &b, GridRanges &result) const;

    /// removes all grid ranges shorter than toleranne 'cutOff', returns a new
    /// simplified grid range
    void trimGridRange(const GridRanges& src, Scalar cutOff, GridRanges &result) const;

};

}

#endif
