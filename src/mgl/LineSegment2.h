/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.
=======
*/
#ifndef LINE_SEGMENT2_H_
#define LINE_SEGMENT2_H_

#include "mgl.h"

namespace mgl {


/// A line segment of 2 point 2d points.
class LineSegment2
{

public:
	Vector2 a,b; // the 2 points

public:
	LineSegment2();

	LineSegment2(const LineSegment2& other);

	LineSegment2(const Vector2 &a, const Vector2 &b);

	LineSegment2 & operator= (const LineSegment2 & other);

	Scalar squaredLength() const;
	Scalar length() const;

	LineSegment2 elongate(const Scalar& dist) const;
	LineSegment2 prelongate(const Scalar& dist) const;

};

/// List of Lists of line segments. Used to lookup
/// A SegmentTable may contain, for example, a perimeter
/// and hole(s) in that perimeter of a slice.
typedef std::vector< std::vector<LineSegment2 > > SegmentTable;

std::ostream& operator << (std::ostream &os, const LineSegment2 &s);

bool collinear(const LineSegment2 &prev, const LineSegment2 &current, Scalar tol, Vector2 &mid);


}//end namespace mgl

#endif //LINESEGMENT_H_

