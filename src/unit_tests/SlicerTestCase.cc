#include <list>
#include <limits>

#include <cppunit/config/SourcePrefix.h>
#include "SlicerTestCase.h"

#include "mgl/mgl.cc"
#include "mgl/core.h"
#include "mgl/configuration.h"
#include "mgl/slicy.h"

#include "mgl/shrinky.h"
#include "mgl/meshy.h"

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

void initConfig(Configuration &config)
{
	config["slicer"]["firstLayerZ"] = 0.11;
	config["slicer"]["layerH"] = 0.35;
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

	std::list<TriangleSegment2> cuts;
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
			TriangleSegment2 cut;
			cut.a.x = a.x;
			cut.a.y = a.y;
			cut.b.x = b.x;
			cut.b.y = b.y;
			cuts.push_back(cut);
		}
	}

	cout << "SEGMENTS" << endl;
	int i=0;
	for(std::list<TriangleSegment2>::iterator it = cuts.begin(); it != cuts.end(); it++)
	{
		cout << i << ") " << it->a << " to " << it->b << endl;
		i++;
	}

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



void dumpAngles1(std::vector<TriangleSegment2> & segments)
{
    // grab 2 semgments, make sure they are in sequence
    // get the angle
    // inset
    // ijkAngles(insets);
    cout << "i, j, k, angle" << endl;
    cout << "---------------" << endl;
    for(int id = 0;id < segments.size();id++){
        //cout << id << " / " << insets.size() << endl;
        TriangleSegment2 seg = segments[id];
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

void dumpAngles3(std::vector<TriangleSegment2> & segments)
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
        TriangleSegment2 seg = segments[id];
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

void dumpAngles2(std::vector<TriangleSegment2> & segments)
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
        TriangleSegment2 seg = segments[id];
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
void trimSegments(const std::vector<TriangleSegment2> & segments,
					const std::vector<bool> &convexVertices,
					std::vector<TriangleSegment2>&newSegments,
					std::vector<bool>&newConvex)
{
	newSegments.reserve(segments.size());
	newConvex.reserve(segments.size());
	for(int id = 0;id < segments.size();id++){
		const TriangleSegment2 &currentSegment = segments[id];
		unsigned int previousSegmentId;
		if(id == 0)
			previousSegmentId = segments.size() - 1;
		else
			previousSegmentId = id - 1;
		const TriangleSegment2 &previousSegment = segments[previousSegmentId];


		bool convex = convexVertices[id];

		if (convex)
		{
			cout << "Trimming convex: " << id << endl;

			TriangleSegment2 newSegment;
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

void createConvexReflexLists(	const std::vector<TriangleSegment2> & segments,
								std::vector<unsigned int> &convex,
								std::vector<unsigned int> &reflex)
{
	for(int id = 0; id < segments.size(); id++){

	        const TriangleSegment2 &seg = segments[id];

	        unsigned int previousSegmentId;
	        if(id == 0)
	            previousSegmentId = segments.size() - 1;
	        else
	            previousSegmentId = id - 1;

	        const TriangleSegment2 &prevSeg = segments[previousSegmentId];

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

	std::vector<TriangleSegment2> square;
	square.push_back(TriangleSegment2(a,b));
	square.push_back(TriangleSegment2(b,c));
	square.push_back(TriangleSegment2(c,d));
	square.push_back(TriangleSegment2(d,a));

	std::vector< std::vector<TriangleSegment2 > > insetTable;
	std::vector<TriangleSegment2> &segments = square;

	Shrinky shrinky("./test_cases/slicerTestCase/output/testInsetSquare.scad", 0.25);
	Scalar insetDist = 1;
	unsigned int shells = 6;
	Scalar cuttOffLength = 1.0;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- "<< endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<TriangleSegment2 >());
		std::vector<TriangleSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist, cuttOffLength, finalInsets);
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

	std::vector<TriangleSegment2> segs;
	segs.push_back(TriangleSegment2(a,b));
	segs.push_back(TriangleSegment2(b,c));
	segs.push_back(TriangleSegment2(c,d));
	segs.push_back(TriangleSegment2(d,e));
	segs.push_back(TriangleSegment2(e,f));
	segs.push_back(TriangleSegment2(f,a));

	std::vector< std::vector<TriangleSegment2 > > insetTable;


	insetTable.push_back(segs);


	std::vector<TriangleSegment2> &segments = segs;
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset2.scad", 0.25);

	Scalar insetDist = 1;
	unsigned int shells = 6;
	Scalar cuttOffLength = 1.0;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- "<< endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<TriangleSegment2 >());
		std::vector<TriangleSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist, cuttOffLength, finalInsets);
	    segments = finalInsets;
	}

}

void SlicerTestCase::testInset3()
{
	// shape of an M
	std::vector<TriangleSegment2> segs;
	segs.push_back(TriangleSegment2(Vector2(10.0, 10.0), Vector2(10.0, -10.0)));
	segs.push_back(TriangleSegment2(Vector2(10.0, -10.0), Vector2(-10.0, -10.0)));
	segs.push_back(TriangleSegment2(Vector2(-10.0, -10.0), Vector2(7, -2)));
	segs.push_back(TriangleSegment2(Vector2(7, -2), Vector2(7, 2)));
	segs.push_back(TriangleSegment2(Vector2(7, 2), Vector2(-10.0, 10.0)));
	segs.push_back(TriangleSegment2(Vector2(-10.0, 10.0), Vector2(10.0, 10.0)));

	SegmentTable insetTable;
	insetTable.push_back(segs);

	std::vector<TriangleSegment2> &segments = segs;
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset3.scad", 0.25);

	Scalar insetDist = 1;
	unsigned int shells = 6;
	Scalar cuttOffLength = 1.0;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- "<< endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<TriangleSegment2 >());
		std::vector<TriangleSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist,  cuttOffLength, finalInsets);
	    segments = finalInsets;
	}
}

void SlicerTestCase::testInset4()
{

	// 3d knot slice 51 loop 5
/*
	std::vector<TriangleSegment2> segs;
	segs.push_back(TriangleSegment2(Vector2(-13.645961, -4.999121), Vector2(-14.336842, -6.058092+y)));
	segs.push_back(TriangleSegment2(Vector2(-14.336842, -6.058092), Vector2(-14.337009, -6.058348+y)));
	segs.push_back(TriangleSegment2(Vector2(-14.337009, -6.058348), Vector2(-14.802628, -6.773062+y)));
	segs.push_back(TriangleSegment2(Vector2(-14.802628, -6.773062), Vector2(-16.07675, -7.167382+y)));
	segs.push_back(TriangleSegment2(Vector2(-16.07675, -7.167382), Vector2(-16.076844, -7.167411+y)));
	segs.push_back(TriangleSegment2(Vector2(-16.076844, -7.167411), Vector2(-16.848004, -7.40625+y)));
	segs.push_back(TriangleSegment2(Vector2(-16.848004, -7.40625), Vector2(-13.645961, -4.999121+y)));

	*/
	Scalar x = 17.5;
	Scalar y = 2.5;

	std::vector<TriangleSegment2> segs;
	segs.push_back(TriangleSegment2(Vector2(x -13.645961, y-4.999121), Vector2(x-14.336842, -6.058092+y)));
	segs.push_back(TriangleSegment2(Vector2(x -14.336842, y-6.058092), Vector2(x-14.337009, -6.058348+y)));
	segs.push_back(TriangleSegment2(Vector2(x -14.337009, y-6.058348), Vector2(x-14.802628, -6.773062+y)));
	segs.push_back(TriangleSegment2(Vector2(x -14.802628, y-6.773062), Vector2(x-16.07675, -7.167382+y)));
	segs.push_back(TriangleSegment2(Vector2(x -16.07675, y-7.167382),  Vector2(x-16.076844, -7.167411+y)));
	segs.push_back(TriangleSegment2(Vector2(x -16.076844, y -7.167411), Vector2(x-16.848004, -7.40625+y)));

	segs.push_back(TriangleSegment2(Vector2(x -16.848004, y-7.40625), Vector2(x-18.376197, -6.967647+y)));
	segs.push_back(TriangleSegment2(Vector2(x -18.376197, y-6.967647), Vector2(x-19.263841, -6.695174+y)));
	segs.push_back(TriangleSegment2(Vector2(x -19.263841, y-6.695174), Vector2(x-21.191443, -4.741984+y)));
	segs.push_back(TriangleSegment2(Vector2(x -21.191443, y-4.741984), Vector2(x-22.156938, -2.118398+y)));
	segs.push_back(TriangleSegment2(Vector2(x -22.156938, y-2.118398), Vector2(x-21.689291, 0.504512+y)));
	segs.push_back(TriangleSegment2(Vector2(x -21.689291, y+0.504512), Vector2(x-20.943817, 1.445306+y)));
	segs.push_back(TriangleSegment2(Vector2(x -20.943817, y+1.445306), Vector2(x-20.943815, 1.445309+y)));
	segs.push_back(TriangleSegment2(Vector2(x -20.943815, y+1.445309), Vector2(x-20.225564, 2.351764+y)));
	segs.push_back(TriangleSegment2(Vector2(x -20.225564, y+2.351764), Vector2(x-18.768249, 2.821355+y)));
	segs.push_back(TriangleSegment2(Vector2(x -18.768249, y+2.821355), Vector2(x-18.76824, 2.821358+y)));
	segs.push_back(TriangleSegment2(Vector2(x -18.76824, y+2.821358), Vector2(x-18.228663, 2.995237+y)));
	segs.push_back(TriangleSegment2(Vector2(x -18.228663, y+2.995237), Vector2(x-16.146636, 2.324387+y)));
	segs.push_back(TriangleSegment2(Vector2(x -16.146636, y+2.324387), Vector2(x-15.572093, 1.664333+y)));
	segs.push_back(TriangleSegment2(Vector2(x -15.572093, y+1.664333), Vector2(x-15.572093, 1.664333+y)));
	segs.push_back(TriangleSegment2(Vector2(x -15.572093, y+1.664333), Vector2(x-14.511551, 0.493087+y)));
	segs.push_back(TriangleSegment2(Vector2(x -14.511551,y+ 0.493087), Vector2(x-14.468399, 0.329635+y)));
	segs.push_back(TriangleSegment2(Vector2(x -14.468399, y+0.329635), Vector2(x-14.455353, 0.288157+y)));
	segs.push_back(TriangleSegment2(Vector2(x -14.455353, y+0.288157), Vector2(x-13.845001, -1.377057+y)));
	segs.push_back(TriangleSegment2(Vector2(x -13.845001, y-1.377057), Vector2(x-13.50102, -2.315544+y)));
	segs.push_back(TriangleSegment2(Vector2(x -13.50102, y-2.315544), Vector2(x-13.546223, -2.823043+y)));
	segs.push_back(TriangleSegment2(Vector2(x -13.546223, y-2.823043), Vector2(x-13.655021, -4.041291+y)));
	segs.push_back(TriangleSegment2(Vector2(x -13.655021, y-4.041291), Vector2(x-13.656539, -4.0652+y)));
	segs.push_back(TriangleSegment2(Vector2(x -13.656539, y-4.0652), Vector2(x-13.657668, -4.095116+y)));
	segs.push_back(TriangleSegment2(Vector2(x -13.657668, y-4.095116), Vector2(x-13.657957, -4.119072+y)));
	segs.push_back(TriangleSegment2(Vector2(x -13.657957, y-4.119072), Vector2(x-13.64922, -4.760708+y)));
	segs.push_back(TriangleSegment2(Vector2(x -13.64922, y-4.760708), Vector2(x-13.649219, -4.760731+y)));
	segs.push_back(TriangleSegment2(Vector2(x -13.649219, y-4.760731), Vector2(x-13.645961, -4.999121+y)));

	SegmentTable insetTable;
	insetTable.push_back(segs);

	std::vector<TriangleSegment2> &segments = segs;
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset4.scad", 1);
	shrinky.dz = 0.05;
	Scalar insetDist = 1;
	unsigned int shells = 1;
	Scalar cuttOffLength = 1.0;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- " << endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<TriangleSegment2 >());

		std::vector<TriangleSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist, cuttOffLength, finalInsets);
	    segments = finalInsets;
	}

}

