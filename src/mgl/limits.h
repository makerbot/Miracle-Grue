/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef LIMITS_H_
#define LIMITS_H_

#include <iostream>
#include <limits>
#include "core.h"


namespace mgl {

struct Limits
{

	friend std::ostream& operator <<(std::ostream &os, const Limits &l);

	Scalar xMin, xMax, yMin, yMax, zMin, zMax;

	Limits()
	{
		xMax = std::numeric_limits<Scalar>::min();
		yMax = xMax;
		zMax = xMax;

		xMin = std::numeric_limits<Scalar>::max();
		yMin = xMin;
		zMin = xMin;

	}
/*
	Limits(const Limits& kat)
	{
		xMin = kat.xMin;
		xMax = kat.xMax;
		yMin = kat.yMin;
		yMax = kat.yMax;
		zMin = kat.zMin;
		zMax = kat.zMax;
	}
*/
	void grow(const Vector3 &p)
	{
		if(p.x < xMin) xMin = p.x;
		if(p.x > xMax) xMax = p.x;
		if(p.y < yMin) yMin = p.y;
		if(p.y > yMax) yMax = p.y;
		if(p.z < zMin) zMin = p.z;
		if(p.z > zMax) zMax = p.z;
	}

	// adds inflate to all sides (half of inflate in + and half inflate in - direction)
	void inflate(Scalar inflateX, Scalar inflateY, Scalar inflateZ)
	{
		xMin -= 0.5 * inflateX;
		xMax += 0.5 * inflateX;
		yMin -= 0.5 * inflateY;
		yMax += 0.5 * inflateY;
		zMin -= 0.5 * inflateZ;
		zMax += 0.5 * inflateZ;
	}

	// grows the limits to contain points that rotate along
	// the XY center point and Z axis
	void tubularZ()
	{
		Vector3 c = center();
		Scalar dx = 0.5 * (xMax-xMin);
		Scalar dy = 0.5 * (yMax - yMin);

		Scalar radius = sqrt(dx*dx + dy*dy);

		Vector3 north = c;
		north.y += radius;

		Vector3 south = c;
		south.y -= radius;

		Vector3 east = c;
		east.x += radius;

		Vector3 west = c;
		west.x -= radius;

		grow(north);
		grow(south);
		grow(east);
		grow(west);
	}

	Vector3 center() const
	{
		Vector3 c(0.5 * (xMin + xMax), 0.5 * (yMin + yMax), 0.5 *(zMin + zMax) );
		return c;
	}

	Scalar deltaX() const
	{
		return (xMax - xMin);
	}

	Scalar deltaY() const
	{
		return (yMax - yMin);
	}

	Limits centeredLimits() const
	{
		Limits out;
		out.xMax = 0.5 * deltaX();
		out.xMin = -out.xMax;
		out.yMax = 0.5 * deltaY();
		out.yMin = -out.yMax;
		out.zMin = zMin;
		out.zMax = zMax;
		return out;
	}



};

std::ostream& operator<<(std::ostream& os, const Limits& l);



}



#endif
