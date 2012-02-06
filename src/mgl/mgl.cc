/**
   MiracleGrue - Model Generator for toolpathing. <http://www.grue.makerbot.com>
   Copyright (C) 2011 Far McKon <Far@makerbot.com>, Hugo Boyer (hugo@makerbot.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

*/


#include "meshy.h"
#include "shrinky.h"
#include "scadtubefile.h"

#include <stdint.h>
#include <cstring>

using namespace mgl;
using namespace std;


bool mgl::sameSame(Scalar a, Scalar b, Scalar tol)
{
	return (a-b) * (a-b) < tol;
}



// returns the angle between 2 vectors
Scalar mgl::angleFromVector2s(const Vector2 &a, const Vector2 &b)
{
	Scalar dot = a.dotProduct(b);
	Scalar d1 = a.magnitude();
	Scalar d2 = b.magnitude();
	Scalar cosTheta = dot / (d1 * d2);
	if (cosTheta >  1.0) cosTheta  = 1;
	if (cosTheta < -1.0) cosTheta = -1;
	Scalar theta = M_PI - acos(cosTheta);
	return theta;
}

// returns the angle between 3 points
Scalar mgl::angleFromPoint2s(const Vector2 &i, const Vector2 &j, const Vector2 &k)
{
	Vector2 a = i - j;
	Vector2 b = j - k;
	Scalar theta = angleFromVector2s(a,b);
	return theta;
}

