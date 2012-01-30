




// #define  CPPUNIT_ENABLE_NAKED_ASSERT 1

#include <cppunit/config/SourcePrefix.h>
#include "SlicerTestCase.h"


//#include "../ModelFileReaderOperation.h"
/*
#include "mgl/meshy.h"

#include "mgl/scadtubefile.h"
*/
#include "mgl/configuration.h"

#include "mgl/slicy.h"
#include "mgl/shrinky.h"

CPPUNIT_TEST_SUITE_REGISTRATION( SlicerTestCase );

using namespace std;
using namespace mgl;


// CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
// CPPUNIT_ASSERT_EQUAL( 12, 12 );
// CPPUNIT_ASSERT( 12L == 12L );


// An edge event occurs when an edge col-
// lapses down to a point if its neighboring edges still have
// nonzero length, they become adjacent.
// Note that one of the endpoints of the disappearing edge
// can be a reflex vertex. A split event occurs when a
// reflex vertex collides with and splits an edge; the edges
// adjacent to the reflex vertex are now adjacent to the
// two parts of the split edge. Each split event divides a
// component of the shrinking polygon into two smaller
// components.  Each event introduces a
// node of degree three into the evolving straight skeleton.
// In degenerate cases, the straight skeleton can have
// vertices of degree higher than three, introduced by si-
// multaneous events at the same location.




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

	std::list<LineSegment2> cuts;
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
			LineSegment2 cut;
			cut.a.x = a.x;
			cut.a.y = a.y;
			cut.b.x = b.x;
			cut.b.y = b.y;
			cuts.push_back(cut);
		}
	}

	cout << "SEGMENTS" << endl;
	int i=0;
	for(std::list<LineSegment2>::iterator it = cuts.begin(); it != cuts.end(); it++)
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
	LineSegment2 cut;
	bool success = slicy.cutFace(z, face, cut);
	cout << "FACE cut " << cut.a << " to " << cut.b << endl;
	CPPUNIT_ASSERT(success);

	list<LineSegment2> loop;
	slicy.splitLoop(z, faces, loop);

	cout << "First loop has " << loop.size() << " segments" << endl;

	size_t i=0;
	for(list<LineSegment2>::iterator it = loop.begin(); it != loop.end(); it++)
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


/*

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
*/



void SlicerTestCase::testAngles()
{

	double t = 1e-10;

	Vector2 i(1,1);
	Vector2 j(0,0);
	Vector2 k(2,-2);

	Scalar angle0 = angleFromVector2s(i, k);

	Vector2 p (12,18);
	i += p;
	j += p;
	k += p;

	Scalar angle1 = angleFromPoint2s(i, j, k);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(M_PI/2, angle0, t);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0, angle1 - angle0, t);
}



void dumpAngles1(std::vector<LineSegment2> & segments)
{
    // grab 2 semgments, make sure they are in sequence
    // get the angle
    // inset
    // ijkAngles(insets);
    cout << "i, j, k, angle" << endl;
    cout << "---------------" << endl;
    for(int id = 0;id < segments.size();id++){
        //cout << id << " / " << insets.size() << endl;
        LineSegment2 seg = segments[id];
        unsigned int previousSegmentId;
        if(id == 0)
            previousSegmentId = segments.size() - 1;

        else
            previousSegmentId = id - 1;

        Vector2 & i = segments[previousSegmentId].a;
        Vector2 & j = segments[id].a;
        Vector2 & k = segments[id].b;
        Scalar angle = angleFromPoint2s(i, j, k);
        cout << i << " , " << j << ", " << k << " ,\t " << angle << endl;
    }
}

Scalar internalAngle(const Vector2 &ab0, const Vector2 &ab1)
{
	assert(ab0.squaredMagnitude() > 0);
	assert(ab1.squaredMagnitude() > 0);
	Scalar a1 = atan2(ab0.y, ab0.x);
	Scalar a2 = atan2(ab1.y, ab1.x);

	if(a1 < 0) a1 = 2 * M_PI - a1;
	if(a2 < 0) a2 = 2 * M_PI - a1;

	return a2-a1;

}

