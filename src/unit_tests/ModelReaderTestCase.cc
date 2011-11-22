
#include <time.h>
#include <fstream>
#include <algorithm> // find
#include <iterator>  // distance
#include <iomanip>


#define  CPPUNIT_ENABLE_NAKED_ASSERT 1

#include <cppunit/config/SourcePrefix.h>
#include "ModelReaderTestCase.h"

#include "../Configuration.h"
#include "../ModelFileReaderOperation.h"

#include "../BGL/BGLMesh3d.h"

CPPUNIT_TEST_SUITE_REGISTRATION( ModelReaderTestCase );

using namespace std;
using namespace BGL;


/*
CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
CPPUNIT_ASSERT_EQUAL( 12, 12 );
CPPUNIT_ASSERT( 12L == 12L );
*/

typedef unsigned int index_t;
typedef std::vector<index_t> TriangleIndices;
typedef std::vector<TriangleIndices> TrianglesInSlices;

// returns the minimum and maximum z for the 3 vertices of a triangle
void minMaxZ(const Triangle3d &t, Scalar &min,  Scalar &max )
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

class MeshyMess
{

public:
	std::string error;
	MeshyMess(const char *msg)
	 :error(msg)
	{

	}

};

class Meshy
{
public:
	Meshy(Scalar sliceHeight)
		:sliceHeight(sliceHeight)
	{
	}

	//
	// Adds a triangle to the global array and for each slice of interest
	//
	void addTriangle(Triangle3d &t)
	{
		Scalar min;
		Scalar max;
		minMaxZ(t, min, max);
		int minSliceIndex = floor( (min+0.5) / sliceHeight);
		int maxSliceIndex = floor( (max+0.5) / sliceHeight);


		if (maxSliceIndex > sliceTable.size() )
		{

			// cout << "resize to " << maxSliceIndex << endl;
			sliceTable.resize(maxSliceIndex); // make room for potentially new slices
			// cout << "new slices: " << sliceTable.size() << endl;
		}

		allTriangles.push_back(t);

		size_t newTriangleId = allTriangles.size() -1;


		// cout << "adding triangle " << newTriangleId << " to layer " << minSliceIndex  << " to " << maxSliceIndex << endl;
		for (int i= minSliceIndex; i< maxSliceIndex; i++)
		{
			TriangleIndices &trianglesForSlice = sliceTable[i];
			trianglesForSlice.push_back(newTriangleId);
		}
	}

	void dump(std::ostream &out)
	{
		out << "dumping " << this << endl;
		out << "Nb of triangles: " << allTriangles.size() << endl;
		size_t sliceCount = sliceTable.size();
		out << "triangles per slice: (" << sliceCount << " slices)" << endl;
		for (int i= 0; i< sliceCount; i++)
		{
			TriangleIndices &trianglesForSlice = sliceTable[i];
			//trianglesForSlice.push_back(newTriangleId);
			cout << "  slice " << i << " size: " << trianglesForSlice.size() << endl;
			//cout << "adding triangle " << newTriangleId << " to layer " << i << endl;
		}
	}

	const TrianglesInSlices& getSliceTable() const
	{
		return sliceTable;
	}

	const std::vector<Triangle3d>& getAllTriangles() const
	{
		return allTriangles;
	}


private:


