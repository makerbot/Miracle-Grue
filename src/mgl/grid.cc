/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include <set>

#include "grid.h"

using namespace mgl;
using namespace std;

using namespace libthing;




std::ostream& mgl::operator << (std::ostream &os,const ScalarRange &p)
{
	cout << "[" << p.min << ", " << p.max << "]";
	return os;
}


// local types, methods and functions
void scalarRangesFromIntersections(const std::set<Scalar> &lineCuts, std::vector<ScalarRange> &ranges)
{
	ranges.reserve(lineCuts.size());
    bool inside = false;
    Scalar xBegin = 0; // initial value is not used
    Scalar xEnd = 0;   // initial value is not used
    for(std::set<Scalar>::iterator it = lineCuts.begin(); it != lineCuts.end(); it++)
	{
    	Scalar intersection = *it;
    	if(inside)
    	{
    		xEnd = intersection;
    		// gridSegments.push_back(LineSegment2(Vector2(xBegin,y), Vector2(xEnd,y)));
    		ranges.push_back(ScalarRange(xBegin, xEnd));
    	}
    	else
    	{
    		xBegin = intersection;
    	}
    	inside = !inside;
	}

    if(inside)
    {
    	// this is not good. xMax should be outside the object
    	GridException messup("Ray has been cast outside the model mesh.");

    }
}

void rayCastAlongX(	const SegmentTable &outlineLoops,
									Scalar y,
									Scalar xMin,
									Scalar xMax,
									std::vector<ScalarRange> &ranges)
{
    std::set<Scalar> lineCuts;

    // go through all the segments in every loop
    for(unsigned int j=0; j< outlineLoops.size(); j++)
    {
        const std::vector<LineSegment2> &outlineLineSegment2s = outlineLoops[j];
        for(std::vector<LineSegment2>::const_iterator it= outlineLineSegment2s.begin(); it!= outlineLineSegment2s.end(); it++)
        {
            const LineSegment2 &segment = *it;
            Scalar intersectionX, intersectionY;
            if (segmentSegmentIntersection( xMin,
                                            y,
                                            xMax,
                                            y,
                                            segment.a.x,
                                            segment.a.y,
                                            segment.b.x,
                                            segment.b.y,
                                            intersectionX,
                                            intersectionY))
            {
                lineCuts.insert(intersectionX);
            }
        }
    }

    scalarRangesFromIntersections(lineCuts, ranges);

}

void rayCastAlongY(	const SegmentTable &outlineLoops,
						Scalar x,
						Scalar yMin,
						Scalar yMax,
						std::vector<ScalarRange> &ranges)
{
    std::set<Scalar> lineCuts;

    // go through all the segments in every loop
    for(unsigned int j=0; j< outlineLoops.size(); j++)
    {
        const std::vector<LineSegment2> &outlineLineSegment2s = outlineLoops[j];
        for(std::vector<LineSegment2>::const_iterator it= outlineLineSegment2s.begin(); it!= outlineLineSegment2s.end(); it++)
        {
            const LineSegment2 &segment = *it;
            Scalar intersectionX, intersectionY;
            if (segmentSegmentIntersection( x,
                                            yMin,
                                            x,
                                            yMax,
                                            segment.a.x,
                                            segment.a.y,
                                            segment.b.x,
                                            segment.b.y,
                                            intersectionX,
                                            intersectionY))
            {
                lineCuts.insert(intersectionY);
            }
        }
    }
    scalarRangesFromIntersections(lineCuts, ranges);

}


void castRaysOnSliceAlongX(const SegmentTable &outlineLoops,
					const std::vector<Scalar> &yValues,
					Scalar xMin,
					Scalar xMax,
					ScalarRangeTable &rangeTable)
{
	assert(rangeTable.size() == 0);
	rangeTable.resize(yValues.size());

	for(size_t i=0; i < rangeTable.size(); i++)
	{
		Scalar y = yValues[i];
		std::vector<ScalarRange> &ranges = rangeTable[i];
	    rayCastAlongX(outlineLoops, y, xMin, xMax, ranges);
	}
}