void SlicerTestCase::testHexagon()
{
	std::vector<TriangleSegment2> segs;
	segs.push_back(TriangleSegment2(Vector2(10.0, -5.773501), Vector2(9.89, -5.83701)));
	segs.push_back(TriangleSegment2(Vector2(9.89, -5.83701), Vector2(0.0, -11.54701)));
	segs.push_back(TriangleSegment2(Vector2(0.0, -11.54701), Vector2(-0.11, -11.483502)));
	segs.push_back(TriangleSegment2(Vector2(-0.11, -11.483502), Vector2(-10.0, -5.773501)));
	segs.push_back(TriangleSegment2(Vector2(-10.0, -5.773501), Vector2(-10.0, -5.646484)));
	segs.push_back(TriangleSegment2(Vector2(-10.0, -5.646484), Vector2(-10.0, 5.773504)));
	segs.push_back(TriangleSegment2(Vector2(-10.0, 5.773504), Vector2(-9.89, 5.837012)));
	segs.push_back(TriangleSegment2(Vector2(-9.89, 5.837012), Vector2(0.0, 11.54701)));
	segs.push_back(TriangleSegment2(Vector2(0.0, 11.54701), Vector2(0.11, 11.483502)));
	segs.push_back(TriangleSegment2(Vector2(0.11, 11.483502), Vector2(10.0, 5.773504)));
	segs.push_back(TriangleSegment2(Vector2(10.0, 5.773504), Vector2(10.0, 5.646487)));
	segs.push_back(TriangleSegment2(Vector2(10.0, 5.646487), Vector2(10.0, -5.773501)));

	SegmentTable insetTable;
	insetTable.push_back(segs);

	std::vector<TriangleSegment2> &segments = segs;
	Shrinky shrinky("./test_cases/slicerTestCase/output/hexagon.scad", 1);
	shrinky.dz = 0.05;
	Scalar insetDist = 1;
	unsigned int shells = 1;
	Scalar cuttOffLength = 1.0;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- " << endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<TriangleSegment2 >());

		std::vector<TriangleSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist, cuttOffLength, finalInsets);
		segments = finalInsets;
	}
}

