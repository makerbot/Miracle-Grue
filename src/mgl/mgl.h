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
#include <map>
#include <sstream>

#include "Exception.h"
#include "libthing/Scalar.h"
#include "libthing/Vector2.h"
#include "libthing/Vector3.h"
#include "libthing/LineSegment2.h"
#include "libthing/Triangle3.h"


//#define M_TAU M_PI*2

// #define STRONG_CHECKING

#define dbg__ std::cout <<  __FUNCTION__ << "::" << __LINE__  << "*" << std::endl;
#define dbgs__(s) std::cout <<  __FUNCTION__ << "::" << __LINE__  << " > "<< s << std::endl;


namespace mgl {

static const Scalar M_TAU = M_PI * 2;

std::string getMiracleGrueVersionStr();

/// Structure contains list of triangle 'id's, used to
/// reference which triangle in the master list is related.
typedef std::vector<index_t> TriangleIndices;

/// A list of all slices, where each slice
/// is just a list of triangles id's that are related to
/// that specified slice.
typedef std::vector<TriangleIndices> SliceTable;

typedef int layer_index_t;


// Bring over from mgl.cc
Scalar AreaSign(const libthing::Vector2& a,
		const libthing::Vector2& b,
		const libthing::Vector2& c);
bool convexVertex(const libthing::Vector2& i,
		const libthing::Vector2& j,
		const libthing::Vector2& k);

std::ostream& operator <<(std::ostream& os,
		const libthing::Vector2& pt);
std::ostream& operator <<(std::ostream& os,
		const libthing::Vector3& v);
//std::ostream& operator<<(std::ostream& os, libthing::LineSegment2 const& line);

bool collinear(const libthing::LineSegment2 &prev,
		const libthing::LineSegment2 &current,
		Scalar tol, libthing::Vector2 &mid);

class LayerException : public Exception {
public:

	LayerException(const char *msg) : Exception(msg) {}
	LayerException(const std::string& msg) : Exception(msg) {}
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

class LayerMeasure {
public:

	class LayerAttributes {
	public:
		LayerAttributes(Scalar p = 0., Scalar t = 0.27);
		Scalar position; // Z position
		Scalar thickness; // Height of filament
		//Scalar width;		// Width of filament
	};

	typedef std::map<layer_index_t, LayerAttributes> attributesMap;
	/* Old interface */
	LayerMeasure(Scalar firstLayerZ, Scalar layerH);
	layer_index_t zToLayerAbove(Scalar z) const;
	Scalar sliceIndexToHeight(layer_index_t layerIndex) const;
	Scalar getLayerH() const;
	
	/* New interface */
	LayerAttributes getLayerAttributes(layer_index_t layerIndex) const;
	Scalar getLayerPosition(layer_index_t layerIndex) const;
	Scalar getLayerThickness(layer_index_t layerIndex) const;
	void setLayerAttributes(layer_index_t layerIndex, 
			const LayerAttributes& attribs);
	

private:

	Scalar firstLayerZ;
	Scalar layerH;

	attributesMap attributes;
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

template <typename T>
std::string stringify(const T& x) {
	std::ostringstream o;
	if (!(o << x))
		throw Exception("stringify(generic)");
	return o.str();
}


} // namespace

#endif
