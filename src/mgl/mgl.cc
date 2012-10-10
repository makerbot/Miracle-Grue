/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

 */

#include <iomanip>
#include <set>
#include <fstream>


#include <stdint.h>
#include <cstring>
#include <map>

#include <json/reader.h>
#include <json/writer.h>
#include <json/value.h>
#include <json/writer.h>

#include "meshy.h"
#include "shrinky.h"
#include "ScadDebugFile.h"



namespace mgl {

using namespace std;
using namespace libthing;

const Scalar LayerMeasure::INVALID_SCALAR = std::numeric_limits<Scalar>::min();

string getMiracleGrueProgramName() {
    return "MiracleGrue Turboencabulator";
}
string getMiracleGrueVersionStr() {
	return "v 0.04";
}

ostream& operator<<(ostream& os, const Point3Type& v) {
	os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
	return os;
}

ostream& operator<<(std::ostream& os, const Polygon& polygon) {
	for (unsigned int i = 0; i < polygon.size(); i++) {
		os << i << ") " << polygon[i] << endl;
	}
	return os;
}

void rotatePolygon(Polygon& polygon, Scalar angle) {
	for (unsigned int i = 0; i < polygon.size(); i++) {
		const Vector2 &p = polygon[i];
		polygon[i] = p.rotate2d(angle);
	}
}

void rotatePolygons(Polygons& polygons, Scalar angle) {
	for (unsigned int i = 0; i < polygons.size(); i++) {
		Polygon& polygon = polygons[i];
		rotatePolygon(polygon, angle);
	}
}

LayerMeasure::LayerAttributes::LayerAttributes(Scalar d, Scalar t, Scalar wr ,
		layer_measure_index_t b) 
		: delta(d), thickness(t), widthRatio(wr), base(b) {}
bool LayerMeasure::LayerAttributes::isAbsolute() const {
	return base < 0;
}
LayerMeasure::LayerMeasure(Scalar firstLayerZ, Scalar layerH, Scalar widthRatio) 
		: firstLayerZ(firstLayerZ), layerH(layerH), 
		layerWidthRatio(widthRatio), issuedIndex(256) {
	attributes[0] = LayerAttributes(0, 0, widthRatio);
	attributes[0].base = -1;
}
layer_measure_index_t LayerMeasure::zToLayerAbove(Scalar z) const {
	Scalar const tol = 0.000001; // tolerance: 1 nanometer
	if (libthing::tlower(z, firstLayerZ, tol))
		return 0;
	Scalar const layer = (z + tol - firstLayerZ) / layerH;
	return static_cast<layer_measure_index_t> (ceil(layer));
}

Scalar LayerMeasure::sliceIndexToHeight(layer_measure_index_t sliceIndex) const {
	return firstLayerZ + sliceIndex * layerH;
}

Scalar LayerMeasure::getLayerH() const {
	return layerH;
}
Scalar LayerMeasure::getLayerW() const {
	return layerH * layerWidthRatio;
}
Scalar LayerMeasure::getLayerWidthRatio() const {
	return layerWidthRatio;
}
void LayerMeasure::setLayerH(Scalar h) {
	layerH = h;
}
void LayerMeasure::setLayerWidthRatio(Scalar wr) {
	layerWidthRatio = wr;
}
const LayerMeasure::LayerAttributes& LayerMeasure::getLayerAttributes(
		layer_measure_index_t layerIndex) const {
	attributesMap::const_iterator iter = attributes.find(layerIndex);
	if(iter == attributes.end()){
		stringstream msg;
		msg << "Unable to find attributes for layer index " << layerIndex;
		LayerException mixup = msg.str();
		throw mixup;
	}
	return iter->second;
}
LayerMeasure::LayerAttributes& LayerMeasure::getLayerAttributes(
		layer_measure_index_t layerIndex) {
	attributesMap::iterator iter = attributes.find(layerIndex);
	if(iter == attributes.end()){
		stringstream msg;
		msg << "Unable to find attributes for layer index " << layerIndex;
		LayerException mixup = msg.str();
		throw mixup;
	}
	return iter->second;
}
Scalar LayerMeasure::getLayerPosition(layer_measure_index_t layerIndex) const {
	if(layerIndex < 0)
		return 0.0;
	const LayerAttributes& currentAttribs = getLayerAttributes(layerIndex);
	return currentAttribs.delta + getLayerPosition(currentAttribs.base);
}
Scalar LayerMeasure::getLayerThickness(layer_measure_index_t layerIndex) const {
	return getLayerAttributes(layerIndex).thickness;
}
Scalar LayerMeasure::getLayerWidth(layer_measure_index_t layerIndex) const {
	const LayerAttributes& currentAttribs = getLayerAttributes(layerIndex);
	return currentAttribs.thickness * currentAttribs.widthRatio;
}
layer_measure_index_t LayerMeasure::createAttributes(
		const LayerAttributes& attribs) {
	attributes[issuedIndex] = attribs;
	return issuedIndex++;
}

ostream& operator<<(ostream& os, const Limits& l) {
	os << "[" << l.xMin << ", " << l.yMin << ", " << l.zMin << "] [" 
			<< l.xMax << ", " << l.yMax << ", " << l.zMax << "]";
	return os;
}

ostream& operator <<(ostream &os, const Vector2 &pt) {
	os << "[" << pt.x << ", " << pt.y << "]";
	return os;
}

void exceptionToJson(std::ostream& os, const Exception& mixup, bool warn) {
    Json::Value msg(Json::objectValue);
    msg["type"] = "message";
    msg[(warn ? "warning" : "error")] = mixup.error;

    Json::FastWriter writer;
    os << writer.write(msg);
}
void exceptionToJson(std::ostream& os, const std::string& mixup, bool warn) {
    Json::Value msg(Json::objectValue);
    msg["type"] = "message";
    msg[(warn ? "warning" : "error")] = mixup;

    Json::FastWriter writer;
    os << writer.write(msg);
}

#if defined WIN32 && defined _MSVC_VER
#pragma warning(disable:4996)
#endif

/**
 * Assuming the 2d points are on a plane, and that point order indicates a
 * vector out of that plane, returns magnitude of that vector.
 *  See Also: Right-hand-rule
 * Ex: ((0,0)(0,1)(1,0))  returns -1 (normal points negative Z out of plane)
 * Ex: ((1,0)(0,0)(0,1))  returns 1 (normal points positive Z out of plane)
 */
Scalar AreaSign(const Vector2 &a, const Vector2 &b, const Vector2 &c) {
	Scalar area2;
	area2 = (b[0] - a[0]) * (Scalar) (c[1] - a[1]) -
			(c[0] - a[0]) * (Scalar) (b[1] - a[1]);

	return area2;
}

/**
 * @returns true if the triangle of these vectors has a negative index,
 * false otherwise
 */
bool convexVertex(const Vector2 &i, const Vector2 &j, const Vector2 &k) {
	return AreaSign(i, j, k) < 0;
}

//std::ostream& mgl::operator << (std::ostream &os, const LineSegment2 &s)
//{
//	os << "[ " << s.a << ", " << s.b << "]";
//	return os;
//}

/**
 * @returns true if the passed line segments are colinear within the tolerance tol
 */
bool collinear(const Segment2Type &prev, const Segment2Type &current,
		Scalar tol, Vector2 &mid) {

	Scalar x1 = prev.a[0];
	Scalar y1 = prev.a[1];
	mid.x = 0.5 * (prev.b[0] + current.a[0]);
	mid.y = 0.5 * (prev.b[1] + current.a[1]);
	Scalar x3 = current.b[0];
	Scalar y3 = current.b[1];

	Scalar c = ((mid.x - x1) * (y3 - y1) - (x3 - x1) * (mid.y - y1));
	bool r = libthing::tequals(c, 0, tol);
	return r;
}



/// Verifies each Vector2 in the passed Polygon are in tolerance
// tol

bool tequalsPolygonCompare(Polygon& poly1, Polygon& poly2, Scalar tol) {
	if (poly1.size() != poly2.size())
		return false;
	if ((void*) &poly1 == (void*) &poly2)
		return true;
	size_t size = poly1.size();
	for (size_t i = 0; i < size; i++) {
		if (false == poly1[i].tequals(poly2[i], tol))
			return false;
	}
	return true;
}


/// Verifies each Polygon in the passed Polygons are in tolerance

bool tequalsPolygonsCompare(Polygons& polys1, Polygons& polys2, Scalar tol) {
	if (polys1.size() != polys2.size())
		return false;
	if ((void*) &polys1 == (void*) &polys2)
		return true;
	size_t size = polys1.size();
	for (size_t i = 0; i < size; i++) {
		Polygon p0 = polys1[i];
		Polygon p1 = polys2[i];

		bool same = tequalsPolygonCompare(p0, p1, tol);
		if (!same)
			return false;
	}
	return true;
}


}