bool sliceTriangle(const Vector3& vertex1,
					 const Vector3& vertex2,
						const Vector3& vertex3,
						   Scalar Z,
						   Vector3 &a,
						   	   Vector3 &b)
{
	Scalar tol = 1e-6;

	Scalar u, px, py, v, qx, qy;
	if (vertex1.z > Z && vertex2.z > Z && vertex3.z > Z)
	{
		// Triangle is above Z level.
		return false;
	}
	if (vertex1.z < Z && vertex2.z < Z && vertex3.z < Z)
	{
		// Triangle is below Z level.
		return false;
	}
	if (sameSame(vertex1.z, Z, tol) )
	{
		if (sameSame(vertex2.z,Z, tol) )
		{
			if (sameSame(vertex3.z,Z, tol) )
			{
				// flat face.  Ignore.
				return false;
			}
			//lnref = Line(Point(vertex1), Point(vertex2));
			a.x = vertex1.x;
			a.y = vertex1.y;
			a.z = Z;
			b.x = vertex2.x;
			b.y = vertex2.y;
			b.z = Z;
			return true;
		}
		if (sameSame(vertex3.z,Z, tol) )
		{
			// lnref = Line(Point(vertex1), Point(vertex3));
			a.x = vertex1.x;
			a.y = vertex1.y;
			a.z = Z;

			b.x = vertex3.x;
			b.y = vertex3.y;
			b.z = Z;
			return true;
		}
		if ((vertex2.z > Z && vertex3.z > Z) || (vertex2.z < Z && vertex3.z < Z))
		{
			// only touches vertex1 tip.  Ignore.
			return false;
		}
		u = (Z-vertex2.z)/(vertex3.z-vertex2.z);
		px =  vertex2.x+u*(vertex3.x-vertex2.x);
		py =  vertex2.y+u*(vertex3.y-vertex2.y);
		// lnref = Line(Point(vertex1), Point(px,py));
		a.x = vertex1.x;
		a.y = vertex1.y;
		a.z = Z;
		b.x = px;
		b.y = py;
		b.z = Z;
		return true;
	}
	else if (sameSame(vertex2.z, Z, tol) )
	{
		if (sameSame(vertex3.z,Z, tol) )
		{
			// lnref = Line(Point(vertex2), Point(vertex3));
			a.x = vertex2.x;
			a.y = vertex2.y;
			a.z = Z;
			b.x = vertex3.x;
			b.y = vertex3.y;
			b.z = Z;
			return true;
		}
		if ((vertex1.z > Z && vertex3.z > Z) || (vertex1.z < Z && vertex3.z < Z))
		{
			// only touches vertex2 tip.  Ignore.
			return false;
		}
		u = (Z-vertex1.z)/(vertex3.z-vertex1.z);
		px =  vertex1.x+u*(vertex3.x-vertex1.x);
		py =  vertex1.y+u*(vertex3.y-vertex1.y);
		// lnref = Line(Point(vertex2), Point(px,py));
		a.x = vertex2.x;
		a.y = vertex2.y;
		a.z = Z;
		b.x = px;
		b.y = py;
		b.z = Z;
		return true;
	}
	else if (sameSame(vertex3.z, Z, tol) )
	{
		if ((vertex1.z > Z && vertex2.z > Z) || (vertex1.z < Z && vertex2.z < Z))
		{
			// only touches vertex3 tip.  Ignore.
			return false;
		}
		u = (Z-vertex1.z)/(vertex2.z-vertex1.z);
		px =  vertex1.x+u*(vertex2.x-vertex1.x);
		py =  vertex1.y+u*(vertex2.y-vertex1.y);
		// lnref = Line(Point(vertex3), Point(px,py));
		a.x = vertex3.x;
		a.y = vertex3.y;
		a.z = Z;
		b.x = px;
		b.y = py;
		b.z = Z;
		return true;
	}
	else if ((vertex1.z > Z && vertex2.z > Z) || (vertex1.z < Z && vertex2.z < Z))
	{
		u = (Z-vertex3.z)/(vertex1.z-vertex3.z);
		px =  vertex3.x+u*(vertex1.x-vertex3.x);
		py =  vertex3.y+u*(vertex1.y-vertex3.y);
		v = (Z-vertex3.z)/(vertex2.z-vertex3.z);
		qx =  vertex3.x+v*(vertex2.x-vertex3.x);
		qy =  vertex3.y+v*(vertex2.y-vertex3.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		a.z = Z;
		b.x = qx;
		b.y = qy;
		b.z = Z;
		return true;
	}
	else if ((vertex1.z > Z && vertex3.z > Z) || (vertex1.z < Z && vertex3.z < Z))
	{
		u = (Z-vertex2.z)/(vertex1.z-vertex2.z);
		px =  vertex2.x+u*(vertex1.x-vertex2.x);
		py =  vertex2.y+u*(vertex1.y-vertex2.y);
		v = (Z-vertex2.z)/(vertex3.z-vertex2.z);
		qx =  vertex2.x+v*(vertex3.x-vertex2.x);
		qy =  vertex2.y+v*(vertex3.y-vertex2.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		a.z = Z;
		b.x = qx;
		b.y = qy;
		b.z = Z;
		return true;
	}
	else if ((vertex2.z > Z && vertex3.z > Z) || (vertex2.z < Z && vertex3.z < Z))
	{
		u = (Z-vertex1.z)/(vertex2.z-vertex1.z);
		px =  vertex1.x+u*(vertex2.x-vertex1.x);
		py =  vertex1.y+u*(vertex2.y-vertex1.y);
		v = (Z-vertex1.z)/(vertex3.z-vertex1.z);
		qx =  vertex1.x+v*(vertex3.x-vertex1.x);
		qy =  vertex1.y+v*(vertex3.y-vertex1.y);
		// lnref = Line(Point(px,py), Point(qx,qy));
		a.x = px;
		a.y = py;
		a.z = Z;
		b.x = qx;
		b.y = qy;
		b.z = Z;
		return true;
	}
	return false;
}

bool mgl::Triangle3::cut(Scalar z, Vector3 &a, Vector3 &b) const
{

	Vector3 dir = cutDirection();


	bool success = sliceTriangle(v0,v1,v2, z, a, b );

	Vector3 segmentDir = b - a;
	if(dir.dotProduct(segmentDir) < 0 )
	{
		Vector3 p(a);
		a = b;
		b = p;
	}
	return success;
}

std::ostream& mgl::operator<<(ostream& os, const mgl::Vector3& v)
{
	os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
	return os;
}

// given a point, finds the LineSegment2 that starts the closest from that point
// and return the distance. Also, the iterator to the closest LineSegment2 is "returned"
Scalar findClosestLineSegment2(const Vector2& endOfPreviousLineSegment2,
						vector<TriangleSegment2>::iterator startIt,
						vector<TriangleSegment2>::iterator endIt,
						vector<TriangleSegment2>::iterator &bestSegmentIt ) // "returned here"
{
	bestSegmentIt = endIt; 	// just in case, we'll check for this on the caller side
	Scalar minDist = 1e100; 
	
	Vector3 end(endOfPreviousLineSegment2.x,endOfPreviousLineSegment2.y, 0);
	vector<TriangleSegment2>::iterator it = startIt;
	while(it != endIt)
	{
		Vector3 start(it->a.x, it->a.y, 0);
		Vector3 v = end-start;
		Scalar distance = v.squaredMagnitude();
		if (distance < minDist)
		{
			minDist = distance;
			bestSegmentIt = it;
			// we could decide to stop here ... if we had a threshold
		}
		it ++;
	}
	return minDist;
}

void mgl::loopsAndHoleOgy(std::vector<TriangleSegment2> &segments,
		Scalar tol,
		std::vector< std::vector<TriangleSegment2> > &loops)
{
	// Lets sort this mess out so we can extrude in a continuous line of shiny contour
	// Nota: from their normals (int their previous life as 3d triangles), LineSegment2 know their beginings from their endings
	
	std::vector<Scalar> distances;
	distances.reserve(segments.size());
	
	distances.push_back(0); // this value is not used, it represents the distance between the
							// first LineSegment2 and the one before (and there is no LineSegment2 before)
	for(vector<TriangleSegment2>::iterator i = segments.begin(); i != segments.end(); i++)
	{
		Vector2 &startingPoint = i->b;
		vector<TriangleSegment2>::iterator startIt = i+1;
		vector<TriangleSegment2>::iterator bestSegmentIt;
		if(startIt != segments.end())
		{
			Scalar distance = findClosestLineSegment2(startingPoint, startIt, segments.end(), bestSegmentIt);
			if(bestSegmentIt != segments.end())
			{
				// Swap the segments, because the best is the closest segment to the current one
				TriangleSegment2 tmp(*startIt);
				TriangleSegment2 best(*bestSegmentIt);
				*startIt = best;
				*bestSegmentIt = tmp;
				distances.push_back(distance);

			}
		}
	}
	
	// we now have an optimal sequence of LineSegment2s (except we didn't optimise for interloop traversal).
	// we also have a hop (distances) between each LineSegment2 pair
		

	vector<Scalar>::iterator hopIt = distances.begin();
	vector<TriangleSegment2>::iterator i = segments.begin();
	while(i != segments.end())
	{
		// lets make a loop... we'll call it loop
		loops.push_back(vector<TriangleSegment2>());
		vector<TriangleSegment2> &loop = loops[loops.size()-1];
		
		loop.push_back(*i);

		i++;
		hopIt++;
		if(i == segments.end())
		{
			// this is sad ... a loop with a single segment
			#ifdef STRONG_CHECKING
			assert(0);
			#endif
			break;
		}
		bool thisLoopIsDone = false;
		while(!thisLoopIsDone)
		{
			Scalar distance = *hopIt;
			if(distance < tol) 
			{
				loop.push_back(*i);
				i++;
				hopIt ++;
			}
			else
			{
				thisLoopIsDone = true;
			}
			if(i == segments.end())
			{
				thisLoopIsDone = true;
			}
		}
	}
}

void mgl::segmentationOfTriangles(const TriangleIndices &trianglesForSlice,
		const std::vector<Triangle3> &allTriangles,
		Scalar z,
		std::vector<TriangleSegment2> &segments)
{
    size_t triangleCount = trianglesForSlice.size();
    segments.reserve(triangleCount);
    //#pragma omp parallel for
    for(int i = 0;i < triangleCount;i++)
    {
        index_t triangleIndex = trianglesForSlice[i];
        const Triangle3 & triangle = allTriangles[triangleIndex];
        Vector3 a, b;
        // bool cut = sliceTriangle(triangle[0], triangle[1], triangle[2], z, a, b);
        bool cut = triangle.cut(z, a, b);
        if(cut){
        	TriangleSegment2 s;
            s.a.x = a.x;
            s.a.y = a.y;
            s.b.x = b.x;
            s.b.y = b.y;
            //s.triangle = triangleIndex;

            segments.push_back(s);
        }
    }

}


void mgl::translateLoops(SegmentTable &loops, Vector2 p)
{
	for(unsigned int i=0; i < loops.size(); i++)
	{
		translateSegments(loops[i],p);
	}
}

void mgl::translatePolygons(Polygons &polygons, Vector2 p)
{
	for(unsigned int i=0; i < polygons.size(); i++)
	{
		translatePolygon(polygons[i],p);
	}
}

void mgl::translatePolygon(Polygon &polygon, Vector2 p)
{
	for(unsigned int i=0; i < polygon.size(); i++)
	{
		polygon[i] += p;
	}
}

void mgl::translateSegments(std::vector<TriangleSegment2> &segments, Vector2 p)
{
	for(int i=0; i < segments.size(); i++)
	{
		segments[i].a += p;
		segments[i].b += p;
	}
}


Vector2 mgl::rotate2d(const Vector2 &p, Scalar angle)
{
	// rotate point
	double s = sin(angle); // radians
	double c = cos(angle);
	Vector2 rotated;
	rotated.x = p.x * c - p.y * s;
	rotated.y = p.x * s + p.y * c;
	return rotated;
}

void mgl::rotatePolygon(Polygon& polygon, Scalar angle)
{
	for(unsigned int i=0; i<polygon.size(); i++)
	{
		const Vector2 &p = polygon[i];
		polygon[i] = rotate2d(p, angle);
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

void mgl::rotateLoops(std::vector<std::vector<mgl::TriangleSegment2> > &loops, Scalar angle)
{
	for(unsigned int i=0; i < loops.size(); i++)
	{
		rotateSegments(loops[i], angle);
	}
}

void mgl::rotateSegments(std::vector<TriangleSegment2> &segments, Scalar angle)
{
	for(int i=0; i < segments.size(); i++)
	{
		segments[i].a = rotate2d(segments[i].a, angle);
		segments[i].b = rotate2d(segments[i].b, angle);
	}
}

bool mgl::segmentSegmentIntersection(Scalar p0_x, Scalar p0_y,
									 Scalar p1_x, Scalar p1_y,
									 Scalar p2_x, Scalar p2_y,
									 Scalar p3_x, Scalar p3_y,
									 Scalar &i_x, Scalar &i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        i_x = p0_x + (t * s1_x);
        i_y = p0_y + (t * s1_y);
        return true;
    }

    return false; // No collision
}

bool mgl::segmentSegmentIntersection(const TriangleSegment2 &s0, const TriangleSegment2 &s1, Vector2 &p)
{

	bool s;
	s = segmentSegmentIntersection(	s0.a[0], s0.a[1],
									s0.b[0], s0.b[1],
									s1.a[0], s1.a[1],
									s1.b[0], s1.b[1],
									p.x, p.y);
	return s;
}


void dumpSegments(const char* prefix, const std::vector<TriangleSegment2> &segments)
{
	cout << prefix << "segments = [ // " << segments.size() << " segments" << endl;
    for(int id = 0; id < segments.size(); id++)
    {
    	TriangleSegment2 seg = segments[id];
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
			const std::vector<TriangleSegment2 >  &loop = insetTable[i];
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
    os.precision(3);
    os.setf(ios::fixed);
    os << "[" << pt.x << ", " << pt.y << "]";
    return os;
}


#ifdef __BYTE_ORDER
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define I_AM_LITTLE_ENDIAN
# else
#  if __BYTE_ORDER == __BIG_ENDIAN
#   define I_AM_BIG_ENDIAN
#  else
#error "Unknown byte order!"
#  endif
# endif
#endif /* __BYTE_ORDER */

#ifdef I_AM_BIG_ENDIAN
static inline void convertFromLittleEndian32(uint8_t* bytes)
{
    uint8_t tmp = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = tmp;
    tmp = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = tmp;
}
static inline void convertFromLittleEndian16(uint8_t* bytes)
{
    uint8_t tmp = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = tmp;
}
#else
static inline void convertFromLittleEndian32(uint8_t* bytes)
{
}
static inline void convertFromLittleEndian16(uint8_t* bytes)
{
}
#endif


size_t mgl::loadMeshyFromStl(mgl::Meshy &meshy, const char* filename)
{

	struct vertexes_t {
		float nx, ny, nz;
		float x1, y1, z1;
		float x2, y2, z2;
		float x3, y3, z3;
		uint16_t attrBytes;
	};

	union {
		struct vertexes_t vertexes;
		uint8_t bytes[sizeof(vertexes_t)];
	} tridata;

	union
	{
		uint32_t intval;
		uint16_t shortval;
		uint8_t bytes[4];
	} intdata;

	size_t facecount = 0;

	uint8_t buf[512];
	FILE *f = fopen(filename, "rb");
	if (!f)
	{
		string msg = "Can't open \"";
		msg += filename;
		msg += "\". Check that the file name is correct and that you have sufficient privileges to open it.";
		MeshyMess problem(msg.c_str());
		throw (problem);
	}

	if (fread(buf, 1, 5, f) < 5) {
		string msg = "\"";
		msg += filename;
		msg += "\" is empty!";
		MeshyMess problem(msg.c_str());
		throw (problem);
	}
	bool isBinary = true;
	if (!strncasecmp((const char*) buf, "solid", 5)) {
		isBinary = false;
	}
	if (isBinary) {
		// Binary STL file
		// Skip remainder of 80 character comment field
		if (fread(buf, 1, 75, f) < 75) {
			string msg = "\"";
			msg += filename;
			msg += "\" is not a valid stl file";
			MeshyMess problem(msg.c_str());
			throw (problem);
		}
		// Read in triangle count
		if (fread(intdata.bytes, 1, 4, f) < 4) {
			string msg = "\"";
			msg += filename;
			msg += "\" is not a valid stl file";
			MeshyMess problem(msg.c_str());
			throw (problem);
		}
		convertFromLittleEndian32(intdata.bytes);
		uint32_t tricount = intdata.intval;
		while (!feof(f) && tricount-- > 0) {
			if (fread(tridata.bytes, 1, 3 * 4 * 4 + 2, f) < 3 * 4 * 4 + 2) {
				break;
			}
			for (int i = 0; i < 3 * 4; i++) {
				convertFromLittleEndian32(tridata.bytes + i * 4);
			}
			convertFromLittleEndian16((uint8_t*) &tridata.vertexes.attrBytes);

			vertexes_t &v = tridata.vertexes;
			Vector3 pt1(v.x1, v.y1, v.z1);
			Vector3 pt2(v.x2, v.y2, v.z2);
			Vector3 pt3(v.x3, v.y3, v.z3);

			Triangle3 triangle(pt1, pt2, pt3);
			meshy.addTriangle(triangle);

			facecount++;
		}
		fclose(f);
	} else {
		// ASCII STL file
		// Gobble remainder of solid name line.
		fgets((char*) buf, sizeof(buf), f);
		while (!feof(f)) {
			fscanf(f, "%80s", buf);
			if (!strcasecmp((char*) buf, "endsolid")) {
				break;
			}
			vertexes_t &v = tridata.vertexes;
			bool success = true;
			if (fscanf(f, "%*s %f %f %f", &v.nx, &v.ny, &v.nz) < 3)
				success = false;
			if (fscanf(f, "%*s %*s") < 0)
				success = false;
			if (fscanf(f, "%*s %f %f %f", &v.x1, &v.y1, &v.z1) < 3)
				success = false;
			if (fscanf(f, "%*s %f %f %f", &v.x2, &v.y2, &v.z2) < 3)
				success = false;
			if (fscanf(f, "%*s %f %f %f", &v.x3, &v.y3, &v.z3) < 3)
				success = false;
			if (fscanf(f, "%*s")< 0)
				success = false;
			if (fscanf(f, "%*s")< 0)
				success = false;
			if(!success)
			{
				stringstream msg;
				msg << "Error reading face " << facecount << " in file \"" << filename << "\"";
				MeshyMess problem(msg.str().c_str());
				throw(problem);
			}
			Triangle3 triangle(Vector3(v.x1, v.y1, v.z1),	Vector3(v.x2, v.y2, v.z2),	Vector3(v.x3, v.y3, v.z3));
			meshy.addTriangle(triangle);

			facecount++;
		}
		fclose(f);
	}
	return facecount;
}




void mgl::infillPathology( SegmentTable &outlineLoops,
				const Limits& limits,
				double z,
				double tubeSpacing,
				Polygons& infills)
{
	assert(tubeSpacing != 0);

	assert(infills.size() == 0);

	Scalar deltaY = limits.yMax - limits.yMin;

	int tubeCount = (deltaY) / tubeSpacing;
	std::vector< std::set<Scalar> > intersects;
	// allocate
	intersects.resize(tubeCount);
	for (unsigned int i=0; i < tubeCount; i++)
	{
		Scalar y = -0.5 * deltaY + i * tubeSpacing;
		std::set<Scalar> &lineCuts = intersects[i];

		// go through all the segments in every loop
		for(unsigned int j=0; j< outlineLoops.size(); j++)
		{
			std::vector<TriangleSegment2> &outlineLineSegment2s = outlineLoops[j];
			for(std::vector<TriangleSegment2>::iterator it= outlineLineSegment2s.begin(); it!= outlineLineSegment2s.end(); it++)
			{
				TriangleSegment2 &segment = *it;
				Scalar intersectionX, intersectionY;
				if (segmentSegmentIntersection(limits.xMin, y,
												limits.xMax, y,
												segment.a.x, segment.a.y,
												segment.b.x, segment.b.y,
												intersectionX,  intersectionY))
				{
					lineCuts.insert(intersectionX);

				}
			}
		}
	}

	// tubes.resize(tubeCount);

	bool backAndForth = true;

	Scalar bottom = -0.5 * deltaY;
	for (int i=0; i < tubeCount; i++)
	{

		Scalar y = bottom + i * tubeSpacing;
		std::set<Scalar> &lineCuts = intersects[i];

		Vector2 begin;
		Vector2 end;
		bool inside = false;
		if( backAndForth)
		{
			for(std::set<Scalar>::iterator it = lineCuts.begin(); it != lineCuts.end(); it++)
			{
				inside =! inside;
				Scalar x = *it;

				if(inside)
				{

					begin.x = x;
					begin.y = y;
				}
				else
				{
					end.x = x;
					end.y = y;
					infills.push_back(Polygon());
					Polygon &poly = infills.back();
					poly.push_back(begin);
					poly.push_back(end);
				}
			}
		}
		else
		{
			for(std::set<Scalar>::reverse_iterator it = lineCuts.rbegin(); it != lineCuts.rend(); it++)
			{
				inside =! inside;
				Scalar x = *it;

				if(inside)
				{
					begin.x = x;
					begin.y = y;
				}
				else
				{
					end.x = x;
					end.y = y;
					infills.push_back(Polygon());
					Polygon &poly = infills.back();
					poly.push_back(begin);
					poly.push_back(end);
				}
			}
		}
		backAndForth = !backAndForth;
	}

}


// segments are OK, but polys are better for paths (no repeat point)
void segments2polygon(const std::vector<TriangleSegment2> & segments, Polygon &loop)
{

    loop.reserve(segments.size());
    for(int j = 0;j < segments.size();j++){
        const TriangleSegment2 & line = segments[j];
        Vector2 p(line.a);
        loop.push_back(p);
        if(j == segments.size() - 1){
            Vector2 p(line.b);
            loop.push_back(p);
        }
    }

}

//
// Converts vectors of segments into polygons.
// The ordering is reversed... the last vector of segments is the first polygon
//
// This function accumulates a table of segments into a table of polygons
void mgl::createPolysFromloopSegments(const SegmentTable &segmentTable,
										Polygons& loops)
{
	// outline loops
	unsigned int count = segmentTable.size();
	for(int i=0; i < count; i++)
	{
		const std::vector<TriangleSegment2> &segments = segmentTable[count-1 - i];
		loops.push_back(Polygon());
		Polygon &loop = loops[loops.size()-1];
	    segments2polygon(segments, loop);
	}
}