void SlicerTestCase::testSliceTriangle()
{
	cout << endl << "Testing 'sliceTriangle'..." << endl;

	// this should really be passed to the sliceTriangle function
	Scalar tol = 1e-6;
	Vector3 v1, v2, v3;
	Scalar Z = 0;
	Vector3 a = Vector3(0, 0, 0);
	Vector3 b = Vector3(0, 0, 0);
	bool result;
	numeric_limits<Scalar> scalarLimits;

	///Testing triangle above slice height
	cout << endl << "\t testing above" << endl;
	v1 = Vector3(1, 2, 3);
	v2 = Vector3(2, 3, 4);
	v3 = Vector3(3, 4, 5);
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == false);

	///Testing triangle below slice height
	cout << endl << "\t testing below" << endl;
	Z = 8;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == false);

//	cout << endl << "\t testing == Z" << endl;
//	CPPUNIT_ASSERT(a.z == b.z);
//	CPPUNIT_ASSERT(a.z == Z);

	///Testing triangle exactly on slice height, should not produce a slice

	cout << endl << "\t testing flat face" << endl;
	v1 = Vector3(1, 2, 5);
	v2 = Vector3(2, 3, 5);
	v3 = Vector3(3, 4, 5);
	Z = 5;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == false);

	/// this should be based on a tolerance. can we pass tolerance to sliceTriangle?
	/// Testing triangle approximately at slice height (below tolerance value)
	cout << endl << "\t testing ~flat face" << endl;
	v1 = Vector3(1, 2, 4.999999999);
	v2 = Vector3(2, 3, 5.000000000);
	v3 = Vector3(3, 4, 5.000000001);
	Z = 5;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == false);

	///Testing very small triangles (slightly above tolerance)
	cout << endl << "\t testing tiny triangle" << endl;
	v1 = Vector3(0.0001, 0.0002, 0.0001);
	v2 = Vector3(0.0005, 0.0004, 0.0002);
	v3 = Vector3(0.0003, 0.0001, 0.0003);

	///Testing at middle Z height of very small tri.
	Z = 0.0002;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == true);
	// pre-calculated answer to compare against
	//a = (.0005, .0004, .0002) b = (.0002, .00015, .0002)
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.x, .0005, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.y, .0004, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, Z, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.x, .0002, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.y, .00015, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.z, Z, tol);

	///Testing below middle Z height of very small tri.
	Z = 0.00015;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == true);
	// pre-calculated answer to compare against
	//a = (.0003, .0003, .00015) b = (.00015, .000225, .00015)
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.x, .0003, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.y, .0003, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, Z, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.x, .00015, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.y, .000175, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.z, Z, tol);

	///Testing above middle Z height of very small tri.
	Z = 0.00025;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == true);
	// pre-calculated answer to compare against
	//a = (.00025, .000125, .00025) b = (.0004, .00025, .00025)
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.x, .00025, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.y, .000125, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, Z, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.x, .0004, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.y, .00025, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.z, Z, tol);


	///Testing triangle with side near Scalar_Min
