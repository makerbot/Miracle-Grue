/*
 * mgl.cc
 *
 *  Created on: Dec 14, 2011
 *      Author: hugo
 */

#include "meshy.h"
#include "scadtubefile.h"

#include <stdint.h>
#include <cstring>

using namespace mgl;
using namespace BGL;


bool mgl::sameSame(double a, double b)
{
	return (a-b) * (a-b) < 0.00000001;
}

void mgl::minMaxZ(const BGL::Triangle3d &t, Scalar &min,  Scalar &max )
{
	// find minimum
	min = t.vertex1.z;
	if(t.vertex2.z < min)
		min = t.vertex2.z ;

	if(t.vertex3.z < min)
		min = t.vertex3.z ;

	// find maximum
	max = t.vertex1.z;
	if( t.vertex2.z > max)
		max = t.vertex2.z;

	if (t.vertex3.z > max)
		max = t.vertex3.z;
}


bool sliceTriangle(const Vector3d& vertex1,
					 const Vector3d& vertex2,
						const Vector3d& vertex3,
						   Scalar Z,
						   Vector3d &a,
						   	   Vector3d &b)
{
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
	if (sameSame(vertex1.z, Z) )
	{
		if (sameSame(vertex2.z,Z) )
		{
			if (sameSame(vertex3.z,Z) )
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
		if (sameSame(vertex3.z,Z) )
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
	else if (sameSame(vertex2.z, Z) )
	{
		if (sameSame(vertex3.z,Z) )
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
	else if (sameSame(vertex3.z, Z) )
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

bool mgl::Triangle3::cut(Scalar z, Vector3d &a, Vector3d &b) const
{

	Vector3d dir = cutDirection();
	// Segment cut;

	bool success = sliceTriangle(v0,v1,v2, z, a, b );

	Vector3d segmentDir = b - a;
	if(dir.dotProduct(segmentDir) < 0 )
	{
//		cout << "INVERTED SEGMENT DETECTED" << endl;
		Vector3d p(a);
		a = b;
		b = p;
	}
	return success;
}

std::ostream& mgl::operator<<(ostream& os, const mgl::Vector3d& v)
{
	os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
	return os;
}

// given a point, finds the segment that starts the closest from that point
// and return the distance. Also, the iterator to the closest segment is "returned"
Scalar findClosestSegment(const BGL::Point& endOfPreviousSegment, 
						vector<Segment>::iterator startIt,
						vector<Segment>::iterator endIt, 
						vector<Segment>::iterator &bestSegmentIt ) // "returned here"
{
	bestSegmentIt = endIt; 	// just in case, we'll check for this on the caller side
	Scalar minDist = 1e100; 
	
	Vector3d end(endOfPreviousSegment.x,endOfPreviousSegment.y, 0);
	vector<Segment>::iterator it = startIt;
	while(it != endIt)
	{
		Vector3d start(it->a.x, it->a.y, 0);
		Vector3d v = end-start;
		Scalar distance = v.squareMagnitude();
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

void mgl::loopsAndHoles(std::vector<Segment> &segments, Scalar tol, std::vector< std::vector<Segment> > &loops)
{
	// Lets sort this mess out so we can extrude in a continuous line of shiny contour
	// Nota: from the normals, segment know their beginings from their endings  
	

	std::vector<Scalar> distances;
	distances.reserve(segments.size());
	
	distances.push_back(0); // this value is not used, it represents the distance between the
							// first segment and the one before (and there is no segment before)
	for(vector<Segment>::iterator i = segments.begin(); i != segments.end(); i++)
	{
		BGL::Point &startingPoint = i->b;
		vector<Segment>::iterator startIt = i+1;
		vector<Segment>::iterator bestSegmentIt;
		if(startIt != segments.end())
		{
			Scalar distance = findClosestSegment(startingPoint, startIt, segments.end(), bestSegmentIt);
			if(bestSegmentIt != segments.end())
			{
				// Swap the segments, because the bestSegment is the closest segment to the current one
				Segment tmp;
				tmp.a = startIt->a;
				tmp.b = startIt->b;

				startIt->a = bestSegmentIt->a;
				startIt->b = bestSegmentIt->b;

				bestSegmentIt->a = tmp.a;
				bestSegmentIt->b = tmp.b;
				distances.push_back(distance);
			}
		}
	}
	
	// we now have an optimal sequence of segments (except we didn't optimise for interloop traversal).
	// we also have a hop (distances) between each segment pair
		

	vector<Scalar>::iterator hopIt = distances.begin();
	vector<Segment>::iterator i = segments.begin(); 
	while(i != segments.end())
	{
		// lets make a loop... we'll call it loop
		loops.push_back(vector<Segment>());
		vector<Segment> &loop = loops[loops.size()-1];
		
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
				// cout << " "<< loop.size()<< " loopSegment " << i->a << ", " << i->b  << endl;
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

void mgl::segmentology(	const std::vector<BGL::Triangle3d> &allTriangles,
						const TriangleIndices &trianglesForSlice,
						Scalar z,
						Scalar tol,
						std::vector< std::vector<Segment> > &loops)
{
	std::vector<Segment> segments;
	assert(segments.size() == 0);

	size_t count = trianglesForSlice.size();
	segments.reserve(count);

	//#pragma omp parallel for
	for(int i=0; i< count; i++)
	{
		index_t triangleIndex = trianglesForSlice[i];
		const BGL::Triangle3d &t = allTriangles[triangleIndex];

		Triangle3 triangle(t);
		Vector3d a,b;
		// bool cut = sliceTriangle(triangle[0], triangle[1], triangle[2], z, a, b);
		bool cut = triangle.cut(z,a,b);
		if(cut)
		{
			Segment s;
			s.a.x = a.x;
			s.a.y = a.y;
			s.b.x = b.x;
			s.b.y = b.y;
			segments.push_back(s);
		}
	}
	
	// what we are left with is a series of tubes (outline tubes... triangle has beens)

	// lets break this vector into loops.
	loopsAndHoles(segments, tol, loops);

}

void mgl::translateLoops(std::vector<std::vector<mgl::Segment> > &loops, BGL::Point p)
{
	for(int i=0; i < loops.size(); i++)
	{
		translateSegments(loops[i],p);
	}
}

void mgl::translateSegments(std::vector<mgl::Segment> &segments, BGL::Point p)
{
	for(int i=0; i < segments.size(); i++)
	{
		segments[i].a += p;
		segments[i].b += p;
	}
}


BGL::Point mgl::rotate2d(const BGL::Point &p, Scalar angle)
{
	// rotate point
	double s = sin(angle); // radians
	double c = cos(angle);
	BGL::Point rotated;
	rotated.x = p.x * c - p.y * s;
	rotated.y = p.x * s + p.y * c;
	return rotated;
}

void mgl::rotateLoops(std::vector<std::vector<mgl::Segment> > &loops, Scalar angle)
{
	for(int i=0; i < loops.size(); i++)
	{
		rotateSegments(loops[i], angle);
	}
}

void mgl::rotateSegments(std::vector<mgl::Segment> &segments, Scalar angle)
{
	for(int i=0; i < segments.size(); i++)
	{
		segments[i].a = rotate2d(segments[i].a, angle);
		segments[i].b = rotate2d(segments[i].b, angle);
	}
}

bool mgl::segmentSegmentIntersection(Scalar p0_x, Scalar p0_y, Scalar p1_x, Scalar p1_y,
		Scalar p2_x, Scalar p2_y, Scalar p3_x, Scalar p3_y, Scalar &i_x, Scalar &i_y)
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

void mgl::sliceAndPath(	Meshy &mesh,
					double layerW,
					double tubeSpacing,
					double angle,
					const char* scadFile,
					std::vector< TubesInSlice >  &allTubes)
{
	Scalar tol = 1e-6; // Tolerance for assembling segments into a loop

	// gather context info
	const std::vector<BGL::Triangle3d> &allTriangles = mesh.readAllTriangles();
	const SliceTable &sliceTable = mesh.readSliceTable();
	const Limits& limits = mesh.readLimits();
	// std::cout << "LIMITS: " << limits << std::endl;

	Limits tubularLimits = limits.centeredLimits();
	tubularLimits.inflate(1.0, 1.0, 0.0);
	// make it square along z so that rotation happens inside the limits
	// hence, tubular limits around z
	tubularLimits.tubularZ();

	BGL::Point3d c = limits.center();
	BGL::Point toRotationCenter(-c.x, -c.y);
	BGL::Point backToOrigin(c.x, c.y);

	size_t sliceCount = sliceTable.size();

	BGL::Point3d rotationCenter = limits.center();
	Scalar layerH = mesh.readLayerMeasure().getLayerH();
	// we'll record that in a scad file for you
	ScadTubeFile outlineScad(scadFile, layerH, layerW );
	allTubes.reserve(sliceCount);

	// multi thread stuff
#ifdef OMPFF
	omp_lock_t my_lock;
	omp_init_lock (&my_lock);
	#pragma omp parallel for
#endif

	for(int i=0; i < sliceCount; i++)
	{
		//cout << "SLICE>> " << i << endl;
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(i);
		allTubes.push_back( TubesInSlice(z));
		std::vector<Segment> &infillTubes = allTubes[i].infill;

		const TriangleIndices &trianglesForSlice = sliceTable[i];

		std::vector<std::vector<Segment> > &outlineLoops = allTubes[i].outlines;
		// get the "real" 2D paths for outline
		segmentology(allTriangles, trianglesForSlice, allTubes[i].z, tol, outlineLoops);
		//cout << "Segmentology: " << outlineLoops.size() << " Loops" << endl;

		Scalar layerAngle = i* angle;
		// deep copy
		std::vector<std::vector<Segment> > rotatedLoops = outlineLoops;
		mgl::translateLoops(rotatedLoops, toRotationCenter);
		// rotate the outlines before generating the tubes...
		mgl::rotateLoops(rotatedLoops, layerAngle);

		pathology(rotatedLoops, tubularLimits, allTubes[i].z, tubeSpacing, infillTubes);
		//cout << "Pathology: " << infillTubes.size() << " infill segments" << endl;

		// rotate the TUBES so they fit with the ORIGINAL outlines
		mgl::rotateSegments(infillTubes, -layerAngle);
		mgl::translateSegments(infillTubes, backToOrigin);

		// write the scad file
		// only one thread at a time in here
		{
			#ifdef OMPFF
			OmpGuard lock (my_lock);
			cout << "slice "<< i << "/" << sliceCount << " thread: " << "thread id " << omp_get_thread_num() << " (pool size: " << omp_get_num_threads() << ")"<< endl;
			#endif
			cout << "." ;
			outlineScad.writeTrianglesModule("tri_", mesh, i);

			outlineScad.writeOutlinesModule("out_", outlineLoops, i, allTubes[i].z);
			outlineScad.writeExtrusionsModule("fill_", infillTubes, i, allTubes[i].z );

		}

	}
	// finalize the scad file
	outlineScad.writeSwitcher(sliceTable.size());
}

std::ostream& mgl::operator<<(ostream& os, const Limits& l)
{
	os << "[" << l.xMin << ", " << l.yMin << ", " << l.zMin << "] [" << l.xMax << ", " << l.yMax << ", "<< l.zMax  << "]";
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
		msg += "\"";
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
			Point3d pt1(v.x1, v.y1, v.z1);
			Point3d pt2(v.x2, v.y2, v.z2);
			Point3d pt3(v.x3, v.y3, v.z3);

			Triangle3d triangle(pt1, pt2, pt3);
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
			Point3d pt1(v.x1, v.y1, v.z1);
			Point3d pt2(v.x2, v.y2, v.z2);
			Point3d pt3(v.x3, v.y3, v.z3);

			Triangle3d triangle(pt1, pt2, pt3);
			meshy.addTriangle(triangle);

			facecount++;
		}
		fclose(f);
	}
	return facecount;
}

void mgl::pathology( std::vector< std::vector<Segment> > &outlineLoops,
				const Limits& limits,
				double z,
				double tubeSpacing,
				std::vector<Segment> &tubes)
{
	assert(tubes.size() == 0);

//  rotate outline Segments for that cool look
//	translateSegments(outlineSegments, toOrigin);
//	rotateSegments(segments, angle);
//  translateSegments(segments, toCenter);

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
			// cout << "  Loop " << j << endl;
			std::vector<Segment> &outlineSegments = outlineLoops[j];
			for(std::vector<Segment>::iterator it= outlineSegments.begin(); it!= outlineSegments.end(); it++)
			{
				//cout << "      segment" << endl;
				Segment &segment = *it;
				Scalar intersectionX, intersectionY;
				if (segmentSegmentIntersection(limits.xMin, y, limits.xMax, y, segment.a.x, segment.a.y, segment.b.x, segment.b.y,  intersectionX,  intersectionY))
				{
					lineCuts.insert(intersectionX);
					//cout << "         X" << endl;
				}
			}
		}
	}

	//tubes.resize(tubeCount);

	bool backAndForth = true;

	Scalar bottom = -0.5 * deltaY;
	for (int i=0; i < tubeCount; i++)
	{
		//std::vector<Segment>& lineTubes = tubes[i];
		Scalar y = bottom + i * tubeSpacing;
		std::set<Scalar> &lineCuts = intersects[i];

		Segment segment;
		bool inside = false;
		if( backAndForth)
		{
			for(std::set<Scalar>::iterator it = lineCuts.begin(); it != lineCuts.end(); it++)
			{
				inside =! inside;
				Scalar x = *it;
				// cout << "\t" << x << " " << inside <<",";
				if(inside)
				{
					segment.a.x = x;
					segment.a.y = y;
				}
				else
				{
					segment.b.x = x;
					segment.b.y = y;
					//lineTubes.push_back(segment);
					tubes.push_back(segment);
				}
			}
		}
		else
		{
			for(std::set<Scalar>::reverse_iterator it = lineCuts.rbegin(); it != lineCuts.rend(); it++)
			{
				inside =! inside;
				Scalar x = *it;
				// cout << "\t" << x << " " << inside <<",";
				if(inside)
				{
					segment.a.x = x;
					segment.a.y = y;
				}
				else
				{
					segment.b.x = x;
					segment.b.y = y;
					tubes.push_back(segment);
				}
			}
		}
		backAndForth = !backAndForth;
	}


	// unrotate the tube segments (they are tube rays, not cut triangles)
//	for (int i=0; i < tubeCount; i++)
//	{
//		std::vector<Segment>& tubes = allTubes[i];
//		rotateSegments(segments, -angle);
//		translateSegments(segments, toCenter);

//	}
}


