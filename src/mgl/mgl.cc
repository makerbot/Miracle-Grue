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

#include "meshy.h"
#include "shrinky.h"
#include "scadtubefile.h"



using namespace mgl;
using namespace std;




std::string mgl::getMiracleGrueVersionStr()
{
    return  "v 0.01 alpha";
}

std::ostream& mgl::operator<<(ostream& os, const mgl::Vector3& v)
{
	os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Polygon& polygon)
{
	for(unsigned int i=0; i< polygon.size(); i++)
	{
		os << i << ") " << polygon[i] << endl;
	}
	return os;
}



void mgl::rotatePolygon(Polygon& polygon, Scalar angle)
{
	for(unsigned int i=0; i<polygon.size(); i++)
	{
		const Vector2 &p = polygon[i];
		polygon[i] = p.rotate2d(angle);
	}
}

void mgl::rotatePolygons(Polygons& polygons, Scalar angle)
{
	for(unsigned int i=0; i<polygons.size(); i++)
	{
		Polygon& polygon = polygons[i];
		rotatePolygon(polygon, angle);
	}
}


void dumpSegments(const char* prefix, const std::vector<LineSegment2> &segments)
{
	cout << prefix << "segments = [ // " << segments.size() << " segments" << endl;
    for(size_t id = 0; id < segments.size(); id++)
    {
    	LineSegment2 seg = segments[id];
    	cout << prefix << " [[" << seg.a << ", " << seg.b << "]], // " << id << endl;
    }
    cout << prefix << "]" << endl;
    cout << "// color([1,0,0.4,1])loop_segments(segments,0.050000);" << endl;
}

void dumpInsets(const std::vector<SegmentTable> &insetsForLoops)
{
	for (unsigned int i=0; i < insetsForLoops.size(); i++)
	{
		const SegmentTable &insetTable =  insetsForLoops[i];
		cout << "Loop " << i << ") " << insetTable.size() << " insets"<<endl;

		for (unsigned int i=0; i <insetTable.size(); i++)
		{
			const std::vector<LineSegment2 >  &loop = insetTable[i];
			cout << "   inset " << i << ") " << loop.size() << " segments" << endl;
			dumpSegments("        ",loop);
		}
	}
}



std::ostream& mgl::operator<<(ostream& os, const Limits& l)
{
	os << "[" << l.xMin << ", " << l.yMin << ", " << l.zMin << "] [" << l.xMax << ", " << l.yMax << ", "<< l.zMax  << "]";
	return os;
}

ostream& mgl::operator <<(ostream &os,const Vector2 &pt)
{
    os << "[" << pt.x << ", " << pt.y << "]";
    return os;
}



//void mgl::writeMeshyToStl(mgl::Meshy &meshy, const char* filename)
//{
//	meshy.writeStlFile(filename);
//}


std::string mgl::stringify(double x)
{
  std::ostringstream o;
  if (!(o << x))
    throw Exception("stringify(double)");
  return o.str();
}

std::string mgl::stringify(size_t x)
{
  std::ostringstream o;
  if (!(o << x))
    throw Exception("stringify(double)");
  return o.str();
}

#ifdef WIN32
#pragma warning(disable:4996)
#endif

/**
 * Loads an STL file into a mesh object, from a binary or ASCII stl file.
 *
 * @param meshy a fully constructed mesh object
 * @param filename target file to load into the specified mesh
 *
 * @returns count of triangles loaded into meshy by this call
 */
