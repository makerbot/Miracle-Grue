
#include <time.h>
#include <fstream>
#include <algorithm> // find
#include <iterator>  // distance
#include <iomanip>
#include <limits>
#include <set>

#define  CPPUNIT_ENABLE_NAKED_ASSERT 1

#include <cppunit/config/SourcePrefix.h>
#include "ModelReaderTestCase.h"

#include "../Configuration.h"
#include "../ModelFileReaderOperation.h"

#include "../BGL/BGLPoint.h"
#include "../BGL/BGLMesh3d.h"

#include "../BGL/BGLMesh3d.h"

#include "mgl/limits.h"
#include "mgl/meshy.h"
#include "mgl/segment.h"
#include "mgl/scadtubefile.h"



CPPUNIT_TEST_SUITE_REGISTRATION( ModelReaderTestCase );

using namespace std;
using namespace BGL;
using namespace mgl;

bool sameSame(double a, double b)
{
	return (a-b) * (a-b) < 0.00000001;
}

/*
CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
CPPUNIT_ASSERT_EQUAL( 12, 12 );
CPPUNIT_ASSERT( 12L == 12L );
*/





class Face
{
public:
	index_t edgeIndices[3];
};

class Edge
{
	index_t vertexIndices[2];	// the index of the vertices that make out the edge
	index_t face0;
	int face1;


public:
	friend std::ostream& operator <<(ostream &os,const Edge &pt);


	Edge(index_t v0, index_t v1, index_t face)
		//:vertexIndices({v0,v1},
		:face0(face), face1(-1)
	{
			vertexIndices[0] = v0;
			vertexIndices[1] = v1;

	}

	void lookUpIncidentFaces(int& f1, int &f2) const
	{
		f1 = face0;
		f2 = face1;
	}

	int lookUpNeighbor(index_t face) const
	{
		if(face0 == face)
			return face1;
		if (face1 == face)
			return face0;
		assert(0);
		return -1;
	}

	bool operator==(const Edge &other) const
	{
		assert(&other != this);
	    // Compare the values, and return a bool result.
		if(other.vertexIndices[0] == this->vertexIndices[0] && other.vertexIndices[1] == this->vertexIndices[1])
			return true;
		if(other.vertexIndices[1] == this->vertexIndices[0] && other.vertexIndices[0] == this->vertexIndices[1])
			return true;
		return false;
	}

	bool operator!=(const Edge &other) const
	{
	    return !(*this == other);
	}

	void connectFace(index_t face)
	{
		if(face0 == face)
		{
			cout << "BAD: edge connecting to the same face twice! " << endl;
			cout << "  dump: " << this << endl;
			cout << ((Edge&)*this) << endl;
			assert(0);

		}

		if(face1 == -1)
		{
			face1 = face;
		}
		else
		{
			cout << "BAD: edge connected to face "<< face0  << " and face1 "<< face1<<  " trying to connecting to face " << face << endl;
			cout << "  dump: " << this << endl;
			cout << ((Edge&)*this) << endl;
			assert(0);
		}
	}

};

std::ostream& operator<<(ostream& os, const Edge& e)
{
	os << " " << e.vertexIndices[0] << "\t" << e.vertexIndices[1] << "\t" << e.face0 << "\t" << e.face1;
	return os;
}


///
/// This class consumes triangles (3 coordinates) and creates a list
/// of vertices, edges, and faces.
///
class Slicy
{
	std::vector<Point3d> vertices;
	std::vector<Edge> edges;
	std::vector<Face> faces;
	Scalar tolerence;

	friend std::ostream& operator <<(ostream &os,const Slicy &pt);

public:


	Slicy (Scalar tolerence)
		:tolerence(tolerence)
	{

	}

	index_t addTriangle(const Triangle3d &t)
	{
		index_t faceId = faces.size();

//		cout << "Slicy::addTriangle " << endl;
//		cout << "  v0 " << t.vertex1 << " v1" << t.vertex2 << " v3 " << t.vertex3 << endl;
//		cout << "  id:" << faceId << ": edge (v1,v2, f1,f2)" << endl;
		Face face;
		face.edgeIndices[0] = findOrCreateEdge(t.vertex1, t.vertex2, faceId);
//		cout << "   a) " << face.edge0 << "(" << edges[face.edge0] << ")" << endl;
		face.edgeIndices[1] = findOrCreateEdge(t.vertex2, t.vertex3, faceId);
//		cout << "   b) " << face.edge1 << "(" << edges[face.edge1] << ")" << endl;
		face.edgeIndices[2] = findOrCreateEdge(t.vertex3, t.vertex1, faceId);
//		cout << "   c) " << face.edge2 << "(" << edges[face.edge2] << ")" << endl;

		/*
		cout << "EDGE 0: index " << face.edge0 << " : " << edges[face.edge0] << endl;
		face.edge1 = findOrCreateEdge(t.vertex2, t.vertex3, faceId);
		cout << "EDGE 1: index " << face.edge0 << " edge (v1,v2, f1,f2): " << edges[face.edge0] << endl;
		cout << "EDGE 2: index " << face.edge0 << " edge (v1,v2, f1,f2): " << edges[face.edge0] << endl;
		 */
		faces.push_back(face);
		return faces.size() -1;
	}


