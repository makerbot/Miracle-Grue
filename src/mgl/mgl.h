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
#include <cassert>

#include "Exception.h"
#include "Vector2.h"
#include "Vector3.h"
#include "LineSegment2.h"
#include "Triangle3.h"


// WIN32 compatibility stuff
#ifdef WIN32

#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define M_PI 3.14159265358979323846

#endif // WIN32

// #define STRONG_CHECKING

#define dbg__ std::cout <<  __FUNCTION__ << "::" << __LINE__  << "*" << std::endl;
#define dbgs__(s) std::cout <<  __FUNCTION__ << "::" << __LINE__  << " > "<< s << std::endl;


namespace mgl
{

std::string getMiracleGrueVersionStr();

// Type used for indexes of triangles/etc for unique indexing
typedef unsigned int index_t;

/// Structure contains list of triangle 'id's, used to
/// reference which triangle in the master list is related.
typedef std::vector<index_t> TriangleIndices;

/// A list of all slices, where each slice
/// is just a list of triangles id's that are related to
/// that specified slice.
typedef std::vector<TriangleIndices> SliceTable;


// Bring over from mgl.cc
Scalar AreaSign(const Vector2 &a, const Vector2 &b, const Vector2 &c);
bool convexVertex(const Vector2 &i, const Vector2 &j, const Vector2 &k);

std::ostream& operator << (std::ostream &os,const Vector2 &pt);
std::ostream& operator << (std::ostream& os, const Vector3& v);




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

	unsigned int zToLayerAbove(Scalar z) const
	{
		if(z < 0)
		{
			LayerException mixup("Model with points below the z axis are not supported in this version. Please center your model on the build area");
			throw mixup;
		}

		if (z < firstLayerZ)
			return 0;

		Scalar tol = 0.00000000000001; // tolerance
		Scalar layer = (z+tol-firstLayerZ) / layerH;
		return (unsigned int)ceil(layer);
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
typedef std::vector<Vector2> Polygon;

/// Verifies each Vector2 in the passed Polygon are in tolerance
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