//	Scalar smin = scalarLimits.min();
//	v1 = Vector3(0.0001, 0.0002, 1);
//	v2 = Vector3(0.0005, 0.0004, 2);
//	v3 = Vector3(0.0003, 0.0001, 3);
//	Z = 0.0002;
//	result = sliceTriangle(v1, v2, v3, Z, a, b);
//	CPPUNIT_ASSERT(result == true);
//	// pre-calculated answer to compare against
//	//a = (.0005, .0004, .0002) b = (.0002, .00015, .0002)
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.x, .0005, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.y, .0004, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, Z, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.x, .0002, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.y, .00015, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.z, Z, tol);

	///Testing triangle with slice below
	cout << endl << "test not fully implemented" << endl;

	///Testing very large triangles
	cout << endl << "\t testing GIANT triangle" << endl;
	cout << endl << "test not implemented" << endl;

//	v1 = Vector3(400000, 200000, 100000);
//	v2 = Vector3(-200000, 200000, 500000);
//	v3 = Vector3(600000, -400000, 700000);
//
//	///Testing below middle Z height of very large tri.
//	Z = 200000;
//	result = sliceTriangle(v1, v2, v3, Z, a, b);
//	CPPUNIT_ASSERT(result == true);
//	// pre-calculated answer to compare against
//	//a = (, , 10) b = (, , 10)
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.x, 250000, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.y, 200000, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, Z, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.x, 433333.3333333, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.y, 100000, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.z, Z, tol);
//
//	///Testing at middle Z height of very large tri.
//	Z = 500000;
//	result = sliceTriangle(v1, v2, v3, Z, a, b);
//	CPPUNIT_ASSERT(result == true);
//	// pre-calculated answer to compare against
//	//a = (, , 400000) b = (, , 400000)
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.x, -200000, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.y, 200000, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, Z, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.x, 533333.3333333, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.y, -200000, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.z, Z, tol);
//
//	///Testing above middle Z height of very large tri.
//	result = sliceTriangle(v1, v2, v3, Z, a, b);
//	CPPUNIT_ASSERT(result == true);
//	// pre-calculated answer to compare against
//	//a = (, , 450000) b = (, , 450000)
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.x, 550000, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.y, -250000, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, Z, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.x, 0, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.y, 50000, tol);
//	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.z, Z, tol);

	///Testing that the order in which variables are passed does not affect output
	cout << endl << "\t testing passed variable order" << endl;
	Vector3 order1[2], order2[2], order3[2];

	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, order1[0], order1[1]) == true);
	CPPUNIT_ASSERT(sliceTriangle(v1, v3, v2, Z, order2[0], order2[1]) == true);
	CPPUNIT_ASSERT(sliceTriangle(v2, v1, v3, Z, order3[0], order3[1]) == true);

	CPPUNIT_ASSERT(sameSame(order1[0].x, order2[0].x, tol) || sameSame(order1[0].x, order2[1].x, tol));
	CPPUNIT_ASSERT(sameSame(order1[0].y, order2[0].y, tol) || sameSame(order1[0].y, order2[1].y, tol));
	CPPUNIT_ASSERT(sameSame(order1[0].z, order2[0].z, tol) || sameSame(order1[0].z, order2[1].z, tol));

	CPPUNIT_ASSERT(sameSame(order1[0].x, order3[0].x, tol) || sameSame(order1[0].x, order3[1].x, tol));
	CPPUNIT_ASSERT(sameSame(order1[0].y, order3[0].y, tol) || sameSame(order1[0].y, order3[1].y, tol));
	CPPUNIT_ASSERT(sameSame(order1[0].z, order3[0].z, tol) || sameSame(order1[0].z, order3[1].z, tol));

	///Testing that z height outputs are equal
	cout << endl << "\t testing == Z" << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, b.z, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, Z, tol);




}


void SlicerTestCase::testFutureSlice()
{
	Vector3 v1, v2, v3;
	Vector3 a = Vector3(0, 0, 0);
	Vector3 b = Vector3(0, 0, 0);

	///Testing that heights that just touch one corner don't create segments
	cout << endl << "\t testing corners" << endl;
	v1 = Vector3(1, 2, 1);
	v2 = Vector3(5, 4, 2);
	v3 = Vector3(3, 1, 3);
	Scalar Z = 1;
	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, a, b) == false);
	Z = 3;
	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, a, b) == false);
	Z = 5;
	v2.z = 5;
	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, a, b) == false);

	///Testing that
	cout << endl << "\t testing length" << endl;
	cerr << "The current implementation fails this test case" << endl;
	v1 = Vector3(1, 2, 1);
	v2 = Vector3(5, 4, 2);
	v3 = Vector3(5, 4, 2);
	Z = 2;
	//	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, a, b) == false);

	/// if a triangle was divided in two along a line
	/// only one triangle should return a slice

	cout << endl << "\t testing split triangle" << endl;
	cerr << "Your current implementation fails this test case";
	Vector3 triangle1[3];
	triangle1[0] = Vector3(0, 0, 0);
	triangle1[1] = Vector3(3, 3, 3);
	triangle1[2] = Vector3(0, 0, 3);
	Vector3 triangle2[3];
	triangle2[0] = Vector3(0, 0, 3);
	triangle2[1] = Vector3(3, 3, 3);
	triangle2[2] = Vector3(0, 0, 6);
	Z = 3;

	bool sliceOne = sliceTriangle(triangle1[0], triangle1[1], triangle1[2], Z, a, b);
	bool sliceTwo = 	sliceTriangle(triangle2[0], triangle2[1], triangle2[2], Z, a, b);
	CPPUNIT_ASSERT(sliceOne != sliceTwo);

	cout << endl << "Finished testing 'sliceTriangle'" << endl;
}