void castRaysOnSliceAlongY(const SegmentTable &outlineLoops,
					const std::vector<Scalar> &values, // x
					Scalar min,
					Scalar max,
					ScalarRangeTable &rangeTable)
{
	assert(rangeTable.size() == 0);
	rangeTable.resize(values.size());

	for(size_t i=0; i < rangeTable.size(); i++)
	{
		Scalar value = values[i];
		std::vector<ScalarRange> &ranges = rangeTable[i];
	    rayCastAlongY(outlineLoops, value, min, max, ranges);
	}
}

void polygonsFromScalarRangesAlongX( const ScalarRangeTable &rays,	   // the ranges along x, multiple per lines
								const std::vector<Scalar> &values, // the y values for each line
								Polygons &polygons)				   // the output
{
	// change direction of extrusion
	// for each line
	bool forward = false;
	assert(rays.size() == values.size());

	for(size_t rayId =0; rayId < rays.size(); rayId++)
	{
		const vector<ScalarRange> &ray = rays[rayId];
		if(ray.size() == 0)
			continue;

		forward = !forward;
		Scalar value = values[rayId];

		vector<ScalarRange> line;
		line.reserve(ray.size());
		if(forward)
		{
			line = ray;
		}
		else
		{
			// reverse the ranges in the ray
			for(size_t i=0; i < ray.size(); i++)
			{
				ScalarRange range(ray[i].max, ray[i].min);
				line.push_back(range);
			}
		}
		for(size_t i= 0; i< line.size(); i++ )
		{
			const ScalarRange &range = line[i];
			Vector2 begin(range.min, value);
			Vector2 end(range.max, value);

			// add a polygon
			polygons.push_back(Polygon());
			Polygon &poly = polygons.back();
			poly.push_back(begin);
			poly.push_back(end);
		}
	}
}

void polygonsFromScalarRangesAlongY( const ScalarRangeTable &rays,	   // the ranges along x, multiple per lines
								const std::vector<Scalar> &values, // the x values for each line
								Polygons &polygons)				   // the output
{
	// change direction of extrusion
	// for each line
	bool forward = false;
	assert(rays.size() == values.size());

	for(size_t rayId =0; rayId < rays.size(); rayId++)
	{
		const vector<ScalarRange> &ray = rays[rayId];
		if(ray.size() == 0)
			continue;

		forward = !forward;
		Scalar value = values[rayId];

		vector<ScalarRange> line;
		line.reserve(ray.size());
		if(forward)
		{
			line = ray;
		}
		else
		{
			// reverse the ranges in the ray
			for(size_t i=0; i < ray.size(); i++)
			{
				ScalarRange range(ray[i].max, ray[i].min);
				line.push_back(range);
			}
		}
		for(size_t i= 0; i< line.size(); i++ )
		{
			const ScalarRange &range = line[i];
			Vector2 begin(value, range.min);
			Vector2 end(value, range.max);

			// add a polygon
			polygons.push_back(Polygon());
			Polygon &poly = polygons.back();
			poly.push_back(begin);
			poly.push_back(end);
		}
	}
}

bool intersectRange(Scalar a, Scalar b, Scalar c, Scalar d, Scalar &begin, Scalar &end)
{
	assert(b>=a);
	assert(d>=c);

//	cout << "[" << a << ", " << b << "] inter ";
//  cout << "[" << c << ", " << d << "]";

	if(a >= d)
	{
		cout << " = 0" << endl;
		return false;
	}

	if (c >=b )
	{
		cout << " = 0" << endl;
		return false;
	}

	if(a >= c)
	{
		begin = a;
	}
	else
	{
		begin = c;
	}

	if( b >= d)
	{
		end = d;
	}
	else
	{
		end = b;
	}
//	cout << " = [" << begin << ", " << end << "]" << endl;
	return true;
}


vector< ScalarRange >::const_iterator  subRangeTersect(	const ScalarRange &range,
		 	 	 	 	vector< ScalarRange >::const_iterator it,
		 	 	 	 	vector< ScalarRange >::const_iterator itEnd,
						vector< ScalarRange > &result )
{

	while(it != itEnd)
	{
		const ScalarRange &currentRange = *it;
		if( (it->min >= range.max)  )
		{
			// cout << " subrange done" << endl; // << currentRange << endl;
			return it;
		}

		ScalarRange intersection;
		// cout << " second="<< currentRange << endl;
		if( intersectRange(range.min, range.max, currentRange.min, currentRange.max, intersection.min, intersection.max) )
		{
			// cout << " Intersect: [" << range.min << ", " << range.max << "]"<< endl;
			result.push_back(intersection);
		}
		it ++;
	}
	return it;
}



