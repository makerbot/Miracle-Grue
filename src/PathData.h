/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/

#ifndef __PATH_DATA_H_
#define __PATH_DATA_H_


#include <vector>

#include "DataEnvelope.h"

typedef double real;

struct Point2D
{
	Point2D(real x, real y)
		:x(x), y(y)
	{}
	real x;
	real y;
};

typedef std::vector<Point2D> Polygon;
typedef std::vector<Polygon> Paths;

//
// The Path data is a dictionary of polygons
// there are multiple polygons for each extruders
//
class PathData: public DataEnvelope {

public:
	PathData();
	virtual ~PathData();

	std::vector<Paths> extrusionPaths; // each extruder has paths

};

#endif // __PATH_DATA_H_