void inset2scad(std::vector<TriangleSegment2> &segs, const char* filename, unsigned int shells)
{
    SegmentTable insetTable;
    insetTable.push_back(segs);
    std::vector<TriangleSegment2> & segments = segs;
    Shrinky shrinky(filename, 1);
    shrinky.dz = 0.05;
    Scalar insetDist = 0.35;
    Scalar cuttOffLength = 0.01; //0.0001;
    for(int i = 0;i < shells;i++){
        // cout << endl <<" *** *** Shell " <<i << endl;
        //dumpSegments(segments);
        insetTable.push_back(std::vector<TriangleSegment2>());
        std::vector<TriangleSegment2> & finalInsets = *insetTable.rbegin();
        try {
            shrinky.inset(segments, insetDist, cuttOffLength, finalInsets);
            segments = finalInsets;
        }catch(...){
            cout << "FAIL at inset " << i << endl;
        }
    }

}

void SlicerTestCase::testOpenPoly()
{
	std::vector<TriangleSegment2> segs;
	segs.push_back(TriangleSegment2(Vector2(-4.565341, 2.34464), Vector2(-4.05619, 2.768135)));
	segs.push_back(TriangleSegment2(Vector2(-4.05619, 2.768135), Vector2(-3.179025, 3.497492)));
	segs.push_back(TriangleSegment2(Vector2(-3.179025, 3.497492), Vector2(-2.608039, 4.064568)));
	segs.push_back(TriangleSegment2(Vector2(-2.608039, 4.064568), Vector2(-2.289756, 4.38048)));
	segs.push_back(TriangleSegment2(Vector2(-2.289756, 4.38048), Vector2(-0.908583, 4.875747)));
	segs.push_back(TriangleSegment2(Vector2(-0.908583, 4.875747), Vector2(0.563973, 5.403704)));
	segs.push_back(TriangleSegment2(Vector2(0.563973, 5.403704), Vector2(0.979924, 5.342237)));
	segs.push_back(TriangleSegment2(Vector2(0.979924, 5.342237), Vector2(2.466414, 5.122755)));
	segs.push_back(TriangleSegment2(Vector2(2.466414, 5.122755), Vector2(3.077935, 5.062797)));
	segs.push_back(TriangleSegment2(Vector2(3.077935, 5.062797), Vector2(3.289729, 5.042057)));
	segs.push_back(TriangleSegment2(Vector2(3.289729, 5.042057), Vector2(4.996244, 3.49904)));
	segs.push_back(TriangleSegment2(Vector2(4.996244, 3.49904), Vector2(5.194132, 3.320099)));
	segs.push_back(TriangleSegment2(Vector2(5.194132, 3.320099), Vector2(5.414868, 2.357953)));
	segs.push_back(TriangleSegment2(Vector2(5.414868, 2.357953), Vector2(5.773145, 0.687542)));
	segs.push_back(TriangleSegment2(Vector2(5.773145, 0.687542), Vector2(5.588746, 0.151448)));
	segs.push_back(TriangleSegment2(Vector2(5.588746, 0.151448), Vector2(4.826791, -2.062725)));
	segs.push_back(TriangleSegment2(Vector2(4.826791, -2.062725), Vector2(3.157747, -3.721238)));
	segs.push_back(TriangleSegment2(Vector2(3.157747, -3.721238), Vector2(2.633427, -4.24215)));
	segs.push_back(TriangleSegment2(Vector2(2.633427, -4.24215), Vector2(1.504083, -4.647671)));
	segs.push_back(TriangleSegment2(Vector2(1.504083, -4.647671), Vector2(-0.302466, -5.121101)));
	segs.push_back(TriangleSegment2(Vector2(-0.302466, -5.121101), Vector2(-1.291623, -4.919306)));
	segs.push_back(TriangleSegment2(Vector2(-1.291623, -4.919306), Vector2(-3.150975, -4.539908)));
	segs.push_back(TriangleSegment2(Vector2(-3.150975, -4.539908), Vector2(-4.479818, -3.315774)));
	segs.push_back(TriangleSegment2(Vector2(-4.479818, -3.315774), Vector2(-5.094482, -2.749503)));
	segs.push_back(TriangleSegment2(Vector2(-5.094482, -2.749503), Vector2(-5.464687, -0.947854)));
	segs.push_back(TriangleSegment2(Vector2(-5.464687, -0.947854), Vector2(-5.61223, -0.229557)));
	segs.push_back(TriangleSegment2(Vector2(-5.61223, -0.229557), Vector2(-5.060251, 1.127976)));
	segs.push_back(TriangleSegment2(Vector2(-5.060251, 1.127976), Vector2(-4.565341, 2.34464)));


	inset2scad(segs, "./test_cases/slicerTestCase/output/testOpen.scad", 1);

}