void rangeTersection(const vector< ScalarRange > &oneLine,
					 const vector< ScalarRange > &twoLine,
						vector< ScalarRange > &boolLine )
{
	vector< ScalarRange >::const_iterator itOne = oneLine.begin();
	vector< ScalarRange >::const_iterator itTwo = twoLine.begin();
	while(itOne != oneLine.end())
	{
		const ScalarRange &range = *itOne;
		// cout << "range=" << range << endl;
		itTwo = subRangeTersect(range, itTwo, twoLine.end(), boolLine);
		if(itTwo == twoLine.end())
		{
			itOne++;
			if(itOne != oneLine.end())
			{
				const ScalarRange &lastRange = twoLine.back();
				// cout << "lastRange=" << lastRange << endl;
				subRangeTersect(lastRange, itOne, oneLine.end(), boolLine);
			}
			return;
		}
		itOne++;
	}
}


// return false if the ranges don't intersect
//
bool scalarRangeUnion(const ScalarRange& range0, const ScalarRange& range1, ScalarRange &resultRange)
{
	// cout << " union( " << range0 << ", " << range1 << ")=";
	if( (range1.min > range0.max) || (range0.min > range1.max) )
	{
		// cout << "0" << endl;
		return false;
	}

	resultRange.min = range0.min < range1.min?range0.min:range1.min;
	resultRange.max = range0.max > range1.max?range0.max:range1.max;

	// cout << resultRange<<endl;
	return true;
}

// removes diffRange from srcRange. The result is put in resultRange, and srcRange is updated
// returns false if there is no resultRange


vector< ScalarRange >::const_iterator  subRangeUnion(const ScalarRange &initialRange,
		 	 	 	 	vector< ScalarRange >::const_iterator it,
		 	 	 	 	vector< ScalarRange >::const_iterator itEnd,
						vector< ScalarRange > &result )
{

	ScalarRange range(initialRange);
	// cout << endl <<"-- subRangeUnion --" << endl;
	while(it != itEnd)
	{
		const ScalarRange &itRange = *it;
		if( (itRange.min > range.max)  )
		{
			// cout << " -PUSH" << range << endl;
			result.push_back(range);
			cout << " = " << range;
			return it;
		}
		ScalarRange newRange;
		bool u = scalarRangeUnion(range, itRange, newRange);
		if(u)
		{
			// cout << " -RANGE=" << range << endl;
			range = newRange;
		}
		else
		{
			// cout << " -PUSH" << range << endl;
			result.push_back(itRange);
			cout << " ++ " << itRange;

		}
		it++;
	}
	// cout << " -done!" <<endl;
	result.push_back(range);
	cout << " +++ " << range;
}


void rangeUnion( const vector< ScalarRange > &firstLine,
		 	 	 	  const vector< ScalarRange > &secondLine,
		 	 	 	  vector< ScalarRange > &unionLine )
{
	vector< ScalarRange >::const_iterator itOne = firstLine.begin();
	vector< ScalarRange >::const_iterator itTwo = secondLine.begin();

	cout << "rangeUnion: ";

	while(itOne != firstLine.end())
	{
		const ScalarRange &range = *itOne;
		cout << "first_range=" << range << endl;
		// check that the last range has not advanced beyond the firstLine
		if(unionLine.size() >0)
		{
			ScalarRange &lastUnion = unionLine.back();
			// cout << "LAST RANGE UPDATE COMPARE: last=" << lastUnion << " range=" << range;
			if(range.min <= lastUnion.max && lastUnion.max >= range.max)
			{
				// cout << " !UPDATE ONLY" << endl;
				lastUnion.max = range.max;
				itOne++;
				continue;
			}
		}
		// cout << " !no update" << endl;
		if(itTwo == secondLine.end())
		{
			unionLine.push_back(range);
			cout << " + " << range;
		}
		else
		{
			itTwo = subRangeUnion(range, itTwo, secondLine.end(), unionLine);
		}
		itOne++;
	}

	cout << " rangeUnionDone" << endl;
}




