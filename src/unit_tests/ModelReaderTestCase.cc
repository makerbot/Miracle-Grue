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
	cout << endl;
	cout << endl;
	cout << __FUNCTION__ << endl;
	Scalar zH = 1.0;
	Meshy mesh(zH, zH);

	cout << "ceil(40.0)="<< ceil(40.0)<<endl;

	Triangle3d t;
	t.vertex1 =Point3d(0,10,0);
	t.vertex2 =Point3d(0,10,3.4);
	t.vertex3 =Point3d(0,10,1);

	cout << endl << endl;
	cout << "t " << t.vertex1 << ", " << t.vertex2 << ", " << t.vertex3 << endl;
 	mesh.addTriangle(t);

 	cout << "mesh.dump(cout);" << endl;
 	mesh.dump(cout);


    // 3 layers in this cake!
 	CPPUNIT_ASSERT_EQUAL((size_t)3, mesh.readSliceTable().size());

 	// 1 triangle for each layer
 	const TriangleIndices &slice0 =  mesh.readSliceTable()[0];
 	CPPUNIT_ASSERT_EQUAL((size_t)1, slice0.size());

    const TriangleIndices &slice1 =  mesh.readSliceTable()[1];
    CPPUNIT_ASSERT_EQUAL((size_t)1, slice1.size());

    const TriangleIndices &slice2 =  mesh.readSliceTable()[2];
    CPPUNIT_ASSERT_EQUAL((size_t)1, slice2.size());


	t.vertex1 =Point3d(0,10, 0);
	t.vertex2 =Point3d(0,10, 3.6);
	t.vertex3 =Point3d(0,10, 1);

	mesh.addTriangle(t);
	CPPUNIT_ASSERT_EQUAL((size_t)3, mesh.readSliceTable().size());
	//cout << "#$%^%" << endl;
	const Limits &limits = mesh.readLimits();
	double tol = 0.00001;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0,  limits.xMin, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(10, limits.yMin, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0,  limits.zMin, tol);
	//cout << "#$%^%" << endl;

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0,   limits.xMax, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(10,  limits.yMax, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.6, limits.zMax, tol);
}

void ModelReaderTestCase::testLayerSplit()
{
	cout << endl;

	Meshy mesh(0.35, 0.35);
	unsigned int t0, t1;
	t0 = clock();

	loadMeshyFromStl(mesh, "inputs/Water.stl");
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
	Meshy mesh3(0.35, 0.35);
	t0=clock();
	loadMeshyFromStl(mesh3, "inputs/lightsaber.stl");
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
	Meshy mesh(0.35, 0.35);
	t0=clock();
	loadMeshyFromStl(mesh, "inputs/Water.stl");
	t1=clock()-t0;
	mesh.dump(cout);
	cout << "time: " << t1 << endl;
	CPPUNIT_ASSERT_EQUAL((size_t)172, mesh.readSliceTable().size());

	cout << "Land" << endl;
	Meshy mesh2(0.35, 0.35);

	t0=clock();
	loadMeshyFromStl(mesh2, "inputs/Land.stl");
	t1=clock()-t0;
	cout << "time: " << t1 << endl;
	CPPUNIT_ASSERT_EQUAL((size_t)174, mesh2.readSliceTable().size());
	mesh2.dump(cout);
}