void SlicerTestCase::testSquareBug()
{
	std::vector<TriangleSegment2> segs;
	segs.push_back(TriangleSegment2(Vector2(-2.5, -2.5), Vector2(2.475, -2.5)));
	segs.push_back(TriangleSegment2(Vector2(2.475, -2.5), Vector2(2.5, -2.5)));
	segs.push_back(TriangleSegment2(Vector2(2.5, -2.5), Vector2(2.5, 2.475)));
	segs.push_back(TriangleSegment2(Vector2(2.5, 2.475), Vector2(2.5, 2.5)));
	segs.push_back(TriangleSegment2(Vector2(2.5, 2.5), Vector2(2.475, 2.5)));
	segs.push_back(TriangleSegment2(Vector2(2.475, 2.5), Vector2(-2.5, 2.5)));
	segs.push_back(TriangleSegment2(Vector2(-2.5, 2.5), Vector2(-2.5, 2.475)));
	segs.push_back(TriangleSegment2(Vector2(-2.5, 2.475), Vector2(-2.5, -2.5)));

	inset2scad(segs, "./test_cases/slicerTestCase/output/testSquareBug.scad",5);

}



void SlicerTestCase::testHexaBug()
{
	cout << endl;

	std::vector<TriangleSegment2> segs;
	segs.push_back(TriangleSegment2(Vector2(10.0, -5.773501), Vector2(9.9, -5.831236)));
	segs.push_back(TriangleSegment2(Vector2(9.9, -5.831236), Vector2(0.0, -11.54701)));
	segs.push_back(TriangleSegment2(Vector2(0.0, -11.54701), Vector2(-0.1, -11.489275)));
	segs.push_back(TriangleSegment2(Vector2(-0.1, -11.489275), Vector2(-10.0, -5.773501)));
	segs.push_back(TriangleSegment2(Vector2(-10.0, -5.773501), Vector2(-10.0, -5.658031)));
	segs.push_back(TriangleSegment2(Vector2(-10.0, -5.658031), Vector2(-10.0, 5.773504)));
	segs.push_back(TriangleSegment2(Vector2(-10.0, 5.773504), Vector2(-9.9, 5.831239)));
	segs.push_back(TriangleSegment2(Vector2(-9.9, 5.831239), Vector2(0.0, 11.54701)));
	segs.push_back(TriangleSegment2(Vector2(0.0, 11.54701), Vector2(0.1, 11.489275)));
	segs.push_back(TriangleSegment2(Vector2(0.1, 11.489275), Vector2(10.0, 5.773504)));
	segs.push_back(TriangleSegment2(Vector2(10.0, 5.773504), Vector2(10.0, 5.658034)));
	segs.push_back(TriangleSegment2(Vector2(10.0, 5.658034), Vector2(10.0, -5.773501)));

	unsigned int shells = 5;
    inset2scad(segs, "./test_cases/slicerTestCase/output/testHexaBug.scad", shells);

}

