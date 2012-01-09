
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

    Scalar operator[](unsigned int i) const
    {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    Scalar& operator[](unsigned int i)
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

    Vector3d crossProduct(const Vector3d &vector) const
    {
        return Vector3d(y*vector.z-z*vector.y,
                       z*vector.x-x*vector.z,
                       x*vector.y-y*vector.x);
    }

    void crossProductUpdate(const Vector3d &vector)
    {
    	 *this = crossProduct(vector);
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



std::ostream& operator<<(ostream& os, const Vector3d& v)
{
	os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
	return os;
}

class Triangle3
{
	Vector3d v0, v1, v2;


public:

	Triangle3(const Vector3d& v0, const Vector3d& v1, const Vector3d& v2)
	:v0(v0),v1(v1),v2(v2)
	{

	}

	Vector3d& operator[](unsigned int i)
	{
		 if (i == 0) return v0;
		 if (i == 1) return v1;
		 return v2;
	}

	Vector3d operator[](unsigned int i) const
	{
		 if (i == 0) return v0;
		 if (i == 1) return v1;
		 return v2;
	}

	Vector3d normal() const
	{
		Vector3d a = v1 - v0;
		Vector3d b = v2 - v0;

		Vector3d n = a.crossProduct(b);
		n.normalise();
		return n;
	}

	Vector3d cutDirection() const
	{
		Vector3d n = normal();
		Vector3d up(0,0,1);
		Vector3d d = n.crossProduct(up);
		return d;
	}

	//
	// Sorts the 3 points in assending order
	//
	void zSort(Vector3d &a, Vector3d &b, Vector3d &c ) const
	{

//		if a<b:
//		   if b<c:  a<b<c
//		   else:
//		      if a<c: a<c<b
//		      else:    c<a<b
//		else:
//		   if a<c:  b<a<c
//		   else:
//		      if c<b: c<b<a
//		      else:    b<a<c


		if (v0[2] < v1[2]) {
			if (v1[2] < v2[2]) {
				//v0<v1<v2
				a = v0;
				b = v1;
				c = v2;
			} else {
				if (v0[2] < v2[2]) {
					//v0<v2<v1
					a = v0;
					b = v2;
					c = v1;
				} else {
					//v2<v0<v1
					a = v2;
					b = v0;
					c = v1;
				}
			}
		} else {
			if (v0[2] < v2[2]) {
				//v1<v0<v2
				a = v1;
				b = v0;
				c = v2;

			} else {
				if (v2[2] < v1[2]) {
					//v2<v1<v0
					a = v2;
					b = v1;
					c = v0;
				} else {
					//v1<v2<v0
					a = v1;
					b = v2;
					c = v0;
				}
			}
		}

	}

};


void SlicerTestCase::testNormals()
{

	cout << endl;

//	solid Default
//	  facet normal 7.902860e-01 -2.899449e-01 -5.397963e-01
//	    outer loop
//	      vertex 1.737416e+01 -4.841539e-01 3.165644e+01
//	      vertex 1.576195e+01 1.465057e-01 2.895734e+01
//	      vertex 1.652539e+01 9.044915e-01 2.966791e+01
//	    endloop
//	  endfacet


	Vector3d v0(1.737416e+01, -4.841539e-01, 3.165644e+01);
	Vector3d v1(1.576195e+01, 1.465057e-01, 2.895734e+01);
	Vector3d v2(1.652539e+01, 9.044915e-01, 2.966791e+01);

	Vector3d a = v1 - v0;
	Vector3d b = v2 - v0;

	Vector3d n = a.crossProduct(b);
	n.normalise();
	cout << "Facet normal " << n;

	double tol = 1e-6;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  7.902860e-01, n[0], tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -2.899449e-01, n[1], tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -5.397963e-01, n[2], tol );
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
	cout << endl;
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

	list<index_t> faces;
	size_t faceCount = slicy.readFaces().size();
	for(index_t i=0; i< faceCount; i++)
	{
		faces.push_back(i);
	}

	list<Segment> loop;
	slicy.splitLoop(faces, loop);

	cout << "loop with " << loop.size() << "faces" << endl;
	cout << "faces left: "  << faces.size()  << " / " << slicy.readEdges().size() << endl;

//	list<index_t> edges;
//	slicy.fillEdgeList(z,edges);


	//dumpIntList(edges);

}



void SlicerTestCase::testCut()
{
	double tol = 1e-6;

	cout << endl;
	//	solid Default
	//	  facet normal 7.902860e-01 -2.899449e-01 -5.397963e-01
	//	    outer loop
	//	      vertex 1.737416e+01 -4.841539e-01 3.165644e+01
	//	      vertex 1.576195e+01 1.465057e-01 2.895734e+01
	//	      vertex 1.652539e+01 9.044915e-01 2.966791e+01
	//	    endloop
	//	  endfacet
	Triangle3 triangle(Vector3d(1.737416e+01, -4.841539e-01, 3.165644e+01), Vector3d(1.576195e+01, 1.465057e-01, 2.895734e+01), Vector3d(1.652539e+01, 9.044915e-01, 2.966791e+01));


	Vector3d cut = triangle.cutDirection();

	cout <<  "Cut:  "<< cut << endl;
	// the direction should be on a cpnstant z plane (on a slice)
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, cut[2], tol );

	// degenerate cases:  a flat triangle, a line and a point

	Triangle3 triangleFlat(Vector3d(1.737416e+01, -4.841539e-01, 0), Vector3d(1.576195e+01, 1.465057e-01, 0), Vector3d(1.652539e+01, 9.044915e-01, 0));
	cut = triangleFlat.cutDirection();

	// a flat triangle has no direction.
	cout <<  "Flat Cut:  "<< cut << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, cut.magnitude(), tol );

	Triangle3 line(Vector3d(0, 0, 0), Vector3d(1,1,1), Vector3d(3,3,3));
	cut = line.cutDirection();
	cout <<  "Line Cut:  "<< cut << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, cut.magnitude(), tol );

	Triangle3 point(Vector3d(10, 10, 10), Vector3d(10,10,10), Vector3d(10,10,10));
	cut = line.cutDirection();
	cout <<  "Point Cut:  "<< cut << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, cut.magnitude(), tol );


	// sorting the 3 points
	Vector3d a, b, c;
	triangle.zSort(a,b,c);
	// a=v1, b=v2, c=v0
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.576195e+01, a[0], tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 9.044915e-01, b[1], tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 3.165644e+01, c[2], tol);

}