	// given a face index, this method returns the cached
	void lookupIncidentFacesToFace(index_t faceId, int& face0, int& face1, int& face2) const
	{
		const Face& face = faces[faceId];

		const Edge &e0 = edges[face.edgeIndices[0] ];
		const Edge &e1 = edges[face.edgeIndices[1] ];
		const Edge &e2 = edges[face.edgeIndices[2] ];

		face0 = e0.lookUpNeighbor(faceId);
		face1 = e1.lookUpNeighbor(faceId);
		face2 = e2.lookUpNeighbor(faceId);

	}



	void dump(std::ostream& out) const
	{
		out << "Slicy" << endl;
		out << "  vertices: " << vertices.size() << endl;
		out << "  edges: " << edges.size() << endl;
		out << "  faces: " << faces.size() << endl;

		cout << endl;

		cout << "Vertices:" << endl;

		int x =0;
		for(vector<Point3d>::const_iterator i = vertices.begin(); i != vertices.end(); i++ )
		{
			cout << x << ": " << *i << endl;
			x ++;
		}

		cout << endl;
		cout << "Edges (vertex 1, vertex2, face 1, face2)" << endl;

		x =0;
		for(vector<Edge>::const_iterator i = edges.begin(); i != edges.end(); i++)
		{
			cout << x << ": " << *i << endl;
			x ++;
		}

	}
private:
	index_t findOrCreateEdge(const Point3d &coords0, const Point3d &coords1, size_t face)
	{
		index_t v0 = findOrCreateVertex(coords0);
		index_t v1 = findOrCreateVertex(coords1);

		Edge e(v0, v1, face);
		index_t edgeIndex;

		std::vector<Edge>::iterator it = find(edges.begin(), edges.end(), e);
		if(it == edges.end())
		{
			// cout << "NEW EDGE " << coords << endl;
			edges.push_back(e);
			edgeIndex = edges.size() -1;
		}
		else
		{
			it->connectFace(face);
			edgeIndex = std::distance(edges.begin(), it);
		}
		return edgeIndex;
	}


	index_t findOrCreateVertex(const Point3d &coords)
	{
		for(vector<Point3d>::iterator it = vertices.begin(); it != vertices.end(); it++)
		{
			const Point3d &p = (*it);
			Scalar dx = coords.x - p.x;
			Scalar dy = coords.y - p.y;
			Scalar dz = coords.z - p.z;

			Scalar dd =  dx * dx + dy * dy + dz * dz;
			if( dd < tolerence )
			{
				//cout << "Found VERTEX" << endl;
				index_t vertexIndex = std::distance(vertices.begin(), it);
				return vertexIndex;
			}
		}

		index_t vertexIndex;
		// cout << "NEW VERTEX " << coords << endl;
		vertices.push_back(coords);
		vertexIndex = vertices.size() -1;
		return vertexIndex;
	}
};

std::ostream& operator << (ostream &os, const Slicy &s)
{
	s.dump(os);
	return os;
}

/*
class Cuts
{
	index_t triangle;
	Point3d vertices[2]; // the second one is redundent
};

class Loopy
{
	std::list<index_t> triangleIndices;
	std::list<Cuts> segments;
};

class LoopPole
{
	std::list<index_t> EdgeIndices;
	// std::list<Point> points;
};

*/

// Generating the vertices for an arbitrarily oriented cylinder is a common problem that is fairly straightforward to solve.
// Generating the orthonormal basis vectors used to find the endcap vertices.
// Let W = normalize(P2-P1). As you've noted you need a unit-length vector 'U' that is perpendicular to W. It will also be convenient to have a unit-length vector V perpendicular to both W and U. U, V, and W form the orthonormal basis for the cylinder.
// To find a vector perpendicular to W, simply cross W with the world x, y, or z axis. The only caveat is that two vectors which are nearly aligned will produce a cross product with a very small magnitude, which you may not be able to normalize. To avoid this problem, simply cross W with the world axis corresponding to the component of W whose absolute value is least. Then, normalize the result to get U. Finally, V = Cross(W, U).
// You now have two coordinate systems, each with basis vectors U, V, W, and with origins P1 and P2 respectively. You can then use simple trig to find the vertices of the endcaps.


