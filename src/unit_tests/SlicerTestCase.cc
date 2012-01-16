
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

#include "mgl/meshy.h"
#include "mgl/slicy.h"



CPPUNIT_TEST_SUITE_REGISTRATION( SlicerTestCase );

using namespace std;
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


	Vector3 v0(1.737416e+01, -4.841539e-01, 3.165644e+01);
	Vector3 v1(1.576195e+01, 1.465057e-01, 2.895734e+01);
	Vector3 v2(1.652539e+01, 9.044915e-01, 2.966791e+01);

	Vector3 a = v1 - v0;
	Vector3 b = v2 - v0;

	Vector3 n = a.crossProduct(b);
	n.normalise();
	cout << "Facet normal " << n << endl;

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

	Vector3 p0(0,0,0);
	Vector3 p1(0,1,0);
	Vector3 p2(1,1,0);
	Vector3 p3(1,0,0);

	Triangle3 t0(p0, p1, p2);
	Triangle3 t1(p0, p2, p3);

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
	Triangle3 triangle(Vector3(1.737416e+01, -4.841539e-01, 3.165644e+01), Vector3(1.576195e+01, 1.465057e-01, 2.895734e+01), Vector3(1.652539e+01, 9.044915e-01, 2.966791e+01));


	Vector3 cut = triangle.cutDirection();

	cout <<  "Cut:  "<< cut << endl;
	// the direction should be on a cpnstant z plane (on a slice)
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, cut[2], tol );

	// degenerate cases:  a flat triangle, a line and a Point2

	Triangle3 triangleFlat(Vector3(1.737416e+01, -4.841539e-01, 0), Vector3(1.576195e+01, 1.465057e-01, 0), Vector3(1.652539e+01, 9.044915e-01, 0));
	cut = triangleFlat.cutDirection();

	// a flat triangle has no direction.
	cout <<  "Flat Cut:  "<< cut << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, cut.magnitude(), tol );

	Triangle3 line(Vector3(0, 0, 0), Vector3(1,1,1), Vector3(3,3,3));
	cut = line.cutDirection();
	cout <<  "Line Cut:  "<< cut << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, cut.magnitude(), tol );

	Triangle3 Point2(Vector3(10, 10, 10), Vector3(10,10,10), Vector3(10,10,10));
	cut = line.cutDirection();
	cout <<  "Point2 Cut:  "<< cut << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 0, cut.magnitude(), tol );


	// sorting the 3 Point2s
	Vector3 a, b, c;
	triangle.zSort(a,b,c);
	// a=v1, b=v2, c=v0
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.576195e+01, a[0], tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 9.044915e-01, b[1], tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 3.165644e+01, c[2], tol);

}

void SlicerTestCase::testSlicyKnot_44()
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
	const vector<Triangle3> &allTriangles = mesh.readAllTriangles();
	cout << "Faces: " << allTriangles.size() << endl;
	cout << "layer " << layerCount-1 << " z: " << mesh.readLayerMeasure().sliceIndexToHeight(layerCount-1) << endl;

	int layerIndex = 44;
	CPPUNIT_ASSERT (layerIndex < layerCount);
	const TriangleIndices &trianglesInSlice = sliceTable[layerIndex];
	unsigned int triangleCount = trianglesInSlice.size();
	Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(layerIndex);
	cout << triangleCount <<" triangles in layer " << layerIndex  << " z = " << z << endl;

	std::list<Segment> cuts;
	double tol = 1e-6;
	// Load slice connectivity information
	for (int i=0; i < triangleCount; i++)
	{
		unsigned int triangleIndex = trianglesInSlice[i];
		const Triangle3& t = allTriangles[triangleIndex];
		Triangle3 triangle( Vector3(t[0].x,t[0].y,t[0].z ), Vector3(t[1].x,t[1].y,t[1].z ), Vector3(t[2].x,t[2].y,t[2].z ));

		if(triangle.cutDirection().magnitude() > tol)
		{
			Vector3 a,b;
			triangle.cut(z, a,b);
			Segment cut;
			cut.a.x = a.x;
			cut.a.y = a.y;
			cut.b.x = b.x;
			cut.b.y = b.y;
			cuts.push_back(cut);
		}
	}

	cout << "SEGMENTS" << endl;
	int i=0;
	for(std::list<Segment>::iterator it = cuts.begin(); it != cuts.end(); it++)
	{
		cout << i << ") " << it->a << " to " << it->b << endl;
		i++;
	}

}


void slicyTest()
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
	const vector<Triangle3> &allTriangles = mesh.readAllTriangles();
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
		const Triangle3& t = allTriangles[triangleIndex];
		slicy.addTriangle(t);
	}

	cout << slicy << endl;

	list<index_t> faces;
	size_t faceCount = slicy.readFaces().size();
	for(index_t i=0; i< faceCount; i++)
	{
		faces.push_back(i);

	}


	const Face &face = slicy.readFaces()[0];
	Segment cut;
	bool success = slicy.cutFace(z, face, cut);
	cout << "FACE cut " << cut.a << " to " << cut.b << endl;
	CPPUNIT_ASSERT(success);

	list<Segment> loop;
	slicy.splitLoop(z, faces, loop);

	cout << "First loop has " << loop.size() << " segments" << endl;

	size_t i=0;
	for(list<Segment>::iterator it = loop.begin(); it != loop.end(); it++)
	{
		cout << i << "] " << it->a << ", " << it->b << endl;
		i++;
	}

	cout << "loop with " << loop.size() << "faces" << endl;
	cout << "faces left: "  << faces.size()  << " / " << slicy.readEdges().size() << endl;

//	list<index_t> edges;
//	slicy.fillEdgeList(z,edges);
//	dumpIntList(edges);

}


bool Point2sSameSame(const Vector2 &a, const Vector2 &b)
{
	Scalar dx = a.x - b.x;
	Scalar dy = a.y - b.y;

	return sameSame(dx*dx + dy*dy,0);
}

void insetCorner(const Vector2 &a, const Vector2 &b, const Vector2 &c,
					Scalar insetDist,
					Scalar facetsPerCircle,
					std::vector<Vector2> &newCorner)
{

	Vector2 delta1(b.x - a.x, b.y - a.y);
	Vector2 delta2(c.x - b.x, c.y - b.y);

	Scalar d1 = sqrt(delta1.x * delta1.x + delta1.y * delta1.y);
	Scalar d2 = sqrt(delta2.x * delta2.x + delta2.y * delta2.y);

	assert(d1>0 && d2 > 0);

	Vector2 insetAB (delta1.y/d1 * insetDist, -delta1.x/d1 * insetDist);
	Vector2 insetA  (a.x + insetAB.x, a.y + insetAB.y);
	Vector2 insetBab(b.x + insetAB.x, b.y + insetAB.y);

	Vector2 insetBC (delta2.y/d2 * insetDist, -delta2.x/d2 * insetDist);
	Vector2 insetC  (c.x + insetBC.x, c.y + insetBC.y);
	Vector2 insetBbc(b.x + insetBC.x, b.y + insetBC.y);


	if( Point2sSameSame(insetBab, insetBbc) )
	{
		newCorner.push_back(insetBab);
		return;
	}
	assert(0);
}

void SlicerTestCase::testInset()
{

	Scalar insetDist = 2.0;

	CPPUNIT_ASSERT(1==0);

	Vector2 a(0,0);
	Vector2 b(10,0);
	Vector2 c(20,0);

	std::vector<Vector2> results;
	insetCorner(a,b,c,
			insetDist,
			12,
			results);


}