void ModelReaderTestCase::testSlicyWater()
{
	Meshy mesh(0.35, 0.35);
	loadMeshyFromStl(mesh, "inputs/Water.stl");

	const SliceTable& table = mesh.readSliceTable();

	unsigned int t0,t1;
	t0=clock();

	const vector<Triangle3d>& allTriangles = mesh.readAllTriangles();
	int sliceIndex = 0;
	for (SliceTable::const_iterator i = table.begin(); i != table.end(); i++)
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




BGL::Point rotateAroundPoint(const BGL::Point &center, Scalar angle, const BGL::Point &p)
{
	// translate point back to origin:
	BGL::Point translated = p - center;

	BGL::Point rotated = rotate2d(translated, angle);
	// translate point back:
	BGL::Point r = rotated + center;
	return r;
}

// openscad debugging
string visibleCut(const Triangle3& t, const Vector3d &a, const Vector3d &b)
{
	stringstream out;

	out << "polyhedron ( points   = [  [";
	out << t[0].x << ", " << t[0].y << ", " << t[0].z << "], [";
	out << t[1].x << ", " << t[1].y << ", " << t[1].z << "], [";
	out << t[2].x << ", " << t[2].y << ", " << t[2].z << "] ], ";
	out << "triangles = [  [0, 1, 2] ]); " << endl;
    out << "tube(";
    out << a.x << ", " << a.y << ", " << a.z << ", ";
    out << b.x << ", " << b.y << ", " << b.z << ", ";
    out << " diameter=1, faces=4, thickness_over_width=1";
    out << ");";

    return out.str();
}

void ModelReaderTestCase::testCutTriangle()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	Vector3d v0(-1, 0, 0);
	Vector3d v1(1, 0, 0);
	Vector3d v2(0,0,1);

	Triangle3 t(v0,v1,v2);

	Vector3d a,b;
	bool cut;
	Scalar z = 0.5;
	cut = t.cut( z, a, b);
	cout << "Cutting at z="<< z<< ": a="<<a << " b=" << b << endl;

	CPPUNIT_ASSERT(cut);
	CPPUNIT_ASSERT_DOUBLES_EQUAL( -0.5, b.x, 0.00001 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0, b.y, 0.00001 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.5, a.x, 0.00001 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.0, a.y, 0.00001 );

//	  facet normal 2.621327e-01 7.901105e-01 -5.540864e-01
//	    outer loop
//	      vertex -1.556260e+01, 5.680465e+00, 2.200485e+01
//	      vertex -1.832293e+01, 4.436024e+00, 1.892443e+01
//	      vertex -1.800681e+01, 6.473042e+00, 2.197871e+01
//	    endloop
//	  endfacet

	Vector3d v0b(-1.556260e+01, 5.680465e+00, 2.200485e+01);
	Vector3d v1b(-1.832293e+01, 4.436024e+00, 1.892443e+01 );
	Vector3d v2b( -1.800681e+01, 6.473042e+00, 2.197871e+01);
	Triangle3 tb(v0b,v1b,v2b);



	cout << "triangle B" << endl;
	cout << tb[0] << endl;
	cout << tb[1] << endl;
	cout << tb[2] << endl;
	z = 20.5;

	cut = tb.cut( z, a, b);
	cout << "Cutting at z="<< z<< ": a="<<a << " b=" << b << endl;
	cout << endl << "VISICUT: " << endl << visibleCut(tb, a, b) << endl<<endl;
	CPPUNIT_ASSERT(cut);


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


void batchProcess(	Scalar firstLayerZ,
					Scalar layerH,
					Scalar layerW,
					Scalar tubeSpacing,
					const char* outDir,
					const std::vector<std::string> &models)
{
	FileSystemAbstractor fileSystem;
	std::vector<double> times;
	for (int i=0; i < models.size(); i++)
	{

		std::string modelFile = models[i];
		std::string stlFiles = fileSystem.removeExtension(fileSystem.ExtractFilename(models[i]));
		stlFiles += "_";

		std::string scadFile = outDir;
		scadFile += "/";
		scadFile += fileSystem.ChangeExtension(fileSystem.ExtractFilename(models[i]), ".scad" )  ;


		cout << modelFile << " to " << scadFile << endl;

		unsigned int t0,t1;
		t0=clock();

		Scalar angle = M_PI*0.5;
		Meshy mesh(firstLayerZ, layerH);
		loadMeshyFromStl(mesh, modelFile.c_str());
		cout << modelFile << " LOADED" << endl;
		std::vector< TubesInSlice > allTubes;
		sliceAndPath( mesh,
					layerW,
					tubeSpacing,
					angle,
					scadFile.c_str(),
					allTubes);


		t1=clock()-t0;
		double t = t1 / 1000000.0;
		times.push_back(t);
		cout << "Sliced in " << t << " seconds" << endl;
	}

	cout << endl << endl << "MODEL    TIME 2 SLICE (s)" << endl;
	cout << " ----------" <<endl;
	for (int i=0; i < models.size(); i++)
	{
		cout << models[i] << "\t" << times[i] << endl;
	}
}

void ModelReaderTestCase::testMyStls()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;

	Scalar firstLayerZ = 0.11;
	Scalar layerH = 0.35;
	Scalar layerW = 0.5833333;
	Scalar tubeSpacing = 1.0;

	std::string outDir = "test_cases/modelReaderTestCase/output";
	std::vector<std::string> models;

	models.push_back("inputs/3D_Knot.stl");
	models.push_back("inputs/Water.stl");
	models.push_back("inputs/hexagon.stl");
	models.push_back("inputs/Land.stl");
	models.push_back("../stls/monitor_simple.stl");
	models.push_back("../stls/F1.stl");
	models.push_back("../stls/hexagon.stl");
	models.push_back("../stls/Land.stl");
	models.push_back("../stls/Roal10.stl");
	models.push_back("../stls/soap_highres.stl");
	models.push_back("../stls/TeaPot.stl");
	models.push_back("../stls/Water.stl");
	models.push_back("../stls/Yodsta_Printdata.stl");

	//models.push_back("Pivot-Joint_-_Ball_End_-1X");
	//models.push_back("Toymaker_Skull_1_Million_Polys");
	//	models.push_back("part2");

	batchProcess(firstLayerZ, layerH, layerW, tubeSpacing, outDir.c_str(), models);
}