void SlicerTestCase::testKnotBug()
{
	cout << endl;
	std::vector<TriangleSegment2> segs;
/*	segs.push_back(TriangleSegment2(Vector2(-0.936271, 4.771724), Vector2(-0.701368, 3.540882)));
	segs.push_back(TriangleSegment2(Vector2(-0.701368, 3.540882), Vector2(-0.336485, 1.663085)));
	segs.push_back(TriangleSegment2(Vector2(-0.336485, 1.663085), Vector2(-0.441793, 1.231194)));
	segs.push_back(TriangleSegment2(Vector2(-0.441793, 1.231194), Vector2(-0.801342, -0.266116)));
	segs.push_back(TriangleSegment2(Vector2(-0.801342, -0.266116), Vector2(-1.004681, -1.115166)));
	segs.push_back(TriangleSegment2(Vector2(-1.004681, -1.115166), Vector2(-1.061293, -1.359116)));
	segs.push_back(TriangleSegment2(Vector2(-1.061293, -1.359116), Vector2(-2.875146, -3.51)));
	segs.push_back(TriangleSegment2(Vector2(-2.875146, -3.51), Vector2(-2.899034, -3.538635)));
	segs.push_back(TriangleSegment2(Vector2(-2.899034, -3.538635), Vector2(-2.941039, -3.551972)));
	segs.push_back(TriangleSegment2(Vector2(-2.941039, -3.551972), Vector2(-5.381388, -4.32758)));
	segs.push_back(TriangleSegment2(Vector2(-5.381388, -4.32758), Vector2(-5.988842, -4.128678)));
	segs.push_back(TriangleSegment2(Vector2(-5.988842, -4.128678), Vector2(-7.86314, -3.516169)));
	segs.push_back(TriangleSegment2(Vector2(-7.86314, -3.516169), Vector2(-7.872925, -3.504345)));
	segs.push_back(TriangleSegment2(Vector2(-7.872925, -3.504345), Vector2(-7.884779, -3.490039)));
	segs.push_back(TriangleSegment2(Vector2(-7.884779, -3.490039), Vector2(-8.853356, -2.306823)));
	segs.push_back(TriangleSegment2(Vector2(-8.853356, -2.306823), Vector2(-9.659453, -1.323408)));
	segs.push_back(TriangleSegment2(Vector2(-9.659453, -1.323408), Vector2(-9.71744, -1.051267)));
	segs.push_back(TriangleSegment2(Vector2(-9.71744, -1.051267), Vector2(-9.769822, -0.806228)));
	segs.push_back(TriangleSegment2(Vector2(-9.769822, -0.806228), Vector2(-10.007299, 0.372168)));
	segs.push_back(TriangleSegment2(Vector2(-10.007299, 0.372168), Vector2(-10.273321, 1.661756)));
	segs.push_back(TriangleSegment2(Vector2(-10.273321, 1.661756), Vector2(-10.243438, 1.785139)));
	segs.push_back(TriangleSegment2(Vector2(-10.243438, 1.785139), Vector2(-10.218416, 1.890554)));
	segs.push_back(TriangleSegment2(Vector2(-10.218416, 1.890554), Vector2(-9.893598, 3.182473)));
	segs.push_back(TriangleSegment2(Vector2(-9.893598, 3.182473), Vector2(-9.593159, 4.444312)));
	segs.push_back(TriangleSegment2(Vector2(-9.593159, 4.444312), Vector2(-9.569013, 4.537557)));
	segs.push_back(TriangleSegment2(Vector2(-9.569013, 4.537557), Vector2(-9.544076, 4.644635)));
	segs.push_back(TriangleSegment2(Vector2(-9.544076, 4.644635), Vector2(-8.656473, 5.68841)));
	segs.push_back(TriangleSegment2(Vector2(-8.656473, 5.68841), Vector2(-7.852934, 6.683882)));
	segs.push_back(TriangleSegment2(Vector2(-7.852934, 6.683882), Vector2(-7.776163, 6.775577)));
	segs.push_back(TriangleSegment2(Vector2(-7.776163, 6.775577), Vector2(-7.704205, 6.869564)));
	segs.push_back(TriangleSegment2(Vector2(-7.704205, 6.869564), Vector2(-6.219482, 7.39256)));
	segs.push_back(TriangleSegment2(Vector2(-6.219482, 7.39256), Vector2(-5.239015, 7.784622)));
	segs.push_back(TriangleSegment2(Vector2(-5.239015, 7.784622), Vector2(-4.233641, 7.475214)));
	segs.push_back(TriangleSegment2(Vector2(-4.233641, 7.475214), Vector2(-2.749868, 7.055984)));
	segs.push_back(TriangleSegment2(Vector2(-2.749868, 7.055984), Vector2(-1.787018, 5.843316)));
	segs.push_back(TriangleSegment2(Vector2(-1.787018, 5.843316), Vector2(-0.936271, 4.771724)));
*/
	segs.push_back(TriangleSegment2(Vector2(1.707056, -4.472806), Vector2(4.151384, -6.128421)));
	segs.push_back(TriangleSegment2(Vector2(4.151384, -6.128421), Vector2(4.2242, -6.182884)));
	segs.push_back(TriangleSegment2(Vector2(4.2242, -6.182884), Vector2(4.236332, -6.231379)));
	segs.push_back(TriangleSegment2(Vector2(4.236332, -6.231379), Vector2(4.955788, -8.644448)));
	segs.push_back(TriangleSegment2(Vector2(4.955788, -8.644448), Vector2(4.950397, -8.657197)));
	segs.push_back(TriangleSegment2(Vector2(4.950397, -8.657197), Vector2(4.943079, -8.677898)));
	segs.push_back(TriangleSegment2(Vector2(4.943079, -8.677898), Vector2(4.313023, -10.169516)));
	segs.push_back(TriangleSegment2(Vector2(4.313023, -10.169516), Vector2(3.995515, -11.142574)));
	segs.push_back(TriangleSegment2(Vector2(3.995515, -11.142574), Vector2(3.528162, -11.5063)));
	segs.push_back(TriangleSegment2(Vector2(3.528162, -11.5063), Vector2(3.131938, -11.871103)));
	segs.push_back(TriangleSegment2(Vector2(3.131938, -11.871103), Vector2(2.385168, -12.434162)));
	segs.push_back(TriangleSegment2(Vector2(2.385168, -12.434162), Vector2(1.69226, -13.079852)));
	segs.push_back(TriangleSegment2(Vector2(1.69226, -13.079852), Vector2(1.210771, -13.206586)));
	segs.push_back(TriangleSegment2(Vector2(1.210771, -13.206586), Vector2(0.898318, -13.323702)));
	segs.push_back(TriangleSegment2(Vector2(0.898318, -13.323702), Vector2(-0.169906, -13.593133)));
	segs.push_back(TriangleSegment2(Vector2(-0.169906, -13.593133), Vector2(-1.34021, -14.037541)));
	segs.push_back(TriangleSegment2(Vector2(-1.34021, -14.037541), Vector2(-1.412327, -14.053808)));
	segs.push_back(TriangleSegment2(Vector2(-1.412327, -14.053808), Vector2(-1.508382, -14.087873)));
	segs.push_back(TriangleSegment2(Vector2(-1.508382, -14.087873), Vector2(-2.844753, -13.994394)));
	segs.push_back(TriangleSegment2(Vector2(-2.844753, -13.994394), Vector2(-3.47251, -14.032617)));
	segs.push_back(TriangleSegment2(Vector2(-3.47251, -14.032617), Vector2(-4.1956, -13.984061)));
	segs.push_back(TriangleSegment2(Vector2(-4.1956, -13.984061), Vector2(-5.401424, -14.041935)));
	segs.push_back(TriangleSegment2(Vector2(-5.401424, -14.041935), Vector2(-5.447891, -14.037575)));
	segs.push_back(TriangleSegment2(Vector2(-5.447891, -14.037575), Vector2(-5.565337, -14.036714)));
	segs.push_back(TriangleSegment2(Vector2(-5.565337, -14.036714), Vector2(-6.853898, -13.674497)));
	segs.push_back(TriangleSegment2(Vector2(-6.853898, -13.674497), Vector2(-7.175243, -13.626994)));
	segs.push_back(TriangleSegment2(Vector2(-7.175243, -13.626994), Vector2(-8.342194, -13.359098)));
	segs.push_back(TriangleSegment2(Vector2(-8.342194, -13.359098), Vector2(-8.906587, -13.2754)));
	segs.push_back(TriangleSegment2(Vector2(-8.906587, -13.2754), Vector2(-10.127703, -13.01961)));
	segs.push_back(TriangleSegment2(Vector2(-10.127703, -13.01961), Vector2(-10.81857, -12.900668)));
	segs.push_back(TriangleSegment2(Vector2(-10.81857, -12.900668), Vector2(-12.963774, -12.437108)));
	segs.push_back(TriangleSegment2(Vector2(-12.963774, -12.437108), Vector2(-13.244769, -12.37967)));
	segs.push_back(TriangleSegment2(Vector2(-13.244769, -12.37967), Vector2(-13.703812, -12.270545)));
	segs.push_back(TriangleSegment2(Vector2(-13.703812, -12.270545), Vector2(-16.252395, -10.828427)));
	segs.push_back(TriangleSegment2(Vector2(-16.252395, -10.828427), Vector2(-17.641797, -9.822212)));
	segs.push_back(TriangleSegment2(Vector2(-17.641797, -9.822212), Vector2(-19.410258, -7.215718)));
	segs.push_back(TriangleSegment2(Vector2(-19.410258, -7.215718), Vector2(-19.660132, -6.712659)));
	segs.push_back(TriangleSegment2(Vector2(-19.660132, -6.712659), Vector2(-19.667256, -6.24888)));
	segs.push_back(TriangleSegment2(Vector2(-19.667256, -6.24888), Vector2(-19.706478, -3.689043)));
	segs.push_back(TriangleSegment2(Vector2(-19.706478, -3.689043), Vector2(-18.880039, -2.420304)));
	segs.push_back(TriangleSegment2(Vector2(-18.880039, -2.420304), Vector2(-18.275914, -1.492409)));
	segs.push_back(TriangleSegment2(Vector2(-18.275914, -1.492409), Vector2(-16.872456, -1.058245)));
	segs.push_back(TriangleSegment2(Vector2(-16.872456, -1.058245), Vector2(-15.752105, -0.711392)));
	segs.push_back(TriangleSegment2(Vector2(-15.752105, -0.711392), Vector2(-14.866808, -0.965393)));
	segs.push_back(TriangleSegment2(Vector2(-14.866808, -0.965393), Vector2(-13.591955, -1.331675)));
	segs.push_back(TriangleSegment2(Vector2(-13.591955, -1.331675), Vector2(-13.074093, -1.468555)));
	segs.push_back(TriangleSegment2(Vector2(-13.074093, -1.468555), Vector2(-12.682618, -1.571315)));
	segs.push_back(TriangleSegment2(Vector2(-12.682618, -1.571315), Vector2(-11.578087, -2.373732)));
	segs.push_back(TriangleSegment2(Vector2(-11.578087, -2.373732), Vector2(-10.913518, -2.85576)));
	segs.push_back(TriangleSegment2(Vector2(-10.913518, -2.85576), Vector2(-10.280687, -3.218391)));
	segs.push_back(TriangleSegment2(Vector2(-10.280687, -3.218391), Vector2(-9.273547, -3.785162)));
	segs.push_back(TriangleSegment2(Vector2(-9.273547, -3.785162), Vector2(-9.187383, -3.821357)));
	segs.push_back(TriangleSegment2(Vector2(-9.187383, -3.821357), Vector2(-8.867937, -3.946021)));
	segs.push_back(TriangleSegment2(Vector2(-8.867937, -3.946021), Vector2(-8.279024, -4.34526)));
	segs.push_back(TriangleSegment2(Vector2(-8.279024, -4.34526), Vector2(-8.182986, -4.405148)));
	segs.push_back(TriangleSegment2(Vector2(-8.182986, -4.405148), Vector2(-7.429527, -4.688028)));
	segs.push_back(TriangleSegment2(Vector2(-7.429527, -4.688028), Vector2(-7.278723, -4.728657)));
	segs.push_back(TriangleSegment2(Vector2(-7.278723, -4.728657), Vector2(-6.558254, -4.828098)));
	segs.push_back(TriangleSegment2(Vector2(-6.558254, -4.828098), Vector2(-6.374283, -4.825934)));
	segs.push_back(TriangleSegment2(Vector2(-6.374283, -4.825934), Vector2(-5.559394, -4.794283)));
	segs.push_back(TriangleSegment2(Vector2(-5.559394, -4.794283), Vector2(-5.353345, -4.753359)));
	segs.push_back(TriangleSegment2(Vector2(-5.353345, -4.753359), Vector2(-4.305126, -4.585348)));
	segs.push_back(TriangleSegment2(Vector2(-4.305126, -4.585348), Vector2(-4.143335, -4.536296)));
	segs.push_back(TriangleSegment2(Vector2(-4.143335, -4.536296), Vector2(-3.028448, -4.274805)));
	segs.push_back(TriangleSegment2(Vector2(-3.028448, -4.274805), Vector2(-2.696555, -4.263748)));
	segs.push_back(TriangleSegment2(Vector2(-2.696555, -4.263748), Vector2(-2.553325, -4.27172)));
	segs.push_back(TriangleSegment2(Vector2(-2.553325, -4.27172), Vector2(-0.920331, -4.269894)));
	segs.push_back(TriangleSegment2(Vector2(-0.920331, -4.269894), Vector2(0.720399, -4.398875)));
	segs.push_back(TriangleSegment2(Vector2(0.720399, -4.398875), Vector2(1.210134, -4.419774)));
	segs.push_back(TriangleSegment2(Vector2(1.210134, -4.419774), Vector2(1.707056, -4.472806)));

    inset2scad(segs, "./test_cases/slicerTestCase/output/testKnot.scad", 5);
}