//size_t mgl::loadMeshyFromStl(mgl::Meshy &meshy, const char* filename)
//{
//
//	// NOTE: for stl legacy read-in reasons, we are using floats here,
//	// instead of our own Scalar type
//	struct vertexes_t {
//		float nx, ny, nz;
//		float x1, y1, z1;
//		float x2, y2, z2;
//		float x3, y3, z3;
//		uint16_t attrBytes;
//	};
//
//	union {
//		struct vertexes_t vertexes;
//		uint8_t bytes[sizeof(vertexes_t)];
//	} tridata;
//
//	union
//	{
//		uint32_t intval;
//		uint16_t shortval;
//		uint8_t bytes[4];
//	} intdata;
//
//	size_t facecount = 0;
//
//	uint8_t buf[512];
//	FILE *fHandle = fopen(filename, "rb");
//	if (!fHandle)
//	{
//		string msg = "Can't open \"";
//		msg += filename;
//		msg += "\". Check that the file name is correct and that you have sufficient privileges to open it.";
//		MeshyException problem(msg.c_str());
//		throw (problem);
//	}
//
//	if (fread(buf, 1, 5, fHandle) < 5) {
//		string msg = "\"";
//		msg += filename;
//		msg += "\" is empty!";
//		MeshyException problem(msg.c_str());
//		throw (problem);
//	}
//	bool isBinary = true;
//	if (!strncasecmp((const char*) buf, "solid", 5)) {
//		isBinary = false;
//	}
//	if (isBinary) {
//		// Binary STL file
//		// Skip remainder of 80 character comment field
//		if (fread(buf, 1, 75, fHandle) < 75) {
//			string msg = "\"";
//			msg += filename;
//			msg += "\" is not a valid stl file";
//			MeshyException problem(msg.c_str());
//			throw (problem);
//		}
//		// Read in triangle count
//		if (fread(intdata.bytes, 1, 4, fHandle) < 4) {
//			string msg = "\"";
//			msg += filename;
//			msg += "\" is not a valid stl file";
//			MeshyException problem(msg.c_str());
//			throw (problem);
//		}
//		convertFromLittleEndian32(intdata.bytes);
//		uint32_t tricount = intdata.intval;
//		int countdown = (int)tricount;
//		while (!feof(fHandle) && countdown-- > 0) {
//			if (fread(tridata.bytes, 1, 3 * 4 * 4 + 2, fHandle) < 3 * 4 * 4 + 2) {
//				std::cout << __FUNCTION__ << "BREAKING" << endl;
//				break;
//			}
//			for (int i = 0; i < 3 * 4; i++) {
//				convertFromLittleEndian32(tridata.bytes + i * 4);
//			}
//			convertFromLittleEndian16((uint8_t*) &tridata.vertexes.attrBytes);
//
//			vertexes_t &v = tridata.vertexes;
//			Vector3 pt1(v.x1, v.y1, v.z1);
//			Vector3 pt2(v.x2, v.y2, v.z2);
//			Vector3 pt3(v.x3, v.y3, v.z3);
//
//			Triangle3 triangle(pt1, pt2, pt3);
//			meshy.addTriangle(triangle);
//
//			facecount++;
//		}
//
//		/// Throw removed to continue coding progress. We may not expect all
//		/// triangles to load, depending on situation. Needs debugging/revision
//		if(meshy.triangleCount() != tricount) {
//			string msg = "Warning: triangle count err in \"";
//			msg += filename;
//			msg += "\".  Expected: ";
//			msg += stringify((size_t)tricount);
//			msg += ", Read:";
//			msg += stringify(meshy.triangleCount());
//			msg += ", faced:";
//			msg += stringify(facecount);
//			std::cout << msg;
////			MeshyException problem(msg.c_str());
////			throw (problem);
//		}
//
//
//	} else {
//		// ASCII STL file
//		// Gobble remainder of solid name line.
//		fgets((char*) buf, sizeof(buf), fHandle);
//		while (!feof(fHandle)) {
//			fscanf(fHandle, "%80s", buf);
//			if (!strcasecmp((char*) buf, "endsolid")) {
//				break;
//			}
//			vertexes_t &v = tridata.vertexes;
//			bool success = true;
//			if (fscanf(fHandle, "%*s %f %f %f", &v.nx, &v.ny, &v.nz) < 3)
//				success = false;
//			if (fscanf(fHandle, "%*s %*s") < 0)
//				success = false;
//			if (fscanf(fHandle, "%*s %f %f %f", &v.x1, &v.y1, &v.z1) < 3)
//				success = false;
//			if (fscanf(fHandle, "%*s %f %f %f", &v.x2, &v.y2, &v.z2) < 3)
//				success = false;
//			if (fscanf(fHandle, "%*s %f %f %f", &v.x3, &v.y3, &v.z3) < 3)
//				success = false;
//			if (fscanf(fHandle, "%*s")< 0)
//				success = false;
//			if (fscanf(fHandle, "%*s")< 0)
//				success = false;
//			if(!success)
//			{
//				stringstream msg;
//				msg << "Error reading face " << facecount << " in file \"" << filename << "\"";
//				MeshyException problem(msg.str().c_str());
//				cout << msg.str().c_str()<< endl;
//				cout << buf << endl;
//				throw(problem);
//			}
//			Triangle3 triangle(Vector3(v.x1, v.y1, v.z1),	Vector3(v.x2, v.y2, v.z2),	Vector3(v.x3, v.y3, v.z3));
//			meshy.addTriangle(triangle);
//
//			facecount++;
//		}
//	}
//	fclose(fHandle);
//	return meshy.triangleCount();
//}


