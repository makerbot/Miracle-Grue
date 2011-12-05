#ifndef LIMITS_H_
#define LIMITS_H_

#include <limits>
#include "BGL/BGLPoint.h"


namespace mgl {

struct Limits
{

	friend std::ostream& operator <<(ostream &os, const Limits &l);

	Scalar xMin, xMax, yMin, yMax, zMin, zMax;

	Limits()
	{
		xMax = std::numeric_limits<Scalar>::min();
		yMax = xMax;
		zMax = xMax;

		xMin = std::numeric_limits<Scalar>::max();
		yMin = xMin;
		zMin = zMin;

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
	void grow(const BGL::Point3d &p)
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
		BGL::Point3d c = center();
		Scalar dx = 0.5 * (xMax-xMin);
		Scalar dy = 0.5 * (yMax - yMin);

		Scalar radius = sqrt(dx*dx + dy*dy);

		BGL::Point3d north = c;
		north.y += radius;

		BGL::Point3d south = c;
		south.y -= radius;

		BGL::Point3d east = c;
		east.x += radius;

		BGL::Point3d west = c;
		west.x -= radius;

		grow(north);
		grow(south);
		grow(east);
		grow(west);
	}

	BGL::Point3d center() const
	{
		BGL::Point3d c(0.5 * (xMin + xMax), 0.5 * (yMin + yMax), 0.5 *(zMin + zMax) );
		return c;
	}



};

std::ostream& operator<<(ostream& os, const Limits& l)
{
	os << "[" << l.xMin << ", " << l.yMin << ", " << l.zMin << "] [" << l.xMax << ", " << l.yMax << ", "<< l.zMax  << "]";
	return os;
}

}



#endif
