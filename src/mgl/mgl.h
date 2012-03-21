/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#ifndef CORE_H_
#define CORE_H_

#include <cmath>
#include <vector>
#include <iostream>
#include <cassert>

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

/**
 * base class for all MGL Exceptions
 */
class Exception
{

public:
	std::string error;
	Exception(const char *msg) :error(msg)
	{
		//	std::cerr << std::endl << msg << std::endl;
		// fprintf(stderr, "%s", msg);
	}

};

//////////
// Scalar: Our basic numerical type. double for now;
///////////
typedef double Scalar;
#define SCALAR_SQRT(s) sqrt(s)
#define SCALAR_ABS(s) abs(s)
#define SCALAR_ACOS(s) acos(s)
#define SCALAR_SIN(s) sin(s)
#define SCALAR_COS(s) cos(s)

// See float.h for details on these
#define SCALAR_EPSILON DBL_EPSILON

/** (t)olerance (equals)
 * @returns true if two Scalar values are approximally the same using tolernce
 */
bool tequals(Scalar a, Scalar b, Scalar tol); // = 1e-8

//////////
// Scalar: End numeric type info
///////////

// Type used for indexes of triangles/etc for unique indexing
typedef unsigned int index_t;

/// Structure contains list of triangle 'id's, used to
/// reference which triangle in the master list is related.
typedef std::vector<index_t> TriangleIndices;

/// A list of all slices, where each slice
/// is just a list of triangles id's that are related to
/// that specified slice.
typedef std::vector<TriangleIndices> SliceTable;

#include "Vector2.cc"

std::ostream& operator << (std::ostream &os,const Vector2 &pt);

#include "LineSegment2.cc"

/// List of Lists of line segments. Used to lookup
/// A SegmentTable may contain, for example, a perimeter
/// and hole(s) in that perimeter of a slice.
typedef std::vector< std::vector<LineSegment2 > > SegmentTable;

std::ostream& operator << (std::ostream &os, const LineSegment2 &s);


#include "Vector3.cc"

std::ostream& operator<<(std::ostream& os, const Vector3& v);


#include "Triangle3.cc"





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


//
////
//// The Slice is a series of tubes
////
//// tubes are plastic extrusions
//class TubesInSlice
//{
//public:
//	TubesInSlice(Scalar z)
//		:z(z)
//	{
//	}
//
//	Scalar z;
//	std::vector<LineSegment2d> infill;
//	std::vector< std::vector<LineSegment2d> > outlines;
//};

/// A polygon is an arbitarty collection of 2d points
typedef std::vector<Vector2> Polygon;

/// A vector of polygon objects
typedef std::vector<Polygon> Polygons;

std::ostream& operator<<(std::ostream& os, const Polygon& v);


} // namespace

#endif