//
// Adds 2 triangles with a common edge
// to a Slicy
//
void ModelReaderTestCase::testSlicySimple()
{

	Point3d p0(0,0,0);
	Point3d p1(0,1,0);
	Point3d p2(1,1,0);
	Point3d p3(1,0,0);

	Triangle3d t0(p0, p1, p2);
	Triangle3d t1(p0, p2, p3);

	Slicy sy(0.001);

	cout << endl << endl;
	cout << "******** slicy with 2 triangles *******" << endl;
	cout << "add t0" << endl;
	size_t face0 = sy.addTriangle(t0);

	cout << "add t1" << endl;
	size_t face1 = sy.addTriangle(t1);

	cout << "xx"<< endl;



	int a,b,c;
	sy.lookupIncidentFacesToFace(face0, a,b,c);

	cout << "xx"<< endl;
	CPPUNIT_ASSERT(a==face1 || b==face1 || c== face1);

	cout << "xxx"<< endl;
	sy.dump(cout);
//	cout << sy << endl;
}

//
// This test loads 1 triangle and verifies the number of slices
//
void ModelReaderTestCase::testMeshySimple()
{
	Scalar zH = 1.0;
	Meshy mesh(zH);

	Triangle3d t;
	t.vertex1 =Point3d(0,10,0);
	t.vertex2 =Point3d(0,10,2.4);
	t.vertex3 =Point3d(0,10,1);

	cout << endl << endl;
	cout << "t " << t.vertex1 << ", " << t.vertex2 << ", " << t.vertex3 << endl;
 	mesh.addTriangle(t);

 	mesh.dump(cout);

 	CPPUNIT_ASSERT_EQUAL((size_t)2, mesh.readSliceTable().size());

	t.vertex1 =Point3d(0,10, 0);
	t.vertex2 =Point3d(0,10, 2.6);
	t.vertex3 =Point3d(0,10, 1);

	mesh.addTriangle(t);
	CPPUNIT_ASSERT_EQUAL((size_t)3, mesh.readSliceTable().size());

	const Limits &limits = mesh.readLimits();
	double tol = 0.00001;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0,  limits.xMin, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(10, limits.yMin, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0,  limits.zMin, tol);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0,   limits.xMax, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(10,  limits.yMax, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.6, limits.zMax, tol);
}

void ModelReaderTestCase::testLayerSplit()
{
	Meshy mesh(0.35);
	unsigned int t0, t1;
	t0 = clock();
	//LoadMeshyFromStl(mesh, "inputs/Water.stl");
	LoadMeshyFromStl(mesh, "inputs/Water.stl");
	t1=clock()-t0;
	mesh.dump(cout);

	cout << " **** testLayerSplit " << endl;
	for(int i=0; i != mesh.readSliceTable().size(); i++)
	{
		stringstream ss;
		ss << "test_cases/modelReaderTestCase/output/water_" << i << ".stl";
		mesh.writeStlFileForLayer(i, ss.str().c_str());
		cout << ss.str().c_str() << endl;
	}
}

void ModelReaderTestCase::testLargeMeshy()
{
	unsigned int t0,t1;
	cout << "Light saber" << endl;
	Meshy mesh3(0.35);
	t0=clock();
	LoadMeshyFromStl(mesh3, "inputs/lightsaber.stl");
	t1=clock()-t0;
	mesh3.dump(cout);
	cout << "lightsaber read in " << t1 << endl;

	t0=clock();
	// do something..
	Mesh3d meshLightSaber;
	cout << "RAW read" << endl;
	int count = meshLightSaber.loadFromSTLFile("inputs/lightsaber.stl");
	t1=clock()-t0;
	cout << "lightsaber read in " << t1 << endl;

}

void ModelReaderTestCase::testMeshyLoad()
{
	unsigned int t0,t1;
	cout << "Water" << endl;
	Meshy mesh(0.35);
	t0=clock();
	LoadMeshyFromStl(mesh, "inputs/Water.stl");
	t1=clock()-t0;
	mesh.dump(cout);
	cout << "time: " << t1 << endl;
	CPPUNIT_ASSERT_EQUAL((size_t)173, mesh.readSliceTable().size());

	cout << "Land" << endl;
	Meshy mesh2(0.35);

	t0=clock();
	LoadMeshyFromStl(mesh2, "inputs/Land.stl");
	t1=clock()-t0;
	cout << "time: " << t1 << endl;
	CPPUNIT_ASSERT_EQUAL((size_t)174, mesh2.readSliceTable().size());
	mesh2.dump(cout);
}

