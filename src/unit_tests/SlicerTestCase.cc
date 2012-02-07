#include <list>

#include <cppunit/config/SourcePrefix.h>
#include "SlicerTestCase.h"

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
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset3.scad", 0.25);

	Scalar insetDist = 1;
	unsigned int shells = 6;

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
	Shrinky shrinky("./test_cases/slicerTestCase/output/testInset4.scad", 1);
	shrinky.dz = 0.05;
	Scalar insetDist = 1;
	unsigned int shells = 1;

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
	Shrinky shrinky("./test_cases/slicerTestCase/output/hexagon.scad", 1);
	shrinky.dz = 0.05;
	Scalar insetDist = 1;
	unsigned int shells = 1;

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

void SlicerTestCase::testSliceTriangle() {
	cout << endl << "Testing 'sliceTriangle'..." << endl;

	// this should really be passed to the sliceTriangle function
	double tol = 1e-6;
	Vector3 v1, v2, v3;
	Scalar Z = 0;
	Vector3 a, b;
	bool result;

	cout << endl << "\t testing above" << endl;
	v1 = Vector3(1, 2, 3);
	v2 = Vector3(2, 3, 4);
	v3 = Vector3(3, 4, 5);
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == false);

	cout << endl << "\t testing below" << endl;
	Z = 8;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == false);

	cout << endl << "\t testing == Z" << endl;
	CPPUNIT_ASSERT(a.z == b.z);
	CPPUNIT_ASSERT(a.z == Z);

	cout << endl << "\t testing flat face" << endl;
	v1 = Vector3(1, 2, 5);
	v2 = Vector3(2, 3, 5);
	v3 = Vector3(3, 4, 5);
	Z = 5;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == false);

	// this should be based on a tolerance. can we pass tolerance to sliceTriangle?
	cout << endl << "\t testing ~flat face" << endl;
	v1 = Vector3(1, 2, 4.999999999);
	v2 = Vector3(2, 3, 5.000000000);
	v3 = Vector3(3, 4, 5.000000001);
	Z = 5;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == false);

	cout << endl << "\t testing tiny triangle" << endl;
	v1 = Vector3(0.0001, 0.0002, 0.0001);
	v2 = Vector3(0.0005, 0.0004, 0.0002);
	v3 = Vector3(0.0003, 0.0001, 0.0003);
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

	Z = 0.00015;
	result = sliceTriangle(v1, v2, v3, Z, a, b);
	CPPUNIT_ASSERT(result == true);
	// pre-calculated answer to compare against
	//a = (.0003, .0003, .00015) b = (.00015, .000225, .00015)
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.x, .0003, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.y, .0003, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, Z, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.x, .00015, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.y, .000225, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b.z, Z, tol);

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

	cout << endl << "\t testing GIANT triangle" << endl;
	cout << "This test has not been implemented!";
	cout << endl << "\t testing order" << endl;
	Vector3 order1[2], order2[2], order3[2];

	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, order1[0], order1[1]) == true);
	CPPUNIT_ASSERT(sliceTriangle(v1, v3, v2, Z, order2[0], order2[1]) == true);
	CPPUNIT_ASSERT(sliceTriangle(v2, v1, v3, Z, order3[0], order3[1]) == true);
	cout << "o1" << order1[0] << ", " << order1[1] << endl;
	cout << "o2" << order2[0] << ", " << order2[1] << endl;
	cout << "o3" << order3[0] << ", " << order3[1] << endl;

	cout << "This test has not been completely implemented!";

	cout << endl << "\t testing corners" << endl;
	v1 = Vector3(1, 2, 1);
	v2 = Vector3(5, 4, 2);
	v3 = Vector3(3, 1, 3);
	Z = 1;
	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, a, b) == false);
	Z = 3;
	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, a, b) == false);
	Z = 5;
	v2.z = 5
	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, a, b) == false);

	cout << endl << "\t testing length" << endl;
	v1 = Vector3(1, 2, 1);
	v2 = Vector3(5, 4, 2);
	v3 = Vector3(5, 4, 2);
	Z = 2;
	CPPUNIT_ASSERT(sliceTriangle(v1, v2, v3, Z, a, b) == false);

	// if a triangle was divided in two along a line
	// only one triangle should return a slice
	//
	cout << endl << "\t testing ???" << endl;
	v1 = Vector3(1, 2, 1);
	v2 = Vector3(5, 4, 2);
	v3 = Vector3(3, 1, 3);
	Z = 2;

	cout << endl << "Finished testing 'sliceTriangle'" << endl;
}