void ModelReaderTestCase::testKnot()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;
	Scalar firstZ = 0.11;
	Scalar layerH = 0.35;
	Scalar layerW = 0.5833333;
	Scalar tubeSpacing = 1.0;

	std::string outDir = "test_cases/modelReaderTestCase/output";
	std::vector<std::string> models;
	models.push_back("inputs/3D_Knot.stl");
	batchProcess(firstZ, layerH, layerW, tubeSpacing, outDir.c_str(), models);
}

void ModelReaderTestCase::testInputStls()
{
	BOOST_LOG_TRIVIAL(trace) << endl << "Starting: " <<__FUNCTION__ << endl;
	Scalar firstZ = 0.11;
	Scalar layerH = 0.35;
	Scalar layerW = 0.5833333;
	Scalar tubeSpacing = 1.0;

	std::string outDir = "test_cases/modelReaderTestCase/output";
	std::vector<std::string> models;

	models.push_back("inputs/3D_Knot.stl");
	models.push_back("inputs/Water.stl");
	models.push_back("inputs/hexagon.stl");
	models.push_back("inputs/Land.stl");

	batchProcess(firstZ, layerH, layerW, tubeSpacing, outDir.c_str(), models);

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


void ModelReaderTestCase::fixContourProblem()
{
	cout << endl;

	Scalar tol = 1e-6;

	Scalar firstZ =0.11;
	double layerH = 0.35;
	int layerIndex = 30;

	LayerMeasure zTapeMeasure(firstZ, layerH);

	Meshy mesh(firstZ, layerH); // 0.35
	cout << "LOADING... " << endl;
	loadMeshyFromStl(mesh, "inputs/3D_Knot.stl");

	Scalar z = zTapeMeasure.sliceIndexToHeight(30);

	const std::vector<Triangle3d> &allTriangles = mesh.readAllTriangles();
	const SliceTable &sliceTable = mesh.readSliceTable();
	const TriangleIndices &trianglesForSlice = sliceTable[30];
	std::vector< std::vector<Segment> > outlineSegments;
	// get 2D paths for outline
	segmentology(allTriangles, trianglesForSlice, z, tol, outlineSegments);

	std::vector<std::pair<double,double> > minsAndMaxes;
	size_t triangleCount = trianglesForSlice.size();
	minsAndMaxes.reserve(triangleCount);
	cout << "triangles for layer "<< layerIndex <<", z=" << z<< endl;

	for (int i=0; i < triangleCount; i++)
	{
		index_t idx = trianglesForSlice[i];
		const Triangle3d &t = allTriangles[idx];

		double min, max;
		minMaxZ(t,min, max);
		minsAndMaxes.push_back(std::pair<Scalar, Scalar>(min, max) );
		cout << t.vertex1.z << "\t\t" << t.vertex2.z << "\t\t" << t.vertex3.z << "\t\t" << min << "\t"<< max << endl;
		cout << "  min " << min << " max " << endl;
		CPPUNIT_ASSERT(z >= min);
//		CPPUNIT_ASSERT(z <= max);
	}

	for (int i=0; i < triangleCount; i++)
	{
		cout << i  << " ";
		CPPUNIT_ASSERT(z >= minsAndMaxes[i].first);
//		CPPUNIT_ASSERT(z <= minsAndMaxes[i].second);
	}
	cout << endl;
}


void ModelReaderTestCase::testLayerMeasure()
{
	cout << endl;
	double tol = 0.000000001;
	LayerMeasure n(0.11, 0.35 ); // n is for nozzle

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.11, n.sliceIndexToHeight(0), tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.46, n.sliceIndexToHeight(1), tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.81, n.sliceIndexToHeight(2), tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.16, n.sliceIndexToHeight(3), tol);

	Scalar z = 0;
	unsigned int abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 0, abode);

	z = 0.1;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 0, abode);

	z = 0.11;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 1, abode);

	z = 0.12;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 1, abode);

	z = 0.45;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 1, abode);

	z = 0.46;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 2, abode);

	z = 0.47;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 2, abode);

	z = 1.15999;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 3, abode);

	z = 1.1600001;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 4, abode);

	// one million! mouhahahahahaahahahaaha :-)
	z = 350000.11;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 1000001, abode);
}




/*

void dump(const list<index_t> &edges)
{
	for(list<index_t>::const_iterator i=edges.begin(); i != edges.end(); i++)
	{
		cout << "  " <<  *i << endl;
	}
}

void ModelReaderTestCase::testSlicyKnot()
{
	string modelFile = "inputs/3D_Knot.stl";

    Configuration config;
    config["slicer"]["firstLayerZ"] = 0.11;
	config["slicer"]["layerH"] = 0.35;
	Meshy mesh(config["slicer"]["firstLayerZ"].asDouble(), config["slicer"]["layerH"].asDouble()); // 0.35
	LoadMeshyFromStl(mesh, modelFile.c_str());

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
	//dump(edges);

}
*/