	void writeTriangle(std::ostream &out, const Triangle3d& t) const
	{
		//  normalize( (v1-v0) cross (v2 - v0) )

		// normalize
		// y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x

		double n0=0;
		double n1=0;
		double n2=0;

		out << " facet normal " << n0 << " " << n1 << " " << n2 << endl;
		out << "  outer loop"<< endl;
		out << "    vertex " << t.vertex1.x << " " << t.vertex1.y << " " << t.vertex1.z << endl;
		out << "    vertex " << t.vertex2.x << " " << t.vertex2.y << " " << t.vertex2.z << endl;
		out << "    vertex " << t.vertex3.x << " " << t.vertex3.y << " " << t.vertex3.z << endl;
		out << "  end loop" << endl;
		out << " end facet" << endl;
	}
public:
	void writeStlFileForLayer(unsigned int layerIndex, const char* fileName) const
	{

		//solid Default
		//  facet normal 1.435159e-01 2.351864e-02 9.893685e-01
		//    outer loop
		//      vertex -7.388980e-02 -2.377973e+01 6.062650e+01
		//      vertex -1.193778e-01 -2.400027e+01 6.063834e+01
		//      vertex -4.402440e-06 -2.490700e+01 6.064258e+01
		//    endloop
		//  endfacet
		//endsolid Default

		ofstream out(fileName);
		if(!out) {
			stringstream ss;
			ss << "Can't open \"" << fileName << "\"";
			MeshyMess problem(ss.str().c_str());
			throw (problem);
		}
		stringstream ss;
		ss << setfill ('0') << setw(10);
		ss <<  "slice_" << layerIndex;
		string solidName = ss.str();
		// bingo!
		out << "solid Slice_" << layerIndex << endl;
		Scalar n0, n1, n2;
		out << scientific;
		const TriangleIndices &trianglesForSlice = sliceTable[layerIndex];
		for(std::vector<index_t>::const_iterator i = trianglesForSlice.begin(); i!= trianglesForSlice.end(); i++)
		{
			index_t index = *i;
			const Triangle3d &t = allTriangles[index];
			writeTriangle(out, t);
		}

		out << "end solid " << solidName;
		out.close();
	}

	Scalar sliceHeight;
	std::vector<Triangle3d>  allTriangles;
	TrianglesInSlices sliceTable;
};



size_t LoadMeshyFromStl(Meshy &meshy, const char* filename)
{

	Mesh3d mesh;
	int count = mesh.loadFromSTLFile(filename);
	for (Triangles3d::iterator i= mesh.triangles.begin(); i != mesh.triangles.end(); i++)
	{
		Triangle3d &t = *i;
		meshy.addTriangle(t);
	}
	return (size_t) count;
}



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

 	CPPUNIT_ASSERT_EQUAL((size_t)2, mesh.getSliceTable().size());

	t.vertex1 =Point3d(0,10,0);
	t.vertex2 =Point3d(0,10,2.6);
	t.vertex3 =Point3d(0,10,1);

	mesh.addTriangle(t);
	CPPUNIT_ASSERT_EQUAL((size_t)3, mesh.getSliceTable().size());
}

void ModelReaderTestCase::testLayerSplit()
{
	Meshy mesh(0.35);
	unsigned int t0, t1;
	t0 = clock();
	//LoadMeshyFromStl(mesh, "inputs/Water.stl");
	LoadMeshyFromStl(mesh, "inputs/soap_highres.stl");
	t1=clock()-t0;
	mesh.dump(cout);

	cout << " **** testLayerSplit " << endl;
	for(int i=0; i != mesh.sliceTable.size(); i++)
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
	CPPUNIT_ASSERT_EQUAL((size_t)173, mesh.getSliceTable().size());

	cout << "Land" << endl;
	Meshy mesh2(0.35);

	t0=clock();
	LoadMeshyFromStl(mesh2, "inputs/Land.stl");
	t1=clock()-t0;
	cout << "time: " << t1 << endl;
	CPPUNIT_ASSERT_EQUAL((size_t)174, mesh2.getSliceTable().size());
	mesh2.dump(cout);
}

void ModelReaderTestCase::testSlicyWater()
{
	Meshy mesh(0.35);
	LoadMeshyFromStl(mesh, "inputs/Water.stl");

	const TrianglesInSlices& table = mesh.getSliceTable();

	unsigned int t0,t1;
	t0=clock();

	const vector<Triangle3d>& allTriangles = mesh.getAllTriangles();
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