void ModelReaderTestCase::testSlicyWater()
{
	Meshy mesh(0.35);
	LoadMeshyFromStl(mesh, "inputs/Water.stl");

	const TrianglesInSlices& table = mesh.readSliceTable();

	unsigned int t0,t1;
	t0=clock();

	const vector<Triangle3d>& allTriangles = mesh.readAllTriangles();
	int sliceIndex = 0;
	for (TrianglesInSlices::const_iterator i = table.begin(); i != table.end(); i++)
	{
		const TriangleIndices& sliceables = *i;
		t1=clock()-t0;
		cout << "clock: " << t1 << endl;
		cout << "Slice: " << sliceIndex  << " ("<< sliceables.size()  << " triangles)" << endl;

		Slicy sy(0.000000001);
		cout  << " Triangles in this slice" << endl;
		for (TriangleIndices::const_iterator j = sliceables.begin(); j != sliceables.end(); j++ )
		{
			index_t index = (*j);
			const Triangle3d& triangle = allTriangles[index];
//			cout << "adding triangle # " << index << endl;
			sy.addTriangle(triangle);
		}
//		cout << sy << endl;
		sliceIndex ++;
	}
	t1=clock()-t0;
	cout << "clock: " << t1 << endl;
}


//
// NOTE: increase nozzle h between layers, or suffer the consequences
//








BGL::Point rotateAroundPoint(const BGL::Point &center, Scalar angle, const BGL::Point &p)
{
	// translate point back to origin:
	BGL::Point translated = p - center;

	BGL::Point rotated = rotate2d(translated, angle);
	// translate point back:
	BGL::Point r = rotated + center;
	return r;
}






void pathology( std::vector<Segment> &segments,
				const Limits& limits,
				double z,
				double tubeSpacing ,
				double angle,
				std::vector<std::vector<Segment> > &allTubes)
{
	assert(allTubes.size() == 0);

	// It is pitch black. You are likely to be eaten by a grue.



	// rotate segments for that cool look
	Point3d c = limits.center();
	Point toOrigin(-c.x, -c.y);
	Point toCenter(c.x, c.y);

	translateSegments(segments, toOrigin);
	rotateSegments(segments, angle);
	// translateSegments(segments, toCenter);

	int tubeCount = (limits.yMax - limits.yMin) / tubeSpacing;

	std::vector< std::set<Scalar> > intersects;
	// allocate
	intersects.resize(tubeCount);

	for (int i=0; i < tubeCount; i++)
	{
		Scalar y = -0.5 * (limits.yMax - limits.yMin) + i * tubeSpacing;
		std::set<Scalar> &lineCuts = intersects[i];
		for(std::vector<Segment>::iterator i= segments.begin(); i!= segments.end(); i++)
		{
			Segment &segment = *i;
			Scalar intersectionX, intersectionY;
			if (segmentSegmentIntersection(limits.xMin, y, limits.xMax, y, segment.a.x, segment.a.y, segment.b.x, segment.b.y,  intersectionX,  intersectionY))
			{
				lineCuts.insert(intersectionX);
			}
		}
	}

	allTubes.resize(tubeCount);
	for (int i=0; i < tubeCount; i++)
	{
		std::vector<Segment>& lineTubes = allTubes[i];
		Scalar y = -0.5 * (limits.yMax - limits.yMin) + i * tubeSpacing;
		std::set<Scalar> &lineCuts = intersects[i];

		Segment segment;
		bool inside = false;
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
				lineTubes.push_back(segment);
			}
		}
	}
	// unrotate the tube segments (they are tube rays, not cut triangles)
	for (int i=0; i < tubeCount; i++)
	{
		std::vector<Segment>& tubes = allTubes[i];
		rotateSegments(segments, -angle);
		translateSegments(segments, toCenter);

	}
}

std::string tubeScad(int layerIndex, Scalar z, const std::vector<std::vector<Segment> > &allTubes)
{
	stringstream ss;
	ss << "// layer " << layerIndex << endl;
	int rayCount = allTubes.size();
	for (int i=0; i < rayCount; i++)
	{
		const std::vector<Segment> &tubes = allTubes[i];
		ss << "// Ray " << i << " z=" << z << endl;
		for(int j=0; j<tubes.size(); j++)
		{
			const Segment &tube = tubes[j];
			ss << "// segment: " << tube.a << ", " << tube.b << ", z=" << z << endl;
		}
	}
	return ss.str();
}