void dumpAngles3(std::vector<LineSegment2> & segments)
{
    // grab 2 semgments, make sure they are in sequence
    // get the angle
    // inset
    // ijkAngles(insets);

	cout << "dumpAngles3" << endl;
	cout << "v0, v1, angle" << endl;
    cout << "---------------" << endl;

    for(int id = 0;id < segments.size();id++){
        //cout << id << " / " << insets.size() << endl;
        LineSegment2 seg = segments[id];
        unsigned int previousSegmentId;
        if(id == 0)
            previousSegmentId = segments.size() - 1;

        else
            previousSegmentId = id - 1;

        Vector2 ab0 = segments[previousSegmentId].b - segments[previousSegmentId].a;
        Vector2 ab1 = segments[id].b - segments[id].a ;

        Scalar angle = internalAngle(ab0,ab1);
        cout << ab0 << " , " << ab1 << " ,\t " << angle << endl;
    }
}

void dumpAngles2(std::vector<LineSegment2> & segments)
{
    // grab 2 semgments, make sure they are in sequence
    // get the angle
    // inset
    // ijkAngles(insets);

	cout << "dumpAngles2" << endl;
	cout << "v0, v1, angle" << endl;
    cout << "---------------" << endl;

    for(int id = 0;id < segments.size();id++){
        //cout << id << " / " << insets.size() << endl;
        LineSegment2 seg = segments[id];
        unsigned int previousSegmentId;
        if(id == 0)
            previousSegmentId = segments.size() - 1;

        else
            previousSegmentId = id - 1;

        Vector2 ab0 = segments[previousSegmentId].b - segments[previousSegmentId].a;
        Vector2 ab1 = segments[id].b - segments[id].a ;

        Scalar angle = angleFromVector2s(ab0,ab1);
        cout << ab0 << " , " << ab1 << " ,\t " << angle << endl;
    }
}


/*
void trimSegments(const std::vector<LineSegment2> & segments,
					const std::vector<bool> &convexVertices,
					std::vector<LineSegment2>&newSegments,
					std::vector<bool>&newConvex)
{
	newSegments.reserve(segments.size());
	newConvex.reserve(segments.size());
	for(int id = 0;id < segments.size();id++){
		const LineSegment2 &currentSegment = segments[id];
		unsigned int previousSegmentId;
		if(id == 0)
			previousSegmentId = segments.size() - 1;
		else
			previousSegmentId = id - 1;
		const LineSegment2 &previousSegment = segments[previousSegmentId];


		bool convex = convexVertices[id];

		if (convex)
		{
			cout << "Trimming convex: " << id << endl;

			LineSegment2 newSegment;
			newSegment = currentSegment;
			bool trimmed = segmentSegmentIntersection(	previousSegment, currentSegment, newSegment.a);
			assert(trimmed);
			newSegments.push_back(newSegment);
			newConvex.push_back(convex);
		}
		// cout << i << " , " << j << ", " << k << " ,\t " << angle << ", " << convex << endl;

	}
}
*/


/*

void createConvexReflexLists(	const std::vector<LineSegment2> & segments,
								std::vector<unsigned int> &convex,
								std::vector<unsigned int> &reflex)
{
	for(int id = 0; id < segments.size(); id++){

	        const LineSegment2 &seg = segments[id];

	        unsigned int previousSegmentId;
	        if(id == 0)
	            previousSegmentId = segments.size() - 1;
	        else
	            previousSegmentId = id - 1;

	        const LineSegment2 &prevSeg = segments[previousSegmentId];

	        const Vector2 & i = prevSeg.a;
	        const Vector2 & j = seg.a;
	        const Vector2 & j2 = prevSeg.b;
	        const Vector2 & k = seg.b;
	        bool isSameSame = j.sameSame(j2);

	        if(!isSameSame)
	        {
	        	cout << endl << "ERROR" <<  endl;
	        	cout << "id: " << id << "/" << segments.size()<< ", prevId: " <<  previousSegmentId << endl;
	        	cout << "j: " << j << ", j2: "<< j2 << endl;
	        	cout << "SameSame " << isSameSame<< endl;
	        	CPPUNIT_ASSERT(isSameSame);
	        }

	        Scalar angle = angleFromPoint2s(i, j, k);
	        if( convexVertex(i,j,k))
	        	convex.push_back(id);
	        else
	        	reflex.push_back(id);
	    }
}

*/


