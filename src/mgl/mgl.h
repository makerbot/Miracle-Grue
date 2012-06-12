/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef MGL_H_
#define MGL_H_

#include <cmath>
#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>

#include "Exception.h"
#include "libthing/Scalar.h"
#include "libthing/Vector2.h"
#include "libthing/Vector3.h"
#include "libthing/LineSegment2.h"
#include "libthing/Triangle3.h"


#define M_TAU M_PI*2

// #define STRONG_CHECKING

#define dbg__ std::cout <<  __FUNCTION__ << "::" << __LINE__  << "*" << std::endl;
#define dbgs__(s) std::cout <<  __FUNCTION__ << "::" << __LINE__  << " > "<< s << std::endl;


namespace mgl
{

std::string getMiracleGrueVersionStr();

/// Structure contains list of triangle 'id's, used to
/// reference which triangle in the master list is related.
typedef std::vector<index_t> TriangleIndices;

/// A list of all slices, where each slice
/// is just a list of triangles id's that are related to
/// that specified slice.
typedef std::vector<TriangleIndices> SliceTable;


// Bring over from mgl.cc
Scalar AreaSign(const libthing::Vector2&a, const libthing::Vector2&b, const libthing::Vector2&c);
bool convexVertex(const libthing::Vector2&i, const libthing::Vector2&j, const libthing::Vector2&k);

std::ostream& operator << (std::ostream &os,const libthing::Vector2&pt);
std::ostream& operator << (std::ostream& os, const libthing::Vector3& v);
//std::ostream& operator<<(std::ostream& os, libthing::LineSegment2 const& line);

bool collinear(const libthing::LineSegment2 &prev, const libthing::LineSegment2 &current,
		Scalar tol, libthing::Vector2 &mid);


class LayerException : public Exception {
	public: LayerException(const char *msg)	 :Exception(msg) {	};
};

// A tape measure for layers, since layers have variable height.
// Class that relates height (a scalar) to layer index (unsigned int)
//
// This class assumes that the model's triangles are
// all above 0 (the z of each of the 3 vertices is >= 0.0).
// worse, the layers MUST start at 0. Lazy programmer!
// This is good enough for now, until the class "sees" every triangle
// during loading and recalcs layers on the fly.
//
class LayerMeasure
{
	Scalar firstLayerZ;
	Scalar layerH;

public:
	LayerMeasure(Scalar firstLayerZ, Scalar layerH)
		:firstLayerZ(firstLayerZ), layerH(layerH)
	{}

	unsigned int zToLayerAbove(Scalar const z) const
	{
		Scalar const tol = 0.000001; // tolerance: 1 nanometer
		if (libthing::tlower(z, 0, tol))
		{
			std::ostringstream oss;
			oss << "Model with points below the z axis are not supported in this version. Please center your model on the build area.";
			oss << " z=" << z << std::endl;
			LayerException mixup(oss.str().c_str());
			throw mixup;
		}

		if (libthing::tlower(z, firstLayerZ, tol))
			return 0;

		Scalar const layer = (z+tol-firstLayerZ) / layerH;
		return static_cast<unsigned int>(ceil(layer));
	}

	Scalar sliceIndexToHeight(unsigned int sliceIndex) const
	{
		return firstLayerZ + sliceIndex * layerH;
	}

	Scalar getLayerH() const
	{
		return layerH;
	}
};

/// A polygon is an arbitarty collection of 2d points
typedef std::vector<libthing::Vector2> Polygon;

/// Verifies each libthing::Vector2in the passed Polygon are in tolerance
// tol
bool tequalsPolygonCompare(Polygon& poly1, Polygon& poly2, Scalar tol);

typedef std::vector<Polygon> Polygons;

/// Verifies each Polygon in the passed Polygons are in tolerance
bool tequalsPolygonsCompare(Polygons& poly1, Polygons& poly2, Scalar tol);


/// A vector of Polygons objects is a PolygonsGroup
typedef std::vector<Polygons> PolygonsGroup;


std::ostream& operator<<(std::ostream& os, const Polygon& v);


std::string stringify(double x);
std::string stringify(size_t x);


} // namespace

#endif
