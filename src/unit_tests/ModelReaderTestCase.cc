
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
#include "mgl/slicy.h"



CPPUNIT_TEST_SUITE_REGISTRATION( ModelReaderTestCase );

using namespace std;
using namespace BGL;
using namespace mgl;


/*
CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
CPPUNIT_ASSERT_EQUAL( 12, 12 );
CPPUNIT_ASSERT( 12L == 12L );
*/


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

//	translateSegments(segments, toOrigin);
//	rotateSegments(segments, angle);
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
//		rotateSegments(segments, -angle);
//		translateSegments(segments, toCenter);

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



void sliceToScad(const char*modelFile, const char* outDir, const char* stlFilePrefix, const char* scadFile)
{
	Meshy mesh(0.35);
	double tubeSpacing = 1.0;

	LoadMeshyFromStl(mesh, modelFile);
	// mesh.dump(cout);

	cout << "Splitting up: " << modelFile << endl;
	cout << "And creating: " << scadFile << endl;

	const std::vector<Triangle3d> &allTriangles = mesh.readAllTriangles();
	const TrianglesInSlices &sliceTable = mesh.readSliceTable();
	const Limits& limits = mesh.readLimits();
	std::cout << "LIMITS: " << limits << std::endl;

	Limits tubularLimits = limits;
	tubularLimits.inflate(1.0, 1.0, 0.0);
	tubularLimits.tubularZ();


	stringstream outScadName;
	ScadTubeFile outlineScad(scadFile);

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
		stlName << outDir  <<  "/" << stlFilePrefix << i << ".stl";
		mesh.writeStlFileForLayer(i, stlName.str().c_str());

		outlineScad.writeTubesModule("out_", outlineSegments, i, z);
		outlineScad.writeStlModule("stl_", stlFilePrefix,  i);

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

}

void ModelReaderTestCase::test3dKnot()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	std::string outDir = "test_cases/modelReaderTestCase/output";

	std::vector<std::string> models;

	std::string stlDirectory = "inputs";
	models.push_back("3D_Knot");
	models.push_back("Water");
	models.push_back("hexagon");
	models.push_back("Land");



/*
	std::string stlDirectory = "../stls";
	//std::string stlDirectory = "/home/hugo/code/stls";

	models.push_back("3D_Knot");
	models.push_back("F1");
	models.push_back("hexagon");
	models.push_back("Land");
	models.push_back("Roal10");
	models.push_back("soap_highres");
	models.push_back("TeaPot");
	models.push_back("Water");
	models.push_back("Yodsta_Printdata");
	//models.push_back("Pivot-Joint_-_Ball_End_-1X");
	//models.push_back("Toymaker_Skull_1_Million_Polys");

	models.push_back("part2");
*/
	std::vector<double> times;
	for (int i=0; i < models.size(); i++)
	{

		std::string modelFile;
		modelFile += stlDirectory;
		modelFile += "/";
		modelFile += models[i];
		modelFile += ".stl";

		std::string stlFiles;
		stlFiles += models[i];
		stlFiles += "_";

		std::string scadFile;
		scadFile += models[i];
		scadFile += ".scad";

		cout << endl << endl;
		cout << modelFile << " to " << stlFiles << " and " << scadFile << endl;

		unsigned int t0,t1;
		t0=clock();
		sliceToScad(modelFile.c_str(), outDir.c_str(), stlFiles.c_str(), scadFile.c_str());
		t1=clock()-t0;
		double t = t1 / 1000000.0;
		times.push_back(t);
		cout << "In only " << t << "seconds" << endl;
	}

	cout << endl << endl << "MODEL *** TIME 2 SLICE (s)" << endl;
	cout << " ----------" <<endl;
	for (int i=0; i < models.size(); i++)
	{
		cout << models[i] << "\t" << times[i] << endl;
	}


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


