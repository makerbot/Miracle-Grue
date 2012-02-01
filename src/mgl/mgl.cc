/*
 * mgl.cc
 *
 *  Created on: Dec 14, 2011
 *      Author: hugo
 */

#include "meshy.h"
#include "shrinky.h"
#include "scadtubefile.h"

#include <stdint.h>
#include <cstring>

using namespace mgl;
using namespace std;


bool mgl::sameSame(Scalar a, Scalar b)
{
	return (a-b) * (a-b) < 0.00000001;
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

bool mgl::Triangle3::cut(Scalar z, Vector3 &a, Vector3 &b) const
{

	Vector3 dir = cutDirection();
	// LineSegment2 cut;

	bool success = sliceTriangle(v0,v1,v2, z, a, b );

	Vector3 LineSegment2Dir = b - a;
	if(dir.dotProduct(LineSegment2Dir) < 0 )
	{
//		cout << "INVERTED LineSegment2 DETECTED" << endl;
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
						vector<LineSegment2>::iterator startIt,
						vector<LineSegment2>::iterator endIt,
						vector<LineSegment2>::iterator &bestLineSegment2It ) // "returned here"
{
	bestLineSegment2It = endIt; 	// just in case, we'll check for this on the caller side
	Scalar minDist = 1e100; 
	
	Vector3 end(endOfPreviousLineSegment2.x,endOfPreviousLineSegment2.y, 0);
	vector<LineSegment2>::iterator it = startIt;
	while(it != endIt)
	{
		Vector3 start(it->a.x, it->a.y, 0);
		Vector3 v = end-start;
		Scalar distance = v.squaredMagnitude();
		if (distance < minDist)
		{
			minDist = distance;
			bestLineSegment2It = it;
			// we could decide to stop here ... if we had a threshold
		}
		it ++;
	}
	return minDist;
}

void mgl::loopsAndHoles(std::vector<LineSegment2> &LineSegment2s,
		Scalar tol,
		std::vector< std::vector<LineSegment2> > &loops)
{
	// Lets sort this mess out so we can extrude in a continuous line of shiny contour
	// Nota: from their normals (int their previous life as 3d triangles), LineSegment2 know their beginings from their endings
	
	std::vector<Scalar> distances;
	distances.reserve(LineSegment2s.size());
	
	distances.push_back(0); // this value is not used, it represents the distance between the
							// first LineSegment2 and the one before (and there is no LineSegment2 before)
	for(vector<LineSegment2>::iterator i = LineSegment2s.begin(); i != LineSegment2s.end(); i++)
	{
		Vector2 &startingPoint = i->b;
		vector<LineSegment2>::iterator startIt = i+1;
		vector<LineSegment2>::iterator bestLineSegment2It;
		if(startIt != LineSegment2s.end())
		{
			Scalar distance = findClosestLineSegment2(startingPoint, startIt, LineSegment2s.end(), bestLineSegment2It);
			if(bestLineSegment2It != LineSegment2s.end())
			{
				// Swap the LineSegment2s, because the bestLineSegment2 is the closest LineSegment2 to the current one
				LineSegment2 tmp;
				tmp.a = startIt->a;
				tmp.b = startIt->b;

				startIt->a = bestLineSegment2It->a;
				startIt->b = bestLineSegment2It->b;

				bestLineSegment2It->a = tmp.a;
				bestLineSegment2It->b = tmp.b;
				distances.push_back(distance);
			}
		}
	}
	
	// we now have an optimal sequence of LineSegment2s (except we didn't optimise for interloop traversal).
	// we also have a hop (distances) between each LineSegment2 pair
		

	vector<Scalar>::iterator hopIt = distances.begin();
	vector<LineSegment2>::iterator i = LineSegment2s.begin();
	while(i != LineSegment2s.end())
	{
		// lets make a loop... we'll call it loop
		loops.push_back(vector<LineSegment2>());
		vector<LineSegment2> &loop = loops[loops.size()-1];
		
		loop.push_back(*i);

		i++;
		hopIt++;
		if(i == LineSegment2s.end())
		{
			// this is sad ... a loop with a single LineSegment2
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
				// cout << " "<< loop.size()<< " loopLineSegment2 " << i->a << ", " << i->b  << endl;
				loop.push_back(*i);
				i++;
				hopIt ++;
			}
			else
			{
				thisLoopIsDone = true;
			}
			if(i == LineSegment2s.end())
			{
				thisLoopIsDone = true;
			}
		}
	}
}

void mgl::segmentology(	const std::vector<Triangle3> &allTriangles,
						const TriangleIndices &trianglesForSlice,
						Scalar z,
						Scalar tol,
						std::vector< std::vector<LineSegment2> > &loops)
{
	std::vector<LineSegment2> LineSegment2s;
	assert(LineSegment2s.size() == 0);

	size_t count = trianglesForSlice.size();
	LineSegment2s.reserve(count);

	//#pragma omp parallel for
	for(int i=0; i< count; i++)
	{
		index_t triangleIndex = trianglesForSlice[i];
		const Triangle3 &triangle = allTriangles[triangleIndex];
		Vector3 a,b;
		// bool cut = sliceTriangle(triangle[0], triangle[1], triangle[2], z, a, b);
		bool cut = triangle.cut(z,a,b);
		if(cut)
		{
			LineSegment2 s;
			s.a.x = a.x;
			s.a.y = a.y;
			s.b.x = b.x;
			s.b.y = b.y;
			LineSegment2s.push_back(s);
		}
	}
	
	// what we are left with is a series of tubes (outline tubes... triangle has beens)

	// lets break this vector into loops.
	loopsAndHoles(LineSegment2s, tol, loops);

}

void mgl::translateLoops(std::vector<std::vector<mgl::LineSegment2> > &loops, Vector2 p)
{
	for(int i=0; i < loops.size(); i++)
	{
		translateSegments(loops[i],p);
	}
}

void mgl::translateSegments(std::vector<mgl::LineSegment2> &segments, Vector2 p)
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

void mgl::rotateLoops(std::vector<std::vector<mgl::LineSegment2> > &loops, Scalar angle)
{
	for(int i=0; i < loops.size(); i++)
	{
		rotateSegments(loops[i], angle);
	}
}

void mgl::rotateSegments(std::vector<mgl::LineSegment2> &LineSegment2s, Scalar angle)
{
	for(int i=0; i < LineSegment2s.size(); i++)
	{
		LineSegment2s[i].a = rotate2d(LineSegment2s[i].a, angle);
		LineSegment2s[i].b = rotate2d(LineSegment2s[i].b, angle);
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

bool mgl::segmentSegmentIntersection(const LineSegment2 &s0, const LineSegment2 &s1, Vector2 &p)
{

	bool s;
	s = segmentSegmentIntersection(	s0.a[0], s0.a[1],
									s0.b[0], s0.b[1],
									s1.a[0], s1.a[1],
									s1.b[0], s1.b[1],
									p.x, p.y);
	return s;
}


void dumpSegments(const char* prefix, const std::vector<LineSegment2> &segments)
{
	cout << prefix << "segments = [ // " << segments.size() << " segments" << endl;
    for(int id = 0; id < segments.size(); id++)
    {
    	LineSegment2 seg = segments[id];
    	cout << prefix << " [[" << seg.a << ", " << seg.b << "]], // " << id << endl;
    }
    cout << prefix << "]" << endl;
    cout << "// color([1,0,0.4,1])loop_segments(segments,0.050000);" << endl;
}

void dumpInsets(const std::vector<InsetTable> &insetsForLoops)
{
	for (unsigned int i=0; i < insetsForLoops.size(); i++)
	{
		const InsetTable &insetTable =  insetsForLoops[i];
		cout << "Loop " << i << ") " << insetTable.size() << " insets"<<endl;

		for (unsigned int i=0; i <insetTable.size(); i++)
		{
			const std::vector<LineSegment2 >  &loop = insetTable[i];
			cout << "   inset " << i << ") " << loop.size() << " segments" << endl;
			dumpSegments("        ",loop);
		}
	}
}

void mgl::sliceAndPath(	Meshy &mesh,
						double layerW,
						double tubeSpacing,
						double angle,
						const char* scadFile,
						// std::vector< TubesInSlice >  &allTubes
						std::vector< SliceData >  &slices)
{
	unsigned int extruderId = 0;
	unsigned int shells = 3;
	assert(slices.size() == 0);

	Scalar tol = 1e-6; // Tolerance for assembling LineSegment2s into a loop

	// gather context info
	const std::vector<Triangle3> &allTriangles = mesh.readAllTriangles();
	const SliceTable &sliceTable = mesh.readSliceTable();
	const Limits& limits = mesh.readLimits();
	// cout << "Limits: " << limits << endl;
	Limits tubularLimits = limits.centeredLimits();
	tubularLimits.inflate(1.0, 1.0, 0.0);
	// make it square along z so that rotation happens inside the limits
	// hence, tubular limits around z
	tubularLimits.tubularZ();

	Vector3 c = limits.center();
	Vector2 toRotationCenter(-c.x, -c.y);
	Vector2 backToOrigin(c.x, c.y);

	size_t sliceCount = sliceTable.size();

	Vector3 rotationCenter = limits.center();
	Scalar layerH = mesh.readLayerMeasure().getLayerH();

	// we'll record that in a scad file for you
	ScadTubeFile outlineScad;
	if(scadFile != NULL)
	{
		outlineScad.open(scadFile);
		outlineScad.writePathViz(layerH, layerW, sliceCount);
	}
	slices.reserve(sliceCount);

	// multi thread stuff
#ifdef OMPFF
	omp_lock_t my_lock;
	omp_init_lock (&my_lock);
	#pragma omp parallel for
#endif


	ProgressBar progress(sliceCount);
	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
	{
		const TriangleIndices &trianglesForSlice = sliceTable[sliceId];

		progress.tick();
		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);

		slices.push_back( SliceData(z,sliceId));
		SliceData &slice = slices[sliceId];
		slice.extruderSlices.push_back(ExtruderSlice());

		// get the "real" 2D paths for outline
		std::vector< std::vector<LineSegment2> > outlinesSegments;
		segmentology(allTriangles, trianglesForSlice, slice.z, tol, outlinesSegments);

/*
		// keep all segments of insets for each loop
		std::vector<InsetTable> insetsForLoops;
		for(unsigned int outlineId=0; outlineId < outlinesSegments.size(); outlineId++)
		{
			// prepare  a new vector of loops for insets of this outline
			insetsForLoops.push_back(InsetTable());
			InsetTable &insetTable = *insetsForLoops.rbegin(); // inset curves for a single loop

			std::vector<LineSegment2> &loop =  outlinesSegments[outlineId];
			insetTable.push_back(std::vector<LineSegment2 >());
			std::vector<LineSegment2> &insets = *insetTable.rbegin();
			MyComputer myComputer;
			stringstream ss;
			ss << "_slice_" << sliceId << "_loop_" << outlineId << ".scad";
			string loopScadFile = myComputer.fileSystem.ChangeExtension(scadFile, ss.str());
			Shrinky shrinky(loopScadFile.c_str());
			for (unsigned int shellId=0; shellId < shells; shellId++)
			{
				Scalar insetDistance = shellId==0? 0.5*layerW : layerW * 0.8;
				shrinky.inset(loop, insetDistance, insets);
				// next, we'll inset from the new polygon
				loop = insets;
			}
		}

		dumpInsets(insetsForLoops);
*/

/*
		// create a vector of polygons for each shell.
		for (unsigned int shellId=0; shellId < shells; shellId++)
		{
			cout << "inset size " << slice.extruderSlices[extruderId].insets.size() << endl;
			slice.extruderSlices[extruderId].insets.push_back(Polygons());
			Polygons &polygons = *slice.extruderSlices[extruderId].insets.rbegin();
			createPolysFromloopSegments(insetsForLoops[shellId] , polygons );
		}
*/
		Scalar layerAngle = sliceId * angle;
		// deep copy the smallest insets as he infill boundaries
		std::vector<std::vector<LineSegment2> > rotatedSegments = outlinesSegments; // insetsForLoops[0]; //
		mgl::translateLoops(rotatedSegments, toRotationCenter);
		// rotate the outlines before generating the tubes...
		mgl::rotateLoops(rotatedSegments, layerAngle);

		std::vector<LineSegment2> infillSegments;
		infillPathology(rotatedSegments,
						tubularLimits,
						slice.z,
						tubeSpacing,
						infillSegments);

		createPolysFromloopSegments(outlinesSegments, slice.extruderSlices[extruderId].loops);

		// rotate and translate the TUBES so they fit with the ORIGINAL outlines
		mgl::rotateSegments(infillSegments, -layerAngle);
		mgl::translateSegments(infillSegments, backToOrigin);
		createPolysFromInfillSegments(infillSegments, slice.extruderSlices[extruderId].infills);

		// write the scad file
		// only one thread at a time in here
		if(scadFile != NULL)
		{
			#ifdef OMPFF
			OmpGuard lock (my_lock);
			cout << "slice "<< i << "/" << sliceCount << " thread: " << "thread id " << omp_get_thread_num() << " (pool size: " << omp_get_num_threads() << ")"<< endl;
			#endif

			outlineScad.writeTrianglesModule("tri_", mesh, sliceId);
			//outlineScad.writeOutlines(slice.extruderSlices[0].loops,  slices[i].z , i);
			outlineScad.writePolygons("outlines_", "outline", slice.extruderSlices[extruderId].loops, slices[sliceId].z, sliceId);
			outlineScad.writePolygons("infill_",   "infill" , slice.extruderSlices[extruderId].infills, slices[sliceId].z, sliceId);

		}
	}
	// finalize the scad file
	if(scadFile != NULL)
	{
		outlineScad.writeSwitcher(sliceTable.size());
		outlineScad.close();
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

#define __ cout <<  __FUNCTION__ << "::" << __LINE__  "*" << endl;
#define ___(s) cout <<  __FUNCTION__ << "::" << __LINE__  << " > "<< s << endl;


void mgl::infillPathology( std::vector< std::vector<LineSegment2> > &outlineLoops,
				const Limits& limits,
				double z,
				double tubeSpacing,
				std::vector<LineSegment2> &tubes)
{
	assert(tubeSpacing != 0);
	assert(tubes.size() == 0);
	Scalar deltaY = limits.yMax - limits.yMin;

	int tubeCount = (deltaY) / tubeSpacing;
	std::vector< std::set<Scalar> > intersects;
	// allocate
	intersects.resize(tubeCount);
	for (unsigned int i=0; i < tubeCount; i++)
	{
		Scalar y = -0.5 * deltaY + i * tubeSpacing;
		std::set<Scalar> &lineCuts = intersects[i];
		// go through all the LineSegment2s in every loop
		for(unsigned int j=0; j< outlineLoops.size(); j++)
		{
			// cout << " Loop " << j << endl;
			std::vector<LineSegment2> &outlineLineSegment2s = outlineLoops[j];
			for(std::vector<LineSegment2>::iterator it= outlineLineSegment2s.begin(); it!= outlineLineSegment2s.end(); it++)
			{
				//cout << " LineSegment2" << endl;
				LineSegment2 &segment = *it;
				Scalar intersectionX, intersectionY;
				if (segmentSegmentIntersection(limits.xMin, y,
												limits.xMax, y,
												segment.a.x, segment.a.y,
												segment.b.x, segment.b.y,
												intersectionX,  intersectionY))
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
		//std::vector<LineSegment2>& lineTubes = tubes[i];
		Scalar y = bottom + i * tubeSpacing;
		std::set<Scalar> &lineCuts = intersects[i];

		LineSegment2 LineSegment2;
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
					LineSegment2.a.x = x;
					LineSegment2.a.y = y;
				}
				else
				{
					LineSegment2.b.x = x;
					LineSegment2.b.y = y;
					//lineTubes.push_back(LineSegment2);
					tubes.push_back(LineSegment2);
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
					LineSegment2.a.x = x;
					LineSegment2.a.y = y;
				}
				else
				{
					LineSegment2.b.x = x;
					LineSegment2.b.y = y;
					tubes.push_back(LineSegment2);
				}
			}
		}
		backAndForth = !backAndForth;
	}

}



void segments2polygon(Polygon & loop, const std::vector<LineSegment2> & segments)
{
    loop.reserve(segments.size());
    for(int j = 0;j < segments.size();j++){
        const LineSegment2 & line = segments[j];
        Vector2 p(line.a.x, line.a.y);
        loop.push_back(p);
        if(j == segments.size() - 1){
            Vector2 p(line.b.x, line.b.y);
            loop.push_back(p);
        }
    }

}

//
// Converts vectors of segments into polygons.
// The ordering is reversed... the last vector of segments is the first polygon
//
void mgl::createPolysFromloopSegments(const std::vector< std::vector<LineSegment2> >  &outlinesSegments,
										Polygons& loops)
{
	// outline loops
	unsigned int count = outlinesSegments.size();
	for(int i=0; i < count; i++)
	{
		const std::vector<LineSegment2> &LineSegment2s = outlinesSegments[count-1 - i];
		loops.push_back(Polygon());
		Polygon &loop = loops[loops.size()-1];
	    segments2polygon(loop, LineSegment2s);
	}
}

void mgl::createPolysFromInfillSegments(const std::vector<LineSegment2> &infillLineSegment2s,
									Polygons& infills)
{
	size_t tubeCount = infillLineSegment2s.size();
	for (int i=0; i< tubeCount; i++)
	{
		const LineSegment2 &LineSegment2 = infillLineSegment2s[i];

		infills.push_back(Polygon());
		Polygon &poly = infills[infills.size()-1];

		Vector2 p0 (LineSegment2.a.x, LineSegment2.a.y);
		Vector2 p1 (LineSegment2.b.x, LineSegment2.b.y);

		poly.push_back(p0);
		poly.push_back(p1);
	}
}





