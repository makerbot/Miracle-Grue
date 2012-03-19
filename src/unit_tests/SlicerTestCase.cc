#include <list>
#include <limits>

#include <cppunit/config/SourcePrefix.h>
#include "SlicerTestCase.h"

#include "insetTests.h"

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

	Shrinky shrinky("./test_cases/slicerTestCase/output/testInsetSquare.scad");
	Scalar insetDist = 1;
	unsigned int shells = 6;
	Scalar cuttOffLength = 1.0;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- "<< endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<TriangleSegment2 >());
		std::vector<TriangleSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist, finalInsets);
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
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset2.scad");

	Scalar insetDist = 1;
	unsigned int shells = 6;
	Scalar cuttOffLength = 1.0;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- "<< endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<TriangleSegment2 >());
		std::vector<TriangleSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist,  finalInsets);
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
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset3.scad");

	Scalar insetDist = 1;
	unsigned int shells = 6;
	Scalar cuttOffLength = 1.0;

	for (int i=0; i < shells; i++)
	{
		cout << "\n" << insetTable.size() << " ----- "<< endl;
		//dumpSegments(segments);
		insetTable.push_back(std::vector<TriangleSegment2 >());
		std::vector<TriangleSegment2> &finalInsets = insetTable[insetTable.size() -1] ;
		shrinky.inset(segments, insetDist,  finalInsets);
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
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset4.scad");
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
		shrinky.inset(segments, insetDist,  finalInsets);
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
	Shrinky shrinky("./test_cases/slicerTestCase/output/hexagon.scad");
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
		shrinky.inset(segments, insetDist,  finalInsets);
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


void inset2scad(std::vector<TriangleSegment2> &segs,
		const char* filename,
		unsigned int shells,
		Scalar layerW,
		Scalar insetDistanceFactor = 0.9)
{
	cout << endl << endl <<" *** *** inset2scad " << filename << endl;
    SegmentTable insetTable;
    insetTable.push_back(segs);
    std::vector<TriangleSegment2> *p = &segs;
    Shrinky shrinky(filename);
    shrinky.dz = 0.05;

    cout << "inset2scad: segments =  "  << segs.size() << endl;
    int i;
    try
    {
    	 for(i = 0;i < shells;i++)
    	 {
    		 Scalar insetDistance = i ==0? 0.5*layerW: insetDistanceFactor *layerW;
    		 cout << "  Shell " << i << " inset: " << insetDistance << endl;
			//dumpSegments(segments);
			insetTable.push_back(std::vector<TriangleSegment2>());

			std::vector<TriangleSegment2> &segments = *p;
			std::vector<TriangleSegment2> & finalInsets = *insetTable.rbegin();

			shrinky.inset(segments, insetDistance , finalInsets);
			p = &finalInsets;
    	 }
    }
    catch(Messup &fup)
    {
            cout << "  !!FAIL at inset " << i << endl;
            cout << fup.error << endl;
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


	inset2scad(segs, "./test_cases/slicerTestCase/output/testOpen.scad", 1, 0.35);

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

	inset2scad(segs, "./test_cases/slicerTestCase/output/testSquareBug.scad",5, 0.35);

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
    inset2scad(segs, "./test_cases/slicerTestCase/output/testHexaBug.scad", shells, 0.35);

}

void SlicerTestCase::testKnotBug()
{
	cout << endl;
	std::vector<TriangleSegment2> segs;
	segs.push_back(TriangleSegment2(Vector2(-0.936271, 4.771724), Vector2(-0.701368, 3.540882)));
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

	inset2scad(segs, "./test_cases/slicerTestCase/output/testKnot_a.scad", 5, 0.35);

	segs.clear();
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

    inset2scad(segs, "./test_cases/slicerTestCase/output/testKnot_b.scad", 5, 0.35);
}

void SlicerTestCase::testKnot89()
{
	cout << endl;
	std::vector<TriangleSegment2> segs;
	double x = 15;
	double y = 5;
	segs.push_back(TriangleSegment2(Vector2(-10.216373+x, 4.813872+y), Vector2(-10.755816+x, 4.031714+y)));
	segs.push_back(TriangleSegment2(Vector2(-10.755816+x, 4.031714+y), Vector2(-11.152832+x, 3.454443+y)));
	segs.push_back(TriangleSegment2(Vector2(-11.152832+x, 3.454443+y), Vector2(-11.574555+x, 2.621187+y)));
	segs.push_back(TriangleSegment2(Vector2(-11.574555+x, 2.621187+y), Vector2(-12.011074+x, 1.760019+y)));
	segs.push_back(TriangleSegment2(Vector2(-12.011074+x, 1.760019+y), Vector2(-12.604249+x, 1.250614+y)));
	segs.push_back(TriangleSegment2(Vector2(-12.604249+x, 1.250614+y), Vector2(-13.404085+x, 0.564558+y)));
	segs.push_back(TriangleSegment2(Vector2(-13.404085+x, 0.564558+y), Vector2(-14.004434+x, -0.033595+y)));
	segs.push_back(TriangleSegment2(Vector2(-14.004434+x, -0.033595+y), Vector2(-14.418341+x, -0.445597+y)));
	segs.push_back(TriangleSegment2(Vector2(-14.418341+x, -0.445597+y), Vector2(-15.759898+x, -0.844797+y)));
	segs.push_back(TriangleSegment2(Vector2(-15.759898+x, -0.844797+y), Vector2(-17.093409+x, -1.241085+y)));
	segs.push_back(TriangleSegment2(Vector2(-17.093409+x, -1.241085+y), Vector2(-17.918947+x, -0.966703+y)));
	segs.push_back(TriangleSegment2(Vector2(-17.918947+x, -0.966703+y), Vector2(-19.529327+x, -0.431058+y)));
	segs.push_back(TriangleSegment2(Vector2(-19.529327+x, -0.431058+y), Vector2(-20.16373+x, 0.471904+y)));
	segs.push_back(TriangleSegment2(Vector2(-20.16373+x, 0.471904+y), Vector2(-21.073318+x, 1.767005+y)));
	segs.push_back(TriangleSegment2(Vector2(-21.073318+x, 1.767005+y), Vector2(-21.27314+x, 4.274367+y)));
	segs.push_back(TriangleSegment2(Vector2(-21.27314+x, 4.274367+y), Vector2(-21.312089+x, 4.764018+y)));
	segs.push_back(TriangleSegment2(Vector2(-21.312089+x, 4.764018+y), Vector2(-21.053422+x, 5.449001+y)));
	segs.push_back(TriangleSegment2(Vector2(-21.053422+x, 5.449001+y), Vector2(-19.868528+x, 7.78337+y)));
	segs.push_back(TriangleSegment2(Vector2(-19.868528+x, 7.78337+y), Vector2(-17.424465+x, 9.880023+y)));
	segs.push_back(TriangleSegment2(Vector2(-17.424465+x, 9.880023+y), Vector2(-17.244495+x, 10.008028+y)));
	segs.push_back(TriangleSegment2(Vector2(-17.244495+x, 10.008028+y), Vector2(-16.012399+x, 10.321173+y)));
	segs.push_back(TriangleSegment2(Vector2(-16.012399+x, 10.321173+y), Vector2(-13.789806+x, 10.886378+y)));
	segs.push_back(TriangleSegment2(Vector2(-13.789806+x, 10.886378+y), Vector2(-13.281309+x, 10.74224+y)));
	segs.push_back(TriangleSegment2(Vector2(-13.281309+x, 10.74224+y), Vector2(-10.561624+x, 10.161692+y)));
	segs.push_back(TriangleSegment2(Vector2(-10.561624+x, 10.161692+y), Vector2(-10.034294+x, 9.210631+y)));
	segs.push_back(TriangleSegment2(Vector2(-10.034294+x, 9.210631+y), Vector2(-9.252324+x, 7.917581+y)));
	segs.push_back(TriangleSegment2(Vector2(-9.252324+x, 7.917581+y), Vector2(-9.350885+x, 7.795203+y)));
	segs.push_back(TriangleSegment2(Vector2(-9.350885+x, 7.795203+y), Vector2(-9.376159+x, 7.75899+y)));
	segs.push_back(TriangleSegment2(Vector2(-9.376159+x, 7.75899+y), Vector2(-9.791112+x, 6.617759+y)));
	segs.push_back(TriangleSegment2(Vector2(-9.791112+x, 6.617759+y), Vector2(-10.055766+x, 5.852983+y)));
	segs.push_back(TriangleSegment2(Vector2(-10.055766+x, 5.852983+y), Vector2(-10.139224+x, 5.317391+y)));
	segs.push_back(TriangleSegment2(Vector2(-10.139224+x, 5.317391+y), Vector2(-10.216373+x, 4.813872+y)));
    inset2scad(segs, "./test_cases/slicerTestCase/output/testKnot89_0.scad", 5, 0.35);

    segs.clear();
    x = 0;
    y = 0; //-8;
    segs.push_back(TriangleSegment2(Vector2(7.361654+x, 8.084142+y), Vector2(7.037812+x, 6.743804+y)));
    segs.push_back(TriangleSegment2(Vector2(7.037812+x, 6.743804+y), Vector2(6.779825+x, 5.577612+y)));
    segs.push_back(TriangleSegment2(Vector2(6.779825+x, 5.577612+y), Vector2(5.03416+x, 4.10054+y)));
    segs.push_back(TriangleSegment2(Vector2(5.03416+x, 4.10054+y), Vector2(4.726941+x, 3.836237+y)));
    segs.push_back(TriangleSegment2(Vector2(4.726941+x, 3.836237+y), Vector2(4.490134+x, 3.79795+y)));
    segs.push_back(TriangleSegment2(Vector2(4.490134+x, 3.79795+y), Vector2(3.168209+x, 3.609145+y)));
    segs.push_back(TriangleSegment2(Vector2(3.168209+x, 3.609145+y), Vector2(1.81713+x, 3.411402+y)));
    segs.push_back(TriangleSegment2(Vector2(1.81713+x, 3.411402+y), Vector2(1.567947+x, 3.382797+y)));
    segs.push_back(TriangleSegment2(Vector2(1.567947+x, 3.382797+y), Vector2(-0.244102+x, 3.927449+y)));
    segs.push_back(TriangleSegment2(Vector2(-0.244102+x, 3.927449+y), Vector2(-1.879337+x, 4.4789+y)));
    segs.push_back(TriangleSegment2(Vector2(-1.879337+x, 4.4789+y), Vector2(-2.01958+x, 4.521378+y)));
    segs.push_back(TriangleSegment2(Vector2(-2.01958+x, 4.521378+y), Vector2(-2.193651+x, 4.58332+y)));
    segs.push_back(TriangleSegment2(Vector2(-2.193651+x, 4.58332+y), Vector2(-3.423879+x, 5.581961+y)));
    segs.push_back(TriangleSegment2(Vector2(-3.423879+x, 5.581961+y), Vector2(-4.082049+x, 6.173753+y)));
    segs.push_back(TriangleSegment2(Vector2(-4.082049+x, 6.173753+y), Vector2(-4.624946+x, 6.588834+y)));
    segs.push_back(TriangleSegment2(Vector2(-4.624946+x, 6.588834+y), Vector2(-5.595969+x, 7.430504+y)));
    segs.push_back(TriangleSegment2(Vector2(-5.595969+x, 7.430504+y), Vector2(-5.647796+x, 7.465518+y)));
    segs.push_back(TriangleSegment2(Vector2(-5.647796+x, 7.465518+y), Vector2(-5.826879+x, 7.60382+y)));
    segs.push_back(TriangleSegment2(Vector2(-5.826879+x, 7.60382+y), Vector2(-6.127494+x, 9.140453+y)));
    segs.push_back(TriangleSegment2(Vector2(-6.127494+x, 9.140453+y), Vector2(-6.418214+x, 10.389987+y)));
    segs.push_back(TriangleSegment2(Vector2(-6.418214+x, 10.389987+y), Vector2(-5.781488+x, 10.799818+y)));
    segs.push_back(TriangleSegment2(Vector2(-5.781488+x, 10.799818+y), Vector2(-5.346511+x, 11.294469+y)));
    segs.push_back(TriangleSegment2(Vector2(-5.346511+x, 11.294469+y), Vector2(-4.992029+x, 11.650013+y)));
    segs.push_back(TriangleSegment2(Vector2(-4.992029+x, 11.650013+y), Vector2(-4.606595+x, 12.265442+y)));
    segs.push_back(TriangleSegment2(Vector2(-4.606595+x, 12.265442+y), Vector2(-4.098665+x, 12.390328+y)));
    segs.push_back(TriangleSegment2(Vector2(-4.098665+x, 12.390328+y), Vector2(-3.748474+x, 12.545966+y)));
    segs.push_back(TriangleSegment2(Vector2(-3.748474+x, 12.545966+y), Vector2(-3.037514+x, 12.793462+y)));
    segs.push_back(TriangleSegment2(Vector2(-3.037514+x, 12.793462+y), Vector2(-1.930969+x, 13.396515+y)));
    segs.push_back(TriangleSegment2(Vector2(-1.930969+x, 13.396515+y), Vector2(-1.848433+x, 13.392425+y)));
    segs.push_back(TriangleSegment2(Vector2(-1.848433+x, 13.392425+y), Vector2(-1.786827+x, 13.39736+y)));
    segs.push_back(TriangleSegment2(Vector2(-1.786827+x, 13.39736+y), Vector2(-0.657551+x, 13.387664+y)));
    segs.push_back(TriangleSegment2(Vector2(-0.657551+x, 13.387664+y), Vector2(0.374751+x, 13.49708+y)));
    segs.push_back(TriangleSegment2(Vector2(0.374751+x, 13.49708+y), Vector2(0.683379+x, 13.509392+y)));
    segs.push_back(TriangleSegment2(Vector2(0.683379+x, 13.509392+y), Vector2(1.123435+x, 13.566937+y)));
    segs.push_back(TriangleSegment2(Vector2(1.123435+x, 13.566937+y), Vector2(1.958958+x, 13.260512+y)));
    segs.push_back(TriangleSegment2(Vector2(1.958958+x, 13.260512+y), Vector2(2.675811+x, 13.062422+y)));
    segs.push_back(TriangleSegment2(Vector2(2.675811+x, 13.062422+y), Vector2(3.273953+x, 12.856118+y)));
    segs.push_back(TriangleSegment2(Vector2(3.273953+x, 12.856118+y), Vector2(4.099944+x, 12.626384+y)));
    segs.push_back(TriangleSegment2(Vector2(4.099944+x, 12.626384+y), Vector2(4.518746+x, 12.241275+y)));
    segs.push_back(TriangleSegment2(Vector2(4.518746+x, 12.241275+y), Vector2(4.946495+x, 11.893057+y)));
    segs.push_back(TriangleSegment2(Vector2(4.946495+x, 11.893057+y), Vector2(5.608902+x, 11.293462+y)));
    segs.push_back(TriangleSegment2(Vector2(5.608902+x, 11.293462+y), Vector2(6.370898+x, 10.658388+y)));
    segs.push_back(TriangleSegment2(Vector2(6.370898+x, 10.658388+y), Vector2(6.5515+x, 10.157591+y)));
    segs.push_back(TriangleSegment2(Vector2(6.5515+x, 10.157591+y), Vector2(7.134971+x, 8.653125+y)));
    segs.push_back(TriangleSegment2(Vector2(7.134971+x, 8.653125+y), Vector2(7.361654+x, 8.084142+y)));

    inset2scad(segs, "./test_cases/slicerTestCase/output/testKnot89_1.scad", 5, 0.35);

    segs.clear();
    x = -6;
    y = 5;
    segs.push_back(TriangleSegment2(Vector2(12.352334+x, 2.973561+y), Vector2(12.583564+x, 3.068644+y)));
    segs.push_back(TriangleSegment2(Vector2(12.583564+x, 3.068644+y), Vector2(15.290237+x, 2.186688+y)));
    segs.push_back(TriangleSegment2(Vector2(15.290237+x, 2.186688+y), Vector2(15.545186+x, 0.776714+y)));
    segs.push_back(TriangleSegment2(Vector2(15.545186+x, 0.776714+y), Vector2(16.006126+x, -0.322287+y)));
    segs.push_back(TriangleSegment2(Vector2(16.006126+x, -0.322287+y), Vector2(15.857028+x, -0.836436+y)));
    segs.push_back(TriangleSegment2(Vector2(15.857028+x, -0.836436+y), Vector2(15.676402+x, -1.958762+y)));
    segs.push_back(TriangleSegment2(Vector2(15.676402+x, -1.958762+y), Vector2(15.595127+x, -3.284348+y)));
    segs.push_back(TriangleSegment2(Vector2(15.595127+x, -3.284348+y), Vector2(14.995552+x, -4.315786+y)));
    segs.push_back(TriangleSegment2(Vector2(14.995552+x, -4.315786+y), Vector2(14.430402+x, -5.539793+y)));
    segs.push_back(TriangleSegment2(Vector2(14.430402+x, -5.539793+y), Vector2(13.572304+x, -6.51587+y)));
    segs.push_back(TriangleSegment2(Vector2(13.572304+x, -6.51587+y), Vector2(12.564352+x, -7.453327+y)));
    segs.push_back(TriangleSegment2(Vector2(12.564352+x, -7.453327+y), Vector2(11.96162+x, -8.044656+y)));
    segs.push_back(TriangleSegment2(Vector2(11.96162+x, -8.044656+y), Vector2(11.406709+x, -8.218566+y)));
    segs.push_back(TriangleSegment2(Vector2(11.406709+x, -8.218566+y), Vector2(10.880462+x, -8.419052+y)));
    segs.push_back(TriangleSegment2(Vector2(10.880462+x, -8.419052+y), Vector2(10.04104+x, -8.689451+y)));
    segs.push_back(TriangleSegment2(Vector2(10.04104+x, -8.689451+y), Vector2(9.120715+x, -9.02775+y)));
    segs.push_back(TriangleSegment2(Vector2(9.120715+x, -9.02775+y), Vector2(7.820139+x, -8.826739+y)));
    segs.push_back(TriangleSegment2(Vector2(7.820139+x, -8.826739+y), Vector2(6.395705+x, -8.599489+y)));
    segs.push_back(TriangleSegment2(Vector2(6.395705+x, -8.599489+y), Vector2(4.515046+x, -6.843429+y)));
    segs.push_back(TriangleSegment2(Vector2(4.515046+x, -6.843429+y), Vector2(4.032511+x, -4.194877+y)));
    segs.push_back(TriangleSegment2(Vector2(4.032511+x, -4.194877+y), Vector2(4.59481+x, -2.782621+y)));
    segs.push_back(TriangleSegment2(Vector2(4.59481+x, -2.782621+y), Vector2(5.219365+x, -1.232909+y)));
    segs.push_back(TriangleSegment2(Vector2(5.219365+x, -1.232909+y), Vector2(7.88199+x, 1.194908+y)));
    segs.push_back(TriangleSegment2(Vector2(7.88199+x, 1.194908+y), Vector2(8.14106+x, 1.425135+y)));
    segs.push_back(TriangleSegment2(Vector2(8.14106+x, 1.425135+y), Vector2(10.447005+x, 2.260258+y)));
    segs.push_back(TriangleSegment2(Vector2(10.447005+x, 2.260258+y), Vector2(11.083053+x, 2.525394+y)));
    segs.push_back(TriangleSegment2(Vector2(11.083053+x, 2.525394+y), Vector2(12.290309+x, 2.943339+y)));
    segs.push_back(TriangleSegment2(Vector2(12.290309+x, 2.943339+y), Vector2(12.352334+x, 2.973561+y)));

    inset2scad(segs, "./test_cases/slicerTestCase/output/testKnot89_2.scad", 5, 0.35);
}


bool attachSegments(TriangleSegment2 &first, TriangleSegment2 &next, Scalar elongation);

void SlicerTestCase::scratch()
{
	TriangleSegment2 s0;
	s0.a  = Vector2(4.357, -0.522);
	s0.b  = Vector2(4.363, -0.539);

	TriangleSegment2 s1;
	s1.a =  Vector2(4.363, -0.534);
	s1.b =  Vector2(4.362, -0.535);

	Scalar elongation = 10;

	bool success = attachSegments(s0, s1, elongation);
//	CPPUNIT_ASSERT(success);
}


void SlicerTestCase::test_slice_0_loop_0()
{
	std::vector<TriangleSegment2> segs;
	double x = 0;
	double y = 0;

	segs.push_back(TriangleSegment2(Vector2(-25.567, 20.4295), Vector2(-24.9313, 20.4295)));
	segs.push_back(TriangleSegment2(Vector2(-24.9313, 20.4295), Vector2(25.567, 20.4295)));
	segs.push_back(TriangleSegment2(Vector2(25.567, 20.4295), Vector2(25.567, -22.0152)));
	segs.push_back(TriangleSegment2(Vector2(25.567, -22.0152), Vector2(25.567, -22.5025)));
	segs.push_back(TriangleSegment2(Vector2(25.567, -22.5025), Vector2(24.9313, -22.5025)));
	segs.push_back(TriangleSegment2(Vector2(24.9313, -22.5025), Vector2(-25.567, -22.5025)));
	segs.push_back(TriangleSegment2(Vector2(-25.567, -22.5025), Vector2(-25.567, 19.9422)));
	segs.push_back(TriangleSegment2(Vector2(-25.567, 19.9422), Vector2(-25.567, 20.4295)));

	inset2scad(segs, "./test_cases/slicerTestCase/output/test_slice_0_loop_0.scad", 5, 0.35);
	segs.clear();

	segs.push_back(TriangleSegment2(Vector2(15.4327, 4.01493), Vector2(15.3874, 3.95251)));
	segs.push_back(TriangleSegment2(Vector2(15.3874, 3.95251), Vector2(15.3761, 3.9335)));
	segs.push_back(TriangleSegment2(Vector2(15.3761, 3.9335), Vector2(15.2193, 4.09319)));
	segs.push_back(TriangleSegment2(Vector2(15.2193, 4.09319), Vector2(15.4386, 4.11746)));
	segs.push_back(TriangleSegment2(Vector2(15.4386, 4.11746), Vector2(15.4327, 4.01493)));
	inset2scad(segs, "./test_cases/slicerTestCase/output/null_bisector.scad", 5, 0.35);
}







void SlicerTestCase::testAttach()
{
	{
		TriangleSegment2 hori;
		hori.a  = Vector2(10,0);
		hori.b  = Vector2(5,0);

		TriangleSegment2 verti;
		verti.a =  Vector2(0,5);
		verti.b =  Vector2(0,10);

		Scalar elongation =10;

		bool success = attachSegments(hori, verti, elongation);
		CPPUNIT_ASSERT(success);
	}

	{
		TriangleSegment2 hori;
		hori.a  = Vector2(10,0);
		hori.b  = Vector2(5,0);

		TriangleSegment2 verti;
		verti.a =  Vector2(0,5);
		verti.b =  Vector2(0,10);

		Scalar elongation =10;

		bool success = attachSegments(hori, verti, elongation);
		CPPUNIT_ASSERT(success);
	}
}



void SlicerTestCase::test_slice_60_loop_5()
{

	std::vector<TriangleSegment2> segs;
	double x = -12;
	double y = -18;
	segs.push_back(TriangleSegment2(Vector2(x+ 16.371815, 17.457659 + y ), Vector2(x+ 15.795807, 14.884597 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 15.795807, 14.884597 + y ), Vector2(x+ 15.561806, 14.676522 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 15.561806, 14.676522 + y ), Vector2(x+ 13.873463, 13.175436 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 13.873463, 13.175436 + y ), Vector2(x+ 13.869994, 13.174956 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 13.869994, 13.174956 + y ), Vector2(x+ 13.861545, 13.173787 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 13.861545, 13.173787 + y ), Vector2(x+ 11.786621, 12.76951 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 11.786621, 12.76951 + y ), Vector2(x+ 11.019653, 12.620338 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 11.019653, 12.620338 + y ), Vector2(x+ 9.882464, 12.929237 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 9.882464, 12.929237 + y ), Vector2(x+ 8.05574, 13.426012 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 8.05574, 13.426012 + y ), Vector2(x+ 8.043185, 13.428187 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 8.043185, 13.428187 + y ), Vector2(x+ 8.034853, 13.429637 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 8.034853, 13.429637 + y ), Vector2(x+ 6.513252, 14.497804 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 6.513252, 14.497804 + y ), Vector2(x+ 5.571676, 15.159099 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 5.571676, 15.159099 + y ), Vector2(x+ 5.237582, 15.894582 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 5.237582, 15.894582 + y ), Vector2(x+ 4.459752, 17.60635 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 4.459752, 17.60635 + y ), Vector2(x+ 4.540218, 17.982435 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 4.540218, 17.982435 + y ), Vector2(x+ 4.996733, 20.115702 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 4.996733, 20.115702 + y ), Vector2(x+ 5.680344, 20.752028 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 5.680344, 20.752028 + y ), Vector2(x+ 7.038937, 22.016047 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 7.038937, 22.016047 + y ), Vector2(x+ 9.878306, 22.757028 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 9.878306, 22.757028 + y ), Vector2(x+ 10.039503, 22.799052 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 10.039503, 22.799052 + y ), Vector2(x+ 10.252997, 22.762199 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 10.252997, 22.762199 + y ), Vector2(x+ 13.035107, 22.007035 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 13.035107, 22.007035 + y ), Vector2(x+ 14.375286, 20.872789 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 14.375286, 20.872789 + y ), Vector2(x+ 15.349479, 20.048227 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 15.349479, 20.048227 + y ), Vector2(x+ 16.367202, 17.482036 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 16.367202, 17.482036 + y ), Vector2(x+ 16.373233, 17.463994 + y )));
	segs.push_back(TriangleSegment2(Vector2(x+ 16.373233, 17.463994 + y ), Vector2(x+ 16.371815, 17.457659 + y )));

	std::vector<TriangleSegment2> segments;
	clip(segs, 5, 20, segments);

	Scalar insetDist = 0.9 * 0.6;
	inset2scad(segments, "./test_cases/slicerTestCase/output/test_slice_60_loop_5.scad", 4, insetDist);
}

void SlicerTestCase::test_slice_34_loop_0()
{
	std::vector<TriangleSegment2> segs;
	Scalar x = 5;
	Scalar y = -7;

	segs.push_back(TriangleSegment2(Vector2(-2.292475+x, 5.35237+y), Vector2(-2.669171+x, 4.804205+y)));
	segs.push_back(TriangleSegment2(Vector2(-2.669171+x, 4.804205+y), Vector2(-3.47445+x, 4.332189+y)));
	segs.push_back(TriangleSegment2(Vector2(-3.47445+x, 4.332189+y), Vector2(-4.37211+x, 3.888145+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.37211+x, 3.888145+y), Vector2(-4.942911+x, 4.025232+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.942911+x, 4.025232+y), Vector2(-5.588408+x, 4.141849+y)));
	segs.push_back(TriangleSegment2(Vector2(-5.588408+x, 4.141849+y), Vector2(-5.705417+x, 4.296731+y)));
	segs.push_back(TriangleSegment2(Vector2(-5.705417+x, 4.296731+y), Vector2(-6.549601+x, 5.238513+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.549601+x, 5.238513+y), Vector2(-6.662077+x, 6.157351+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.662077+x, 6.157351+y), Vector2(-6.928422+x, 7.31343+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.928422+x, 7.31343+y), Vector2(-6.661552+x, 8.253937+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.661552+x, 8.253937+y), Vector2(-6.533501+x, 9.043324+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.533501+x, 9.043324+y), Vector2(-6.367566+x, 9.576332+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.367566+x, 9.576332+y), Vector2(-6.273918+x, 10.082348+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.273918+x, 10.082348+y), Vector2(-6.093011+x, 10.350597+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.093011+x, 10.350597+y), Vector2(-5.966672+x, 10.603812+y)));
	segs.push_back(TriangleSegment2(Vector2(-5.966672+x, 10.603812+y), Vector2(-5.312356+x, 11.579534+y)));
	segs.push_back(TriangleSegment2(Vector2(-5.312356+x, 11.579534+y), Vector2(-4.927272+x, 12.327523+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.927272+x, 12.327523+y), Vector2(-4.650247+x, 12.718004+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.650247+x, 12.718004+y), Vector2(-4.038625+x, 13.759397+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.038625+x, 13.759397+y), Vector2(-2.914685+x, 14.967844+y)));
	segs.push_back(TriangleSegment2(Vector2(-2.914685+x, 14.967844+y), Vector2(-2.292475+x, 5.35237+y)));
	Scalar insetDist = 0.6;

	inset2scad(segs, "./test_cases/slicerTestCase/output/test_slice_34_loop_0.scad", 1, insetDist);


}

TriangleSegment2 elongate(const TriangleSegment2 &s, Scalar dist);
TriangleSegment2 prelongate(const TriangleSegment2 &s, Scalar dist);

void SlicerTestCase::testCollapse()
{
	Vector2 bisector0(-0.336, -0.942);
	Vector2 bisector1(-0.964, 0.266);
	Scalar elongation = 100;
	TriangleSegment2 segment(Vector2( 2.085, 7.968), Vector2( 2.708, -1.648));





	TriangleSegment2 bisectorSegment0;
	bisectorSegment0.a = segment.a;
	bisectorSegment0.b = segment.a + bisector0;

	TriangleSegment2 bisectorSegment1;
	bisectorSegment1.a = segment.b + bisector1;
	bisectorSegment1.b = segment.b;


	TriangleSegment2 s0 = elongate(bisectorSegment0, elongation);
	TriangleSegment2 s1 = prelongate(bisectorSegment1, elongation);
	Vector2 intersection;
	bool attached = segmentSegmentIntersection(s0, s1, intersection);

	cout << "-- testCollapse2 -- " << endl;
	cout << "segment = " << segment << endl;
	cout << "elongation =" << elongation << endl;

	cout << "bisector0 =" << bisector0 << endl;
	cout << "bisector1 =" << bisector1 << endl;
	cout << "s0 =" << s0 << endl;
	cout << "s1 =" << s1 << endl;
	cout << "Intersection = "<< intersection << endl << endl;

	Scalar tol = 0.0001;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(intersection[0],-0.98175, tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(intersection[1],-0.62987, tol );

}

void SlicerTestCase::test_slice_56_loop_3_short()
{
	std::vector<TriangleSegment2> segs;
	Scalar x = 0;
	Scalar y = 0;
	segs.push_back(TriangleSegment2(Vector2(1.85544+x, -3.009228+y), Vector2(5.835363+x, -11.889734+y)));
	segs.push_back(TriangleSegment2(Vector2(5.835363+x, -11.889734+y), Vector2(4.546847+x, -12.246619+y)));
	segs.push_back(TriangleSegment2(Vector2(4.546847+x, -12.246619+y), Vector2(3.360004+x, -12.607207+y)));
	segs.push_back(TriangleSegment2(Vector2(3.360004+x, -12.607207+y), Vector2(3.232559+x, -12.641848+y)));
	segs.push_back(TriangleSegment2(Vector2(3.232559+x, -12.641848+y), Vector2(3.086864+x, -12.687841+y)));
	segs.push_back(TriangleSegment2(Vector2(3.086864+x, -12.687841+y), Vector2(1.812529+x, -12.465019+y)));
	segs.push_back(TriangleSegment2(Vector2(1.812529+x, -12.465019+y), Vector2(0.716099+x, -12.321357+y)));
	segs.push_back(TriangleSegment2(Vector2(0.716099+x, -12.321357+y), Vector2(0.470475+x, -12.281855+y)));
	segs.push_back(TriangleSegment2(Vector2(0.470475+x, -12.281855+y), Vector2(0.209527+x, -12.257269+y)));
	segs.push_back(TriangleSegment2(Vector2(0.209527+x, -12.257269+y), Vector2(0.951433+x, -3.273723+y)));
	segs.push_back(TriangleSegment2(Vector2(0.951433+x, -3.273723+y), Vector2(1.85544+x, -3.009228+y)));

	Scalar insetDist = 0.35;
	inset2scad(segs, "./test_cases/slicerTestCase/output/test_slice_56_loop_3_short.scad", 1, insetDist);
}

void inshelligence( const SegmentTable & outlinesSegments,
					unsigned int nbOfShells,
					double layerW,
					unsigned int sliceId,
					Scalar insetDistanceFactor,
					bool writeDebugScadFiles,
					std::vector<SegmentTable> &insetsForLoops);

void SlicerTestCase::test_hollow_pyramid_1_loop_0()
{
	cout << endl;


	std::vector<TriangleSegment2> segs;

	Scalar x = 0;
	Scalar y = 0;
/*
	segs.push_back(TriangleSegment2(Vector2(18.992264+x, -18.992264+y), Vector2(18.876794+x, -18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(18.876794+x, -18.992264+y), Vector2(18.023204+x, -18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(18.023204+x, -18.992264+y), Vector2(-18.876794+x, -18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.876794+x, -18.992264+y), Vector2(-18.992264+x, -18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.992264+x, -18.992264+y), Vector2(-18.992264+x, -18.978505+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.992264+x, -18.978505+y), Vector2(-18.992264+x, -18.876794+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.992264+x, -18.876794+y), Vector2(-18.992264+x, -18.023204+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.992264+x, -18.023204+y), Vector2(-18.992264+x, 18.876794+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.992264+x, 18.876794+y), Vector2(-18.992264+x, 18.978505+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.992264+x, 18.978505+y), Vector2(-18.992264+x, 18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.992264+x, 18.992264+y), Vector2(-18.978505+x, 18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.978505+x, 18.992264+y), Vector2(-18.876794+x, 18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.876794+x, 18.992264+y), Vector2(-18.023204+x, 18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.023204+x, 18.992264+y), Vector2(18.876794+x, 18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(18.876794+x, 18.992264+y), Vector2(18.978505+x, 18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(18.978505+x, 18.992264+y), Vector2(18.992264+x, 18.992264+y)));
	segs.push_back(TriangleSegment2(Vector2(18.992264+x, 18.992264+y), Vector2(18.992264+x, 18.876794+y)));
	segs.push_back(TriangleSegment2(Vector2(18.992264+x, 18.876794+y), Vector2(18.992264+x, 18.023204+y)));
	segs.push_back(TriangleSegment2(Vector2(18.992264+x, 18.023204+y), Vector2(18.992264+x, -18.876794+y)));
	segs.push_back(TriangleSegment2(Vector2(18.992264+x, -18.876794+y), Vector2(18.992264+x, -18.992264+y)));
*/
	segs.push_back(TriangleSegment2(Vector2(18.807512+x, -18.807512+y), Vector2(18.322538+x, -18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(18.322538+x, -18.807512+y), Vector2(14.73746+x, -18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(14.73746+x, -18.807512+y), Vector2(-18.322538+x, -18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.322538+x, -18.807512+y), Vector2(-18.807512+x, -18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.807512+x, -18.807512+y), Vector2(-18.807512+x, -18.749724+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.807512+x, -18.749724+y), Vector2(-18.807512+x, -18.322538+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.807512+x, -18.322538+y), Vector2(-18.807512+x, -14.73746+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.807512+x, -14.73746+y), Vector2(-18.807512+x, 18.322538+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.807512+x, 18.322538+y), Vector2(-18.807512+x, 18.749724+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.807512+x, 18.749724+y), Vector2(-18.807512+x, 18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.807512+x, 18.807512+y), Vector2(-18.749724+x, 18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.749724+x, 18.807512+y), Vector2(-18.322538+x, 18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(-18.322538+x, 18.807512+y), Vector2(-14.73746+x, 18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(-14.73746+x, 18.807512+y), Vector2(18.322538+x, 18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(18.322538+x, 18.807512+y), Vector2(18.749724+x, 18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(18.749724+x, 18.807512+y), Vector2(18.807512+x, 18.807512+y)));
	segs.push_back(TriangleSegment2(Vector2(18.807512+x, 18.807512+y), Vector2(18.807512+x, 18.322538+y)));
	segs.push_back(TriangleSegment2(Vector2(18.807512+x, 18.322538+y), Vector2(18.807512+x, 14.73746+y)));
	segs.push_back(TriangleSegment2(Vector2(18.807512+x, 14.73746+y), Vector2(18.807512+x, -18.322538+y)));
	segs.push_back(TriangleSegment2(Vector2(18.807512+x, -18.322538+y), Vector2(18.807512+x, -18.807512+y)));

	Scalar layerW = 0.5;
	unsigned int nbOfShells = 3;

	inset2scad(segs,
			"./test_cases/slicerTestCase/output/test_hollow_pyramid_1_loop_0.scad",
			nbOfShells,
			layerW);

}

void SlicerTestCase::test_hollow_pyramid_90_loop_0()
{
	cout << endl;
	std::vector<TriangleSegment2> segs;
	Scalar x = 0;
	Scalar y = 0;
/* inset 0
	segs.push_back(TriangleSegment2(Vector2(-1.296481+x, 4.962655+y), Vector2(-1.296481+x, 4.611891+y)));
	segs.push_back(TriangleSegment2(Vector2(-1.296481+x, 4.611891+y), Vector2(-1.296481+x, 1.296481+y)));
	segs.push_back(TriangleSegment2(Vector2(-1.296481+x, 1.296481+y), Vector2(-4.611891+x, 1.296481+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.611891+x, 1.296481+y), Vector2(-4.962655+x, 1.296481+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.962655+x, 1.296481+y), Vector2(-4.962656+x, 1.60545+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.962656+x, 1.60545+y), Vector2(-4.962655+x, 4.962655+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.962655+x, 4.962655+y), Vector2(-1.60545+x, 4.962656+y)));
	segs.push_back(TriangleSegment2(Vector2(-1.60545+x, 4.962656+y), Vector2(-1.296481+x, 4.962655+y)));
*/
	segs.push_back(TriangleSegment2(Vector2(-1.446481+x, 4.812655+y), Vector2(-1.446481+x, 4.611891+y)));
	segs.push_back(TriangleSegment2(Vector2(-1.446481+x, 4.611891+y), Vector2(-1.446481+x, 1.446481+y)));
	segs.push_back(TriangleSegment2(Vector2(-1.446481+x, 1.446481+y), Vector2(-4.611891+x, 1.446481+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.611891+x, 1.446481+y), Vector2(-4.812655+x, 1.446481+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.812655+x, 1.446481+y), Vector2(-4.812656+x, 1.60545+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.812656+x, 1.60545+y), Vector2(-4.812655+x, 4.812655+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.812655+x, 4.812655+y), Vector2(-1.60545+x, 4.812656+y)));
	segs.push_back(TriangleSegment2(Vector2(-1.60545+x, 4.812656+y), Vector2(-1.446481+x, 4.812655+y)));

	Scalar layerW = 0.8;
	unsigned int nbOfShells = 3;

	inset2scad(segs,
			"./test_cases/slicerTestCase/output/test_hollow_pyramid_90_loop_0.scad",
			nbOfShells,
			layerW);
}

bool collinear(const TriangleSegment2 &a, const TriangleSegment2 &b, Scalar tol, Vector2 &mid );

void SlicerTestCase::test_collinear()
{
	TriangleSegment2 prev = TriangleSegment2(Vector2(-1.605, 4.654), Vector2(-1.605, 4.612));
	TriangleSegment2 current = TriangleSegment2(Vector2(-1.605, 4.612), Vector2(-1.605, 1.446));

	Scalar tol = 1e-6;
	Vector2 mid;
	bool test = collinear(prev, current, tol, mid);
	CPPUNIT_ASSERT(test);


}

void SlicerTestCase::test_hexagon_0_1()
{
	cout << endl;
	std::vector<TriangleSegment2> segs;
	Scalar x = 0;
	Scalar y = 0;

	segs.push_back(TriangleSegment2(Vector2(9.8+x, -5.658031+y), Vector2(9.53+x, -5.813916+y)));
	segs.push_back(TriangleSegment2(Vector2(9.53+x, -5.813916+y), Vector2(-0.0+x, -11.31607+y)));
	segs.push_back(TriangleSegment2(Vector2(-0.0+x, -11.31607+y), Vector2(-0.27+x, -11.160186+y)));
	segs.push_back(TriangleSegment2(Vector2(-0.27+x, -11.160186+y), Vector2(-9.8+x, -5.658031+y)));
	segs.push_back(TriangleSegment2(Vector2(-9.8+x, -5.658031+y), Vector2(-9.8+x, -5.346262+y)));
	segs.push_back(TriangleSegment2(Vector2(-9.8+x, -5.346262+y), Vector2(-9.8+x, 5.658034+y)));
	segs.push_back(TriangleSegment2(Vector2(-9.8+x, 5.658034+y), Vector2(-9.53+x, 5.813918+y)));
	segs.push_back(TriangleSegment2(Vector2(-9.53+x, 5.813918+y), Vector2(0.0+x, 11.31607+y)));
	segs.push_back(TriangleSegment2(Vector2(0.0+x, 11.31607+y), Vector2(0.27+x, 11.160186+y)));
	segs.push_back(TriangleSegment2(Vector2(0.27+x, 11.160186+y), Vector2(9.8+x, 5.658034+y)));
	segs.push_back(TriangleSegment2(Vector2(9.8+x, 5.658034+y), Vector2(9.8+x, 5.346265+y)));
	segs.push_back(TriangleSegment2(Vector2(9.8+x, 5.346265+y), Vector2(9.8+x, -5.658031+y)));

	Scalar layerW = 0.8;
	unsigned int nbOfShells = 3;

	inset2scad(segs,
			"./test_cases/slicerTestCase/output/test_hexagon_0_1.scad",
			nbOfShells,
			layerW);

}

void SlicerTestCase::test_knot_26_0_3()
{
	cout << endl;
	std::vector<TriangleSegment2> initialSegs;

	std::vector<TriangleSegment2>& segs = initialSegs;
	Scalar x = 0;
	Scalar y = 0;

	segs.push_back(TriangleSegment2(Vector2(5.450061+x, 15.545332+y), Vector2(5.025458+x, 14.247893+y)));
	segs.push_back(TriangleSegment2(Vector2(5.025458+x, 14.247893+y), Vector2(4.853937+x, 13.89291+y)));
	segs.push_back(TriangleSegment2(Vector2(4.853937+x, 13.89291+y), Vector2(4.658089+x, 13.454072+y)));
	segs.push_back(TriangleSegment2(Vector2(4.658089+x, 13.454072+y), Vector2(3.795509+x, 12.511628+y)));
	segs.push_back(TriangleSegment2(Vector2(3.795509+x, 12.511628+y), Vector2(3.577698+x, 12.262424+y)));
	segs.push_back(TriangleSegment2(Vector2(3.578258+x, 12.263054+y), Vector2(2.590469+x, 11.166202+y)));
	segs.push_back(TriangleSegment2(Vector2(2.590469+x, 11.166202+y), Vector2(2.267582+x, 10.77404+y)));
	segs.push_back(TriangleSegment2(Vector2(2.267582+x, 10.77404+y), Vector2(1.373256+x, 9.801078+y)));
	segs.push_back(TriangleSegment2(Vector2(1.373256+x, 9.801078+y), Vector2(1.040342+x, 9.387912+y)));
	segs.push_back(TriangleSegment2(Vector2(1.040342+x, 9.387912+y), Vector2(-0.07456+x, 8.250493+y)));
	segs.push_back(TriangleSegment2(Vector2(-0.07456+x, 8.250493+y), Vector2(-0.304133+x, 7.978325+y)));
	segs.push_back(TriangleSegment2(Vector2(-0.304133+x, 7.978325+y), Vector2(-1.25862+x, 7.102147+y)));
	segs.push_back(TriangleSegment2(Vector2(-1.25862+x, 7.102147+y), Vector2(-2.349039+x, 6.75854+y)));
	segs.push_back(TriangleSegment2(Vector2(-2.349039+x, 6.75854+y), Vector2(-4.325067+x, 6.501143+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.325067+x, 6.501143+y), Vector2(-4.804036+x, 7.082986+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.804036+x, 7.082986+y), Vector2(-6.049232+x, 8.189452+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.049232+x, 8.189452+y), Vector2(-6.056864+x, 8.335983+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.056864+x, 8.335983+y), Vector2(-6.091451+x, 8.516329+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.091451+x, 8.516329+y), Vector2(-6.041699+x, 9.953586+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.041699+x, 9.953586+y), Vector2(-6.204598+x, 11.476621+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.204598+x, 11.476621+y), Vector2(-6.197295+x, 11.511933+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.197295+x, 11.511933+y), Vector2(-6.1935+x, 11.563446+y)));
	segs.push_back(TriangleSegment2(Vector2(-6.1935+x, 11.563446+y), Vector2(-5.398063+x, 13.175654+y)));
	segs.push_back(TriangleSegment2(Vector2(-5.398063+x, 13.175654+y), Vector2(-5.167049+x, 13.764749+y)));
	segs.push_back(TriangleSegment2(Vector2(-5.167049+x, 13.764749+y), Vector2(-4.336053+x, 14.917946+y)));
	segs.push_back(TriangleSegment2(Vector2(-4.336053+x, 14.917946+y), Vector2(-3.693982+x, 15.916335+y)));
	segs.push_back(TriangleSegment2(Vector2(-3.693982+x, 15.916335+y), Vector2(-2.646139+x, 16.835044+y)));
	segs.push_back(TriangleSegment2(Vector2(-2.646139+x, 16.835044+y), Vector2(-1.57206+x, 17.821738+y)));
	segs.push_back(TriangleSegment2(Vector2(-1.57206+x, 17.821738+y), Vector2(1.456402+x, 19.364116+y)));
	segs.push_back(TriangleSegment2(Vector2(1.456402+x, 19.364116+y), Vector2(4.645849+x, 18.766898+y)));
	segs.push_back(TriangleSegment2(Vector2(4.645849+x, 18.766898+y), Vector2(5.440787+x, 17.39162+y)));
	segs.push_back(TriangleSegment2(Vector2(5.441104+x, 17.391079+y), Vector2(5.845535+x, 16.708904+y)));
	segs.push_back(TriangleSegment2(Vector2(5.845535+x, 16.708904+y), Vector2(5.450265+x, 15.545944+y)));

	std::vector<TriangleSegment2> clipped;
	std::vector<TriangleSegment2> clipb;


	clip(initialSegs, 1, 13, clipb);
	clip(clipb, 6, 16, clipped);

//    segments =[
//       [[5.450061, 15.545332, 0.000000],[ 5.025458, 14.247893, 0.000000]],
//       [[5.025458, 14.247893, 0.050000],[ -4.804036, 7.082986, 0.050000]],
//       [[-4.804036, 7.082986, 0.100000],[ -6.049232, 8.189452, 0.100000]],
//       [[-6.049232, 8.189452, 0.150000],[ -6.056864, 8.335983, 0.150000]],
//       [[-6.056864, 8.335983, 0.200000],[ -6.091451, 8.516329, 0.200000]],
//       [[-6.091451, 8.516329, 0.250000],[ -6.041699, 9.953586, 0.250000]],
//       [[-6.041699, 9.953586, 0.300000],[ 5.440787, 17.391620, 0.300000]],
//       [[5.441104, 17.391079, 0.350000],[ 5.845535, 16.708904, 0.350000]],
//       [[5.845535, 16.708904, 0.400000],[ 5.450265, 15.545944, 0.400000]],
//       ];


	segs = clipped;
	Scalar layerW =0.4; // 0.8399;
	unsigned int nbOfShells = 3;
	inset2scad(segs,
			"./test_cases/slicerTestCase/output/test_knot_26_0_3.scad",
			nbOfShells,
			layerW);
}


void SlicerTestCase::test_3d_knot_57_3()
{
	cout << endl;

	std::vector<TriangleSegment2> segs;
	Scalar x = 0;
	Scalar y = 0;

	segs.push_back(TriangleSegment2(Vector2(2.211841, -3.693989), Vector2(3.123649, -3.832589)));
	segs.push_back(TriangleSegment2(Vector2(3.123649, -3.832589), Vector2(4.970302, -4.147696)));
	segs.push_back(TriangleSegment2(Vector2(4.970302, -4.147696), Vector2(6.165003, -5.193736)));
	segs.push_back(TriangleSegment2(Vector2(6.165003, -5.193736), Vector2(6.716749, -5.685612)));
	segs.push_back(TriangleSegment2(Vector2(6.716749, -5.685612), Vector2(6.742006, -5.807134)));
	segs.push_back(TriangleSegment2(Vector2(6.742006, -5.807134), Vector2(7.06856, -7.302008)));
	segs.push_back(TriangleSegment2(Vector2(7.06856, -7.302008), Vector2(7.187812, -7.864612)));
	segs.push_back(TriangleSegment2(Vector2(7.187812, -7.864612), Vector2(6.681044, -9.104204)));
	segs.push_back(TriangleSegment2(Vector2(6.681044, -9.104204), Vector2(6.28063, -10.155697)));
	segs.push_back(TriangleSegment2(Vector2(6.28063, -10.155697), Vector2(6.142817, -10.271333)));
	segs.push_back(TriangleSegment2(Vector2(6.142817, -10.271333), Vector2(5.930256, -10.458724)));
	segs.push_back(TriangleSegment2(Vector2(5.930256, -10.458724), Vector2(5.019379, -11.204738)));
	segs.push_back(TriangleSegment2(Vector2(5.019379, -11.204738), Vector2(4.187195, -11.943557)));
	segs.push_back(TriangleSegment2(Vector2(4.187195, -11.943557), Vector2(4.115148, -11.96314)));
	segs.push_back(TriangleSegment2(Vector2(4.115148, -11.96314), Vector2(3.973218, -12.007945)));
	segs.push_back(TriangleSegment2(Vector2(3.973218, -12.007945), Vector2(2.743631, -12.337872)));
	segs.push_back(TriangleSegment2(Vector2(2.743631, -12.337872), Vector2(1.613496, -12.715803)));
	segs.push_back(TriangleSegment2(Vector2(1.613496, -12.715803), Vector2(1.475416, -12.752556)));
	segs.push_back(TriangleSegment2(Vector2(1.475416, -12.752556), Vector2(1.428868, -12.769167)));
	segs.push_back(TriangleSegment2(Vector2(1.428868, -12.769167), Vector2(0.326762, -12.614427)));
	segs.push_back(TriangleSegment2(Vector2(0.326762, -12.614427), Vector2(-0.531772, -12.572738)));
	segs.push_back(TriangleSegment2(Vector2(-0.531772, -12.572738), Vector2(-0.963905, -12.523192)));
	segs.push_back(TriangleSegment2(Vector2(-0.963905, -12.523192), Vector2(-1.344187, -12.522663)));
	segs.push_back(TriangleSegment2(Vector2(-1.344187, -12.522663), Vector2(-1.914821, -12.149995)));
	segs.push_back(TriangleSegment2(Vector2(-1.914821, -12.149995), Vector2(-2.485054, -11.876971)));
	segs.push_back(TriangleSegment2(Vector2(-2.485054, -11.876971), Vector2(-3.074281, -11.548641)));
	segs.push_back(TriangleSegment2(Vector2(-3.074281, -11.548641), Vector2(-3.559397, -11.374582)));
	segs.push_back(TriangleSegment2(Vector2(-3.559397, -11.374582), Vector2(-3.74223, -10.978375)));
	segs.push_back(TriangleSegment2(Vector2(-3.74223, -10.978375), Vector2(-4.174335, -10.332966)));
	segs.push_back(TriangleSegment2(Vector2(-4.174335, -10.332966), Vector2(-4.559665, -9.703962)));
	segs.push_back(TriangleSegment2(Vector2(-4.559665, -9.703962), Vector2(-4.688644, -9.572259)));
	segs.push_back(TriangleSegment2(Vector2(-4.688644, -9.572259), Vector2(-4.414544, -8.600922)));
	segs.push_back(TriangleSegment2(Vector2(-4.414544, -8.600922), Vector2(-3.938897, -7.123844)));
	segs.push_back(TriangleSegment2(Vector2(-3.938897, -7.123844), Vector2(-3.586152, -6.813164)));
	segs.push_back(TriangleSegment2(Vector2(-3.586152, -6.813164), Vector2(-3.251632, -6.546699)));
	segs.push_back(TriangleSegment2(Vector2(-3.251632, -6.546699), Vector2(-2.2504, -5.663855)));
	segs.push_back(TriangleSegment2(Vector2(-2.2504, -5.663855), Vector2(-1.043943, -4.674327)));
	segs.push_back(TriangleSegment2(Vector2(-1.043943, -4.674327), Vector2(-0.904424, -4.628681)));
	segs.push_back(TriangleSegment2(Vector2(-0.904424, -4.628681), Vector2(-0.613999, -4.544302)));
	segs.push_back(TriangleSegment2(Vector2(-0.613999, -4.544302), Vector2(1.038997, -4.031817)));
	segs.push_back(TriangleSegment2(Vector2(1.038997, -4.031817), Vector2(2.211841, -3.693989)));

	std::vector<TriangleSegment2> clipped;
	std::vector<TriangleSegment2> clipb;

	clip(segs, 1, 2, clipb);
/*
 	segs = &clipb;

	clip(clipb, 6, 16, clipped);
	segs =& clipped;
*/
	Scalar layerW = 0.27;
	unsigned int nbOfShells = 8;
	inset2scad(clipb,
			"./test_cases/slicerTestCase/output/test_3d_knot_57_3.scad",
			nbOfShells,
			layerW);
}