bool scalarRangeDifference(const ScalarRange& diffRange,
							ScalarRange& srcRange,
							ScalarRange &resultRange)
{
	// cout << srcRange << " - " << diffRange << " = ";
	// the diffRange is left of srcRange ... no result
	if(diffRange.max <= srcRange.min)
	{
		// cout << "0 (before)" << endl;
		return false;
	}

	// the diff covers the src
	// the src is (partially) occluded
	if(diffRange.min <= srcRange.min)
	{
		if(diffRange.max >= srcRange.max )
		{
			srcRange.min = srcRange.max;
			// cout << "0 (occlusion)" << endl;
			return false;
		}
		// else... adjust the srcRange and make it smaller
		srcRange.min = diffRange.max;
		// cout << "0 partial occlusion, leftover = " << srcRange << endl;
		return false;

	}

	// intersection of the ranges
	if( (diffRange.min >= srcRange.min)  )
	{
		resultRange.min = srcRange.min;
		resultRange.max = diffRange.min;

		// left over on the right side
		if(diffRange.max <= srcRange.max)
		{
			srcRange.min = diffRange.max;
		}
		else
		{
			srcRange.min = srcRange.max;
		}
		// cout << resultRange << " (intersection!) leftover " <<  srcRange << endl;
		return true;
	}

	// srcRange is not occluded by diffRange which
	// is right of scrRange
	// there is nothing to remove: the result is the range
	if(diffRange.min >= srcRange.max)
	{
		resultRange = srcRange;

		// remove srcRange so it is
		// not used twice
		srcRange.min = srcRange.max;
		// cout << resultRange << " (all in!) leftover " <<  srcRange << endl;
		return true;
	}

	cout << "PROBLEM!" << endl;
	assert(0);
	return false;
}

vector< ScalarRange >::const_iterator  subRangeDifference(	const ScalarRange &initialRange,
		 	 	 	 	vector< ScalarRange >::const_iterator it,
		 	 	 	 	vector< ScalarRange >::const_iterator itEnd,
						vector< ScalarRange > &result )
{


	ScalarRange range(initialRange);
	while(it != itEnd)
	{
		const ScalarRange &itRange = *it;
		if( (itRange.min >= range.max)  )
		{
			return it;
		}

		ScalarRange difference;
		if (scalarRangeDifference(itRange, range, difference))
		{
			result.push_back(difference);
		}
		if(range.min >= range.max) // the leftover range has no length
		{
			// cout << "no left over" << endl;
			return it;
		}

		it ++;
	}
	// add the left over (if any)
	if(range.max > range.min)
	{
		result.push_back(range);
	}
	return it;
}

void rangeDifference(const vector< ScalarRange > &srcLine,
		 	 	 	  const vector< ScalarRange > &delLine,
		 	 	 	  vector< ScalarRange > &diffLine )
{
	vector< ScalarRange >::const_iterator itOne = srcLine.begin();
	vector< ScalarRange >::const_iterator itTwo = delLine.begin();
	while(itOne != srcLine.end())
	{
		const ScalarRange &range = *itOne;
		// cout << "range=" << range << endl;
		itTwo = subRangeDifference(range, itTwo, delLine.end(), diffLine);
		if(itTwo == delLine.end())
		{
			return;
		}
		itOne++;
	}
}

// computes the difference between the ranges of two layers
void rangeTableDifference(	const ScalarRangeTable &src,
							const ScalarRangeTable &del,
							ScalarRangeTable &diff)
{
	size_t lineCount = src.size();
	assert(lineCount == del.size());
	diff.resize(lineCount);

	for(size_t i=0; i < src.size(); i++ )
	{
		const vector<ScalarRange> &lineRangeSrc = src[i];
		const vector<ScalarRange> &lineRangeDel = del[i];
		vector<ScalarRange> &lineRangeDiff = diff[i];

		rangeDifference(lineRangeSrc, lineRangeDel, lineRangeDiff);
	}
}




void rangeTableIntersection(const ScalarRangeTable &a,
							const ScalarRangeTable &b,
							ScalarRangeTable &result)
{
	size_t lineCount = a.size();
	assert(lineCount == b.size());
	result.resize(lineCount);

	for(size_t i=0; i < a.size(); i++ )
	{
		const vector<ScalarRange> &lineRange0 = a[i];
		const vector<ScalarRange> &lineRange1 = b[i];
		vector<ScalarRange> &lineRangeRes = result[i];

		rangeTersection(lineRange0, lineRange1, lineRangeRes);
	}
}

