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


class Grid
{
    std::vector<Scalar> xValues;
    std::vector<Scalar> yValues;
    libthing::Vector2 gridCenter;

public:
    Grid();

    Grid(const Limits &limits, Scalar gridSpacing);
    void init(const Limits &limits, Scalar gridSpacing);

    libthing::Vector2 readGridCenter() const {return gridCenter;}
    const std::vector<Scalar>& readXvalues() const{return xValues;}
    const std::vector<Scalar>& readYvalues() const{return yValues;}


    void createGridRanges(const libthing::SegmentTable &loops, GridRanges &gridRanges) const;
    void subSample(const GridRanges &gridRanges, size_t skipCount, GridRanges &result) const;
    void polygonsFromRanges(const GridRanges &gridRanges, bool xDirection, Polygons &polys) const;

    void gridRangeUnion(const GridRanges& a, const GridRanges &b, GridRanges &result) const;
    void gridRangeDifference(const GridRanges& src, const GridRanges &diff, GridRanges &result) const;
    void gridIntersection(const GridRanges& a, const GridRanges &b, GridRanges &result) const;

};

}

#endif
