
#include <time.h>
#include <fstream>
#include <algorithm> // find
#include <iterator>  // distance
#include <iomanip>
#include <limits>
#include <set>



//#define  CPPUNIT_ENABLE_NAKED_ASSERT 1

#include <cppunit/config/SourcePrefix.h>
#include "SlicerTestCase.h"

#include "../Configuration.h"
#include "../ModelFileReaderOperation.h"

//#include "../SliceOperation.h"

#include "../BGL/BGLPoint.h"
#include "../BGL/BGLMesh3d.h"

#include "mgl/limits.h"
#include "mgl/meshy.h"
#include "mgl/slicy.h"



CPPUNIT_TEST_SUITE_REGISTRATION( SlicerTestCase );

using namespace std;
using namespace BGL;
using namespace mgl;



//CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
//CPPUNIT_ASSERT_EQUAL( 12, 12 );
//CPPUNIT_ASSERT( 12L == 12L );



// read this later
// Generating the vertices for an arbitrarily oriented cylinder is a common problem that is fairly straightforward to solve.
// Generating the orthonormal basis vectors used to find the endcap vertices.
// Let W = normalize(P2-P1). As you've noted you need a unit-length vector 'U' that is perpendicular to W. It will also be convenient to have a unit-length vector V perpendicular to both W and U. U, V, and W form the orthonormal basis for the cylinder.
// To find a vector perpendicular to W, simply cross W with the world x, y, or z axis. The only caveat is that two vectors which are nearly aligned will produce a cross product with a very small magnitude, which you may not be able to normalize. To avoid this problem, simply cross W with the world axis corresponding to the component of W whose absolute value is least. Then, normalize the result to get U. Finally, V = Cross(W, U).
// You now have two coordinate systems, each with basis vectors U, V, W, and with origins P1 and P2 respectively. You can then use simple trig to find the vertices of the endcaps.

void dumpIntList(const list<index_t> &edges)
{
	for(list<index_t>::const_iterator i=edges.begin(); i != edges.end(); i++)
	{
		cout << "  " <<  *i << endl;
	}
}


class Vector3d
{
	Scalar x,y,z;

public:
	Vector3d(){}

	Vector3d(Scalar x, Scalar y, Scalar z)
	            : x(x), y(y), z(z)
	{}

