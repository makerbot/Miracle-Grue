/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#include <set>

#include "infill.h"


using namespace std;
using namespace mgl;

bool shrinkLine(Vector2 &left, Vector2 &right, Scalar shrinkDistance)
{
	Vector2 l = right -left;
	if(l.magnitude() > 2 * shrinkDistance)
	{
		left[0]  +=  shrinkDistance;
		right[0] -=  shrinkDistance;
		return true;
	}
	return false;
}



void mgl::infillPathology(const  SegmentTable &outlineLoops,
							const Limits& limits,
							double z,
							double tubeSpacing,
							Scalar infillShrinking,
							Polygons& infills)
{
    assert(tubeSpacing > 0);
    assert(infills.size() == 0);
	Scalar deltaY = limits.yMax - limits.yMin;

        unsigned int gridSize = (unsigned int)((deltaY) / tubeSpacing);
	std::vector< std::set<Scalar> > intersects;
	// allocate
        intersects.resize(gridSize);

        for (unsigned int i=0; i < gridSize; i++)
	{
		Scalar y = -0.5 * deltaY + i * tubeSpacing;
		std::set<Scalar> &lineCuts = intersects[i];

		// go through all the segments in every loop
		for(unsigned int j=0; j< outlineLoops.size(); j++)
		{
                        const std::vector<LineSegment2> &outlineLineSegment2s = outlineLoops[j];
                        for(std::vector<LineSegment2>::const_iterator it= outlineLineSegment2s.begin(); it!= outlineLineSegment2s.end(); it++)
			{
                                const LineSegment2 &segment = *it;
				Scalar intersectionX, intersectionY;
                                if (segmentSegmentIntersection( limits.xMin,
                                                                y,
                                                                limits.xMax, y,
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
	}

	// tubes.resize(tubeCount);

	bool backAndForth = true;
	Scalar bottom = -0.5 * deltaY;
        for (unsigned int i=0; i < gridSize; i++)
	{

		Scalar y = bottom + i * tubeSpacing;
		std::set<Scalar> &lineCuts = intersects[i];

		Vector2 begin;
		Vector2 end;
		bool inside = false;
		if( backAndForth)
		{
			for(std::set<Scalar>::iterator it = lineCuts.begin(); it != lineCuts.end(); it++)
			{
				inside =! inside;
				Scalar x = *it;

				if(inside)
				{

					begin.x = x;
					begin.y = y;
				}
				else
				{
					end.x = x;
					end.y = y;

					if(shrinkLine(begin, end, infillShrinking))
					{
						infills.push_back(Polygon());
						Polygon &poly = infills.back();
						poly.push_back(begin);
						poly.push_back(end);
					}
				}
			}
		}
		else
		{
			for(std::set<Scalar>::reverse_iterator it = lineCuts.rbegin(); it != lineCuts.rend(); it++)
			{
				inside =! inside;
				Scalar x = *it;

				if(inside)
				{
					begin.x = x;
					begin.y = y;
				}
				else
				{
					end.x = x;
					end.y = y;

					if(shrinkLine(end, begin, infillShrinking))
					{
						infills.push_back(Polygon());
						Polygon &poly = infills.back();
						poly.push_back(begin);
						poly.push_back(end);
					}
				}
			}
		}
		backAndForth = !backAndForth;
	}

}