// There are only two ways that a point can reenter the
// polygon after it leaves: either a slab endpoint overtakes
// an edge of the polygon, or a slab overtakes a convex
// vertex of the polygon. Since all the
// segments and their endpoints are moving at the same
// speed, neither of these transitions can occur.
void SlicerTestCase::testInset()
{
	cout << endl;

	Vector2 a(1,1);
	Vector2 b(1,-1);
	Vector2 c(-1,-1);
	Vector2 d(-1,1);

	std::vector<LineSegment2> square;
	square.push_back(LineSegment2(a,b));
	square.push_back(LineSegment2(b,c));
	square.push_back(LineSegment2(c,d));
	square.push_back(LineSegment2(d,a));

	std::vector< std::vector<LineSegment2 > > insetTable;
	std::vector<LineSegment2> &segments = square;

	Shrinky shrinky("./test_cases/slicerTestCase/output/testInsetSquare.scad", 0.25);
	Scalar insetDist = 1;
	unsigned int shells = 6;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- "<< endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<LineSegment2 >());
		std::vector<LineSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist,  finalInsets);
		segments = finalInsets;
	}
}

void SlicerTestCase::testInset2()
{
	cout << endl;


	Vector2 a( 1, 1);
	Vector2 b( 1,-1);
	Vector2 c (-1,-1); // ( 0,-1);

	Vector2 d( 0, 0);
	Vector2 e(-1, 0);
	Vector2 f(-1, 1);

	a *= 10;
	b *= 10;
	c *= 10;
	d *= 10;
	e *= 10;
	f *= 10;

	std::vector<LineSegment2> segs;
	segs.push_back(LineSegment2(a,b));
	segs.push_back(LineSegment2(b,c));
	segs.push_back(LineSegment2(c,d));
	segs.push_back(LineSegment2(d,e));
	segs.push_back(LineSegment2(e,f));
	segs.push_back(LineSegment2(f,a));

	std::vector< std::vector<LineSegment2 > > insetTable;


	insetTable.push_back(segs);


	std::vector<LineSegment2> &segments = segs;
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset2.scad", 0.25);

	Scalar insetDist = 1;
	unsigned int shells = 6;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- "<< endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<LineSegment2 >());
		std::vector<LineSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist,  finalInsets);
	    segments = finalInsets;
	}

}

void SlicerTestCase::testInset3()
{
	// shape of an M
	std::vector<LineSegment2> segs;
	segs.push_back(LineSegment2(Vector2(10.0, 10.0), Vector2(10.0, -10.0)));
	segs.push_back(LineSegment2(Vector2(10.0, -10.0), Vector2(-10.0, -10.0)));
	segs.push_back(LineSegment2(Vector2(-10.0, -10.0), Vector2(7, -2)));
	segs.push_back(LineSegment2(Vector2(7, -2), Vector2(7, 2)));
	segs.push_back(LineSegment2(Vector2(7, 2), Vector2(-10.0, 10.0)));
	segs.push_back(LineSegment2(Vector2(-10.0, 10.0), Vector2(10.0, 10.0)));

	InsetTable insetTable;


	insetTable.push_back(segs);


	std::vector<LineSegment2> &segments = segs;
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset3.scad", 0.25);

	Scalar insetDist = 1;
	unsigned int shells = 6;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- "<< endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<LineSegment2 >());
		std::vector<LineSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist,  finalInsets);
	    segments = finalInsets;
	}
}