/**
 * Assuming the 2d points are on a plane, and that point order indicates a
 * vector out of that plane, returns magnitude of that vector.
 *  See Also: Right-hand-rule
 * Ex: ((0,0)(0,1)(1,0))  returns -1 (normal points negative Z out of plane)
 * Ex: ((1,0)(0,0)(0,1))  returns 1 (normal points positive Z out of plane)
 */
Scalar mgl::AreaSign(const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
	Scalar area2;
    area2 = (b[0] - a[0] ) * (Scalar)( c[1] - a[1]) -
            (c[0] - a[0] ) * (Scalar)( b[1] - a[1]);

    return area2;
}


/**
 * @returns true if the triangle of these vectors has a negative index,
 * false otherwise
 */
bool mgl::convexVertex(const Vector2 &i, const Vector2 &j, const Vector2 &k)
{
	return AreaSign(i,j,k) < 0;
}

std::ostream& mgl::operator << (std::ostream &os, const LineSegment2 &s)
{
	os << "[ " << s.a << ", " << s.b << "]";
	return os;
}

/**
 * @returns true if the passed line segments are colinear within the tolerance tol
 */
bool mgl::collinear(const LineSegment2 &prev, const LineSegment2 &current, Scalar tol, Vector2 &mid)
{

	Scalar x1 = prev.a[0];
	Scalar y1 = prev.a[1];
	mid.x = 0.5 * (prev.b[0] + current.a[0]);
	mid.y = 0.5 * (prev.b[1] + current.a[1]);
	Scalar x3 = current.b[0];
	Scalar y3 = current.b[1];

	Scalar c = ((mid.x - x1) * (y3 - y1) - (x3 - x1) * (mid.y - y1));
	bool r = tequals(c, 0, tol);
	return r;
}

/**
 * @returns a new LineSegment2, elongated to be normalized to a unit vector
 */
LineSegment2 mgl::elongate(const LineSegment2 &s, Scalar dist)
{
	LineSegment2 segment(s);
	Vector2 l = segment.b - segment.a;
	l.normalise();
	l *= dist;
	segment.b += l;
	return segment;
}

/**
 * @returns a new line segment. Of what, I don't know. Wasn't documented.
 */
LineSegment2 mgl::prelongate(const LineSegment2 &s, Scalar dist)
{
	LineSegment2 segment(s);
	Vector2 l = segment.a - segment.b;
	l.normalise();
	l *= dist;
	segment.a += l;
	return segment;
}



/// Verifies each Vector2 in the passed Polygon are in tolerance
// tol
bool mgl::tequalsPolygonCompare(Polygon& poly1, Polygon& poly2, Scalar tol)
{
	if( poly1.size() != poly2.size())
		return false;
	if( (void*)&poly1 == (void*)&poly2 )
		return true;
	size_t size =  poly1.size();
	for(size_t i = 0; i < size; i++ ) {
		if (false == poly1[i].tequals(poly2[i],tol) )
			return false;
	}
	return true;
}


/// Verifies each Polygon in the passed Polygons are in tolerance
bool mgl::tequalsPolygonsCompare(Polygons& polys1, Polygons& polys2, Scalar tol)
{
	if( polys1.size() != polys2.size())
		return false;
	if( (void*)&polys1 == (void*)&polys2 )
		return true;
	size_t size =  polys1.size();
	for(size_t i = 0; i < size; i++ ) {
		Polygon p0 = polys1[i];
		Polygon p1 = polys2[i];

		bool same = tequalsPolygonCompare(p0,p1,tol);
		if ( !same  )
			return false;
	}
	return true;
}