void ModelReaderTestCase::testCutTriangle()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	BGL::Point3d vertex1(-1, 0, 0);
	BGL::Point3d vertex2(1, 0, 0);
	BGL::Point3d vertex3(0,0,1);

	BGL::Point a,b;
	bool cut;
	Scalar z = 0.5;
	cut = sliceTriangle(vertex1, vertex2, vertex3, z, a, b);
	cout << "Cutting at z="<< z<< ": a="<<a << " b=" << b << endl;

	CPPUNIT_ASSERT(cut);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.5, a.x, 0.00001 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0, a.y, 0.00001 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.5, b.x, 0.00001 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0, b.y, 0.00001 );
}

void ModelReaderTestCase::testRotate()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	BGL::Point center(4,4);
	BGL::Point a(4,3);

	Scalar angle = M_PI /2;
	BGL::Point b = rotateAroundPoint(center, angle, a);

	cout << endl << "rotated " << a << " around " << center << " by " << angle << " rads and got " << b << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 5.0, b.x, 0.00001 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.0, b.y, 0.00001 );

}





void ModelReaderTestCase::test3dKnot()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	Meshy mesh(0.35);
	double tubeSpacing = 1.0;

	LoadMeshyFromStl(mesh, "inputs/3D_Knot.stl");
	//LoadMeshyFromStl(mesh,"/home/hugo/code/Miracle-Grue/inputs/DURALEX_1.stl");
	mesh.dump(cout);

	cout << " Splitting up " << endl;
	const std::vector<Triangle3d> &allTriangles = mesh.readAllTriangles();
	const TrianglesInSlices &sliceTable = mesh.readSliceTable();
	const Limits& limits = mesh.readLimits();
	std::cout << "LIMITS: " << limits << std::endl;

	Limits tubularLimits = limits;
	tubularLimits.inflate(1.0, 1.0, 0.0);
	tubularLimits.tubularZ();

	stringstream tubeScadStr;

	stringstream outScadName;

	ScadTubeFile outlineScad("test_cases/modelReaderTestCase/output/3d_knot.scad");

	double dAngle = 0; // M_PI / 4;
	for(int i=0; i != sliceTable.size(); i++)
	{
		Scalar z = (i + 0.5) * mesh.readSliceHeight();
		const TriangleIndices &trianglesForSlice = sliceTable[i];

		std::vector<Segment> outlineSegments;

		// get 2D paths for outline
		segmentology(allTriangles, trianglesForSlice, z, outlineSegments);

		// get 2D rays for each slice
		std::vector<std::vector<Segment> > rowsOfTubes;
		pathology(outlineSegments, tubularLimits, z, tubeSpacing, dAngle * i, rowsOfTubes);

		stringstream stlName;
		stlName << "test_cases/modelReaderTestCase/output/3d_knot_triangles_" << i << ".stl";

		stringstream rayScadName;
		rayScadName << "test_cases/modelReaderTestCase/output/3d_knot_ray_" << i << ".scad";

		mesh.writeStlFileForLayer(i, stlName.str().c_str());
		outlineScad.writeTubesModule("out_", outlineSegments, i, z);
		outlineScad.writeStlModule("stl_", "3d_knot_triangles_",  i);

		// TubeFile raylineScad(rayScadName.str().c_str());
		std::vector<Segment> layerSegments;
		for(int j=0; j<rowsOfTubes.size(); j++)
		{
			const std::vector<Segment> &raySegments = rowsOfTubes[j];
			layerSegments.insert(layerSegments.end(), raySegments.begin(), raySegments.end());
			// raylineScad.writeTubesModule("rays_", i, rowsOfTubes[j], z);
		}
		outlineScad.writeTubesModule("fill_", layerSegments, i, z );
		//	cout << ss.str().c_str() << endl;

	}
	outlineScad.writeSwitcher(sliceTable.size());
	cout << endl << endl << "****************" << endl << endl;
	cout << tubeScadStr << endl;
}

void ModelReaderTestCase::testTubularInflate()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	Limits l0;

	Point3d p0(0,0,0);
	Point3d p1(8,4,1);

	l0.grow(p0);
	l0.grow(p1);

	Limits l1 = l0;
	l1.tubularZ();

	cout << "TUBULAR" << l0 << " is " << l1 << endl;

	double dx = l1.xMax - l1.xMin;
	double dy = l1.yMax - l1.yMin;
	CPPUNIT_ASSERT(dx > 8 );
	CPPUNIT_ASSERT(dy > 8 );

	double t = 0.000000000000001;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0, l0.zMin, t);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, l0.zMax, t);
}