    Scalar operator[](unsigned i) const
    {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    Scalar& operator[](unsigned i)
    {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    void operator*=(const Scalar value)
    {
        x *= value;
        y *= value;
        z *= value;
    }

    Vector3d operator*(const real value) const
    {
        return Vector3d(x*value, y*value, z*value);
    }

    // Adds the given vector to this.
    void operator+=(const Vector3d& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }


    // Returns the value of the given vector added to this.
    Vector3d operator+(const Vector3d& v) const
    {
        return Vector3d(x+v.x, y+v.y, z+v.z);
    }


    void operator-=(const Vector3d& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }


    Vector3d operator-(const Vector3d& v) const
    {
        return Vector3d(x-v.x, y-v.y, z-v.z);
    }

    Vector3d vectorProduct(const Vector3d &vector) const
    {
        return Vector3d(y*vector.z-z*vector.y,
                       z*vector.x-x*vector.z,
                       x*vector.y-y*vector.x);
    }

    void vectorProductUpdate(const Vector3d &vector)
    {
    	 *this = vectorProduct(vector);
    }

    Scalar dotProduct(const Vector3d &vector) const
    {
    	return x*vector.x + y*vector.y + z*vector.z;
    }

    Scalar magnitude() const
    {
        return sqrt(squareMagnitude());
    }

    // Gets the squared magnitude of this vector.
    Scalar squareMagnitude() const
    {
        return x*x+y*y+z*z;
    }

    void normalise()
    {
        Scalar l = magnitude();
        if (l > 0)
        {
            (*this) *= ((Scalar)1)/l;
        }
    }

    Vector3d unit() const
    {
        Vector3d result = *this;
        result.normalise();
        return result;
    }


};


void SlicerTestCase::testNormals()
{

//	solid Default
//	  facet normal 7.902860e-01 -2.899449e-01 -5.397963e-01
//	    outer loop
//	      vertex 1.737416e+01 -4.841539e-01 3.165644e+01
//	      vertex 1.576195e+01 1.465057e-01 2.895734e+01
//	      vertex 1.652539e+01 9.044915e-01 2.966791e+01
//	    endloop
//	  endfacet
//	  facet normal 8.312133e-01 -1.572752e-01 -5.332438e-01
//	    outer loop
//	      vertex 1.737416e+01 -4.841539e-01 3.165644e+01
//	      vertex 1.654797e+01 -1.579103e+00 3.069153e+01
//	      vertex 1.576195e+01 1.465057e-01 2.895734e+01
//	    endloop
//	  endfacet
//	  facet normal 8.324613e-01 -5.462338e-01 -9.293486e-02
//	    outer loop
//	      vertex 1.705642e+01 -1.374897e+00 3.404571e+01
//	      vertex 1.654797e+01 -1.579103e+00 3.069153e+01
//	      vertex 1.737416e+01 -4.841539e-01 3.165644e+01
//	    endloop
//	  endfacet
//	  facet normal 8.883998e-01 -4.463071e-01 -1.074980e-01
//	    outer loop
//	      vertex 1.705642e+01 -1.374897e+00 3.404571e+01
//	      vertex 1.618299e+01 -2.837972e+00 3.290174e+01
//	      vertex 1.654797e+01 -1.579103e+00 3.069153e+01
//	    endloop
//	  endfacet
//	  facet normal 6.417707e-01 -6.720219e-01 3.694820e-01
//	    outer loop
//	      vertex 1.705642e+01 -1.374897e+00 3.404571e+01
//	      vertex 1.565729e+01 -1.529066e+00 3.619552e+01
//	      vertex 1.618299e+01 -2.837972e+00 3.290174e+01
//	    endloop
//	  endfacet



}


//
// Adds 2 triangles with a common edge
// to a Slicy
//
void SlicerTestCase::testSlicySimple()
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

void initConfig(Configuration &config)
{
	config["slicer"]["firstLayerZ"] = 0.11;
	config["slicer"]["layerH"] = 0.35;

}



void SlicerTestCase::testSlicyKnot()
{
	string modelFile = "inputs/3D_Knot.stl";

    Configuration config;
    initConfig(config);
	Meshy mesh(config["slicer"]["firstLayerZ"].asDouble(), config["slicer"]["layerH"].asDouble()); // 0.35
	loadMeshyFromStl(mesh, modelFile.c_str());

	cout << "file " << modelFile << endl;
	const SliceTable &sliceTable = mesh.readSliceTable();
	int layerCount = sliceTable.size();
	cout  << "Slice count: "<< layerCount << endl;
	const vector<Triangle3d> &allTriangles = mesh.readAllTriangles();
	cout << "Faces: " << allTriangles.size() << endl;
	cout << "layer " << layerCount-1 << " z: " << mesh.readLayerMeasure().sliceIndexToHeight(layerCount-1) << endl;

	int layerIndex = 44;
	CPPUNIT_ASSERT (layerIndex < layerCount);
	const TriangleIndices &trianglesInSlice = sliceTable[layerIndex];
	unsigned int triangleCount = trianglesInSlice.size();
	Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(layerIndex);
	cout << triangleCount <<" triangles in layer " << layerIndex  << " z = " << z << endl;

	// Load slice connectivity information
	Slicy slicy(1e-6);
	for (int i=0; i < triangleCount; i++)
	{
		unsigned int triangleIndex = trianglesInSlice[i];
		const Triangle3d& t = allTriangles[triangleIndex];
		slicy.addTriangle(t);
	}
	cout << slicy << endl;

	list<index_t> edges;
	slicy.fillEdgeList(z,edges);

	cout << "edges left: "  << edges.size()  << " / " << slicy.readEdges().size() << endl;
	dumpIntList(edges);

}