void rangeTableUnion(const ScalarRangeTable &a,
							const ScalarRangeTable &b,
							ScalarRangeTable &result)
{
	size_t lineCount = a.size();
	// cout << " rangeTableUnion " << lineCount << " vs " << b.size() << endl;

	assert(lineCount == b.size());
	result.resize(lineCount);

	for(size_t i=0; i < a.size(); i++ )
	{
		const vector<ScalarRange> &lineRange0 = a[i];
		const vector<ScalarRange> &lineRange1 = b[i];
		vector<ScalarRange> &lineRangeRes = result[i];

		rangeUnion(lineRange0, lineRange1, lineRangeRes);
	}
}

// Grid class implementation

Grid::Grid()
{}

Grid::Grid(const Limits &limits, Scalar gridSpacing)
{
	init(limits, gridSpacing);
}

void Grid::init(const Limits &limits, Scalar gridSpacing)
{

	Scalar deltaY = limits.yMax - limits.yMin;
	Scalar deltaX = limits.xMax - limits.xMin;

	gridCenter[0] = limits.xMin + 0.5 * deltaX;
	gridCenter[1] = limits.yMin + 0.5 * deltaY;

	// round to nearest odd number
	unsigned int yGridSize = (unsigned int)( deltaY / gridSpacing) + 1;
	unsigned int xGridSize = (unsigned int)( deltaX / gridSpacing) + 1;

	yValues.resize(yGridSize);
	for (unsigned int i=0; i < yGridSize; i++)
	{
		Scalar v = gridCenter[1] -0.5 * deltaY + i * gridSpacing;
		yValues[i] = v;
	}

	xValues.resize(xGridSize);
	for (unsigned int i=0; i < xGridSize; i++)
	{
		Scalar v = gridCenter[0] -0.5 * deltaX + i * gridSpacing;
		xValues[i] = v;
	}
}



void Grid::createGridRanges(const SegmentTable &loops,
							GridRanges &gridRanges) const
{
	Scalar xMin = xValues[0];
	Scalar xMax = xValues.back();
	castRaysOnSliceAlongX(loops, yValues, xMin, xMax, gridRanges.xRays);

	Scalar yMin = yValues[0];
	Scalar yMax = yValues.back();
	castRaysOnSliceAlongY(loops, xValues, yMin, yMax, gridRanges.yRays);

}

void Grid::subSample(const GridRanges &gridRanges, size_t skipCount, GridRanges &result) const
{
	assert(result.xRays.size() == 0);
	assert(result.yRays.size() == 0);

	result.xRays.resize(gridRanges.xRays.size());
	result.yRays.resize(gridRanges.yRays.size());

	for(size_t i=0; i < gridRanges.xRays.size(); i++)
	{
		result.xRays[i] = gridRanges.xRays[i]; // deep copy of the ranges for the selected lines
		i += skipCount; // skip lines depending on selected infill density
	}

	for(size_t i=0; i < gridRanges.yRays.size(); i++)
	{
		result.yRays[i] = gridRanges.yRays[i]; // deep copy of the ranges for the selected lines
		i += skipCount; // skip lines depending on selected infill density
	}
}

void Grid::polygonsFromRanges(const GridRanges &gridRanges, bool xDirection, Polygons &polys) const
{
	assert(polys.size() == 0);
	if(xDirection)
	{
		polygonsFromScalarRangesAlongX(gridRanges.xRays, yValues, polys);
	}
	else
	{
		polygonsFromScalarRangesAlongY(gridRanges.yRays, xValues, polys);
	}
}

void Grid::gridRangeUnion(const GridRanges& a, const GridRanges &b, GridRanges &result) const
{
	rangeTableUnion(a.xRays, b.xRays, result.xRays);
	rangeTableUnion(a.yRays, b.yRays, result.yRays);
}

void Grid::gridRangeDifference(const GridRanges& src, const GridRanges &del, GridRanges &result) const
{
	rangeTableDifference(src.xRays, del.xRays, result.xRays );
	rangeTableDifference(src.yRays, del.yRays, result.yRays );

}

void Grid::gridIntersection(const GridRanges& a, const GridRanges &b, GridRanges &result) const
{
	rangeTableIntersection(a.xRays, b.xRays, result.xRays);
	rangeTableIntersection(a.yRays, b.yRays, result.yRays);
}

