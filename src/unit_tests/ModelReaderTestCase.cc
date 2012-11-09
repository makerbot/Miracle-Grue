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

#include <sys/stat.h>
#include <bits/basic_string.h>

#include "UnitTestUtils.h"

#include "mgl/configuration.h"
#include "mgl/slicy.h"
#include "mgl/configuration.h"
#include "mgl/gcoder.h"
#include "mgl/segmenter.h"

CPPUNIT_TEST_SUITE_REGISTRATION( ModelReaderTestCase );

using namespace std;
using namespace mgl;

string outputsDir("outputs/test_cases/modelReaderTestCase/");
string inputsDir("test_cases/modelReaderTestCase/");

/*
CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
CPPUNIT_ASSERT_EQUAL( 12, 12 );
CPPUNIT_ASSERT( 12L == 12L );
*/

void ModelReaderTestCase::setUp()
{
	MyComputer computer;
	
	char pathsep = computer.fileSystem.getPathSeparatorCharacter();
	
	outputsDir = string("outputs") + pathsep + string("test_cases") + pathsep + 
			string("modelReaderTestCase") + pathsep;
	inputsDir = string("test_cases") + pathsep + "modelReaderTestCase" + pathsep;
	
	computer.fileSystem.guarenteeDirectoryExistsRecursive(outputsDir.c_str());
	computer.fileSystem.guarenteeDirectoryExistsRecursive(inputsDir.c_str());
}


//
// Adds 2 triangles with a common edge
// to a Slicy
//
//void ModelReaderTestCase::testSlicySimple()
//{
//
//	Vector3 p0(0,0,0);
//	Vector3 p1(0,1,0);
//	Vector3 p2(1,1,0);
//	Vector3 p3(1,0,0);
//
//	Triangle3 t0(p0, p1, p2);
//	Triangle3 t1(p0, p2, p3);
//
//	Connexity sy(0.001);
//
//	cout << endl << endl;
//	cout << "******** slicy with 2 triangles *******" << endl;
//	cout << "add t0" << endl;
//	size_t face0 = sy.addTriangle(t0);
//
//	cout << "add t1" << endl;
//	size_t face1 = sy.addTriangle(t1);
//
//	cout << "xx"<< endl;
//
//
//
//	int a,b,c;
//	sy.lookupIncidentFacesToFace(face0, a,b,c);
//
//	cout << "xx"<< endl;
//	CPPUNIT_ASSERT(a==face1 || b==face1 || c== face1);
//
//	cout << "xxx"<< endl;
//	sy.dump(cout);
////	cout << sy << endl;
//}

//
// This test loads 1 triangle and verifies the number of slices
//
void ModelReaderTestCase::testMeshySimple()
{
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	cout << endl;
	cout << endl;
	cout << __FUNCTION__ << endl;
	//Scalar zH = 1.0;
	Meshy mesh(grueCfg);
//	Segmenter seg(zH, zH);
    
    Segmenter seg(grueCfg);

	cout << "ceil(40.0)="<< ceil(40.0)<<endl;

	Triangle3Type  t(Point3Type(0,10,0), Point3Type(0,10,3.4), Point3Type(0,10,1));

	cout << endl << endl;
	cout << "t " << t[0] << ", " << t[1] << ", " << t[2] << endl;
 	mesh.addTriangle(t);

 	cout << "mesh.dump(cout);" << endl;
 	mesh.dump(cout);

	seg.tablaturize(mesh);
    // 3 layers in this cake!
 	CPPUNIT_ASSERT_EQUAL((size_t)3, seg.readSliceTable().size());

 	// 1 triangle for each layer
 	const TriangleIndices &slice0 =  seg.readSliceTable()[0];
 	CPPUNIT_ASSERT_EQUAL((size_t)1, slice0.size());

    const TriangleIndices &slice1 =  seg.readSliceTable()[1];
    CPPUNIT_ASSERT_EQUAL((size_t)1, slice1.size());

    const TriangleIndices &slice2 =  seg.readSliceTable()[2];
    CPPUNIT_ASSERT_EQUAL((size_t)1, slice2.size());


    t[0] =Point3Type(0,10, 0);
    t[1] =Point3Type(0,10, 3.6);
    t[2] =Point3Type(0,10, 1);

	mesh.addTriangle(t);
	CPPUNIT_ASSERT_EQUAL((size_t)3, seg.readSliceTable().size());
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
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	cout << endl;

	Meshy mesh(grueCfg);
//	Segmenter seg(0.35, 0.35);
    Segmenter seg(grueCfg);
	unsigned int t0, t1;
	t0 = clock();

	Meshy mesh3(grueCfg);
//	Segmenter seg3(0.35, 0.35);
    
    Segmenter seg3(grueCfg);
	string inputFile = inputsDir + "Water.stl";
	mesh.readStlFile(inputFile.c_str() );
	t1=clock()-t0;
	mesh.dump(cout);

//	cout << " **** testLayerSplit " << endl;
//	for(int i=0; i != seg.readSliceTable().size(); i++)
//	{
//		stringstream ss;
//		ss << outputsDir << "water_" << i << ".stl";
//		mesh.writeStlFileForLayer(i, ss.str().c_str());
//		cout << ss.str().c_str() << endl;
//	}
}

void ModelReaderTestCase::testLargeMeshy()
{
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	unsigned int t0,t1;
	cout << "Light saber" << endl;
	Meshy mesh(grueCfg);
//	Segmenter seg(0.35, 0.35);
    Segmenter seg(grueCfg);
	t0=clock();
	string inputFile = inputsDir + "lightsaber.stl";
	mesh.readStlFile(inputFile.c_str() );
	t1=clock()-t0;
	mesh.dump(cout);
	cout << "lightsaber read in " << t1 << endl;

}

void ModelReaderTestCase::testMeshyLoad()
{
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	unsigned int t0,t1;
	cout << "Water" << endl;
	Meshy mesh(grueCfg);
//	Segmenter seg(0.35, 0.35);
    Segmenter seg(grueCfg);
    Segmenter seg2(grueCfg);
	t0=clock();
	string inputFile = inputsDir + "Water.stl";
	mesh.readStlFile(inputFile.c_str() );
	t1=clock()-t0;
	mesh.dump(cout);
	seg.tablaturize(mesh);
	cout << "time: " << t1 << endl;
	CPPUNIT_ASSERT_EQUAL((size_t)172, seg.readSliceTable().size());

	cout << "Land" << endl;
	Meshy mesh2(grueCfg);
//	Segmenter seg2(0.35, 0.35);

	t0=clock();
	string inputFile2 = inputsDir + "Land.stl";
	mesh2.readStlFile(inputFile2.c_str() );
	t1=clock()-t0;
	cout << "time: " << t1 << endl;
	seg2.tablaturize(mesh2);
	CPPUNIT_ASSERT_EQUAL((size_t)174, seg2.readSliceTable().size());
	mesh2.dump(cout);
}

void ModelReaderTestCase::testMeshyCycle()
{
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	unsigned int t0,t1;
	string target = inputsDir + "3D_Knot.stl";
	string drop = outputsDir + "3D_Knot.stl";
	string drop2 = outputsDir + "3D_Knot_v2.stl";

	cout << "Reading test file:"  << target << endl;
	Meshy mesh3(grueCfg);
	//Segmenter seg3(0.35, 0.35);
    Segmenter seg3(grueCfg);
    Segmenter seg4(grueCfg);
	t0=clock();
	mesh3.readStlFile(target.c_str());
	t1=clock()-t0;
//	mesh3.dump(cout);
	cout << "Read: " << target <<" in seconds: " << t1 << endl;
	cout << "Writing test file:"  << drop << endl;
	mesh3.writeStlFile( drop.c_str() );
	unsigned int t2=clock()-t1;
	cout << "Wrote: " << drop <<" in seconds: " << t2 << endl;

	cout << "Reload test, reloading file: "  << drop << endl;
	Meshy mesh4(grueCfg);
//	Segmenter seg4(0.35, 0.35);
	t0=clock();
	mesh4.readStlFile( drop.c_str());
	t1=clock()-t0;
	cout << "Re-Read: " << target <<" in seconds: " << t1 << endl;
//	CPP_UNIT_ASSERT(mesh3 == mesh4);
	cout << "Writing test file: "  << drop2 << endl;
	mesh4.writeStlFile( drop2.c_str());
	t2=clock()-t1;
	cout << "Wrote: " << drop2 <<" in seconds: " << t2 << endl;

}

void ModelReaderTestCase::testMeshyCycleNull()
{
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	unsigned int t0,t1;
	string target = inputsDir + "Null.stl";
	string drop = outputsDir + "Null.stl";
	string drop2 = outputsDir + "Null_v2.stl";

	cout << "Reading test file:"  << target << endl;
	Meshy mesh3(grueCfg);
//	Segmenter seg3(0.35, 0.35);
    Segmenter seg3(grueCfg);
    Segmenter seg4(grueCfg);
	t0=clock();
	mesh3.readStlFile(target.c_str());
	t1=clock()-t0;
//	mesh3.dump(cout);
	cout << "Read: " << target <<" in seconds: " << t1 << endl;
	cout << "Writing test file:"  << drop << endl;
	mesh3.writeStlFile( drop.c_str());
	unsigned int t2=clock()-t1;
	cout << "Wrote: " << drop <<" in seconds: " << t2 << endl;

	cout << "Reload test, reloading file: "  << drop << endl;
	Meshy mesh4(grueCfg);
//	Segmenter seg4(0.35, 0.35);
	t0=clock();
	mesh4.readStlFile( drop.c_str());
	t1=clock()-t0;
	cout << "Re-Read: " << target <<" in seconds: " << t1 << endl;
//	CPP_UNIT_ASSERT(mesh3 == mesh4);
	cout << "Writing test file: "  << drop2 << endl;
	mesh4.writeStlFile( drop2.c_str());
	t2=clock()-t1;
	cout << "Wrote: " << drop2 <<" in seconds: " << t2 << endl;
}

void ModelReaderTestCase::testMeshyCycleMin()
{
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	unsigned int t0,t1;
	string target = inputsDir + "OneTriangle.stl";
	string drop = outputsDir + "OneTriangle.stl";
	string drop2 = outputsDir + "OneTriangle_v2.stl";

	cout << "Reading test file:"  << target << endl;
	Meshy mesh3(grueCfg);
    
    Segmenter seg3(grueCfg);
    Segmenter seg4(grueCfg);
//	Segmenter seg3(0.35, 0.35);
	t0=clock();
	mesh3.readStlFile( target.c_str() );
	t1=clock()-t0;
//	mesh3.dump(cout);
	cout << "Read: " << target <<" in seconds: " << t1 << endl;
	cout << "Writing test file:"  << drop << endl;
	mesh3.writeStlFile( drop.c_str());
	unsigned int t2=clock()-t1;
	cout << "Wrote: " << drop <<" in seconds: " << t2 << endl;

	cout << "Reload test, reloading file: "  << drop << endl;
	Meshy mesh4(grueCfg);
//	Segmenter seg4(0.35, 0.35);
	t0=clock();
	mesh4.readStlFile( drop.c_str());
	t1=clock()-t0;
	cout << "Re-Read: " << target <<" in seconds: " << t1 << endl;
//	CPP_UNIT_ASSERT(mesh3 == mesh4);
	cout << "Writing test file: "  << drop2 << endl;
	mesh4.writeStlFile(drop2.c_str());
	t2=clock()-t1;
	cout << "Wrote: " << drop2 <<" in seconds: " << t2 << endl;

}


//void ModelReaderTestCase::testSlicyWater()
//{
//	Meshy mesh;
//	Segmenter seg(0.35, 0.35);
//	string target = inputsDir + "Water.stl";
//	mesh.readStlFile( target.c_str());
//	seg.tablaturize(mesh);
//	const SliceTable& table = seg.readSliceTable();
//
//	unsigned int t0,t1;
//	t0=clock();
//
//	const vector<Triangle3>& allTriangles = mesh.readAllTriangles();
//	int sliceIndex = 0;
//	for (SliceTable::const_iterator i = table.begin(); i != table.end(); i++)
//	{
//		const TriangleIndices& sliceables = *i;
//		t1=clock()-t0;
//		cout << "clock: " << t1 << endl;
//		cout << "Slice: " << sliceIndex  << " ("<< sliceables.size()  << " triangles)" << endl;
//
//		Connexity sy(0.000000001);
//		cout  << " Triangles in this slice" << endl;
//		for (TriangleIndices::const_iterator j = sliceables.begin(); j != sliceables.end(); j++ )
//		{
//			index_t index = (*j);
//			const Triangle3& triangle = allTriangles[index];
////			cout << "adding triangle # " << index << endl;
//			sy.addTriangle(triangle);
//		}
////		cout << sy << endl;
//		sliceIndex ++;
//	}
//	t1=clock()-t0;
//	cout << "clock: " << t1 << endl;
//}




Point2Type rotateAroundPoint(const Point2Type &center, Scalar angle, const Point2Type &p)
{
	// translate point back to origin:
	Point2Type translated = p - center;

	Point2Type rotated = translated.rotate2d( angle );
	// translate point back:
	Point2Type r = rotated + center;
	return r;
}

// openscad debugging
string visibleCut(const Triangle3Type& t, const Point3Type &a, const Point3Type &b)
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
	std::cout << endl << "Starting: " <<__FUNCTION__ << endl;

	Point3Type v0(-1, 0, 0);
	Point3Type v1(1, 0, 0);
	Point3Type v2(0,0,1);

	Triangle3Type t(v0,v1,v2);

	Point3Type a,b;
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

	Point3Type v0b(-1.556260e+01, 5.680465e+00, 2.200485e+01);
	Point3Type v1b(-1.832293e+01, 4.436024e+00, 1.892443e+01 );
	Point3Type v2b( -1.800681e+01, 6.473042e+00, 2.197871e+01);
	Triangle3Type tb(v0b,v1b,v2b);



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
	std::cout << endl << "Starting: " <<__FUNCTION__ << endl;

	Point2Type center(4,4);
	Point2Type a(4,3);

	Scalar angle = M_PI /2;
	Point2Type b = rotateAroundPoint(center, angle, a);

	cout << endl << "rotated " << a << " around " << center << " by " << angle << " rads and got " << b << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 5.0, b.x, 0.00001 );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 4.0, b.y, 0.00001 );

}





void batchProcess(	Scalar , // firstLayerZ,
					Scalar , // layerH,
					Scalar , // layerW,
					Scalar , // tubeSpacing,
					Scalar , // infillShrinking,
					Scalar , // insetDistanceFactor,
					const char* outDir,
					const std::vector<std::string> &models)
{
	//bool writeDebugScadFiles = true;
	FileSystemAbstractor fileSystem;
	std::vector<double> times;
	for (unsigned i=0; i < models.size(); i++)
	{

		std::string modelFile = models[i];
		std::string stlFiles = fileSystem.removeExtension(fileSystem.ExtractFilename(models[i].c_str()).c_str());
		stlFiles += "_";

		std::string scadFile = outDir;
		scadFile += "/";
		scadFile += fileSystem.ChangeExtension(fileSystem.ExtractFilename(models[i].c_str()).c_str(), ".scad" )  ;

		cout << modelFile << " to " << scadFile << endl;

		unsigned int t0,t1;
		t0=clock();

        class MeshCfg : public GrueConfig {
        public:
            MeshCfg() {
                doPutModelOnPlatform = true;
            }
        };
        MeshCfg grueCfg;
		//Scalar angle = M_PI*0.5;
		Meshy mesh(grueCfg);
//		Segmenter seg(firstLayerZ, layerH);
        Segmenter seg(grueCfg);
		mesh.readStlFile( modelFile.c_str());
		cout << modelFile << " LOADED" << endl;
		std::vector< SliceData > slices;
		seg.tablaturize(mesh);
		//unsigned int nbOfShells = 0;
		//Slicy slicy(mesh.readAllTriangles(), mesh.readLimits(), layerW, layerH, mesh.readSliceTable().size(), scadFile.c_str());

		cout << "Slicing" << endl;
		//unsigned int extruderId = 0;
		unsigned int sliceCount = seg.readSliceTable().size();
		for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
		{
			//const TriangleIndices & trianglesForSlice = seg.readSliceTable()[sliceId];
			//Scalar z = seg.readLayerMeasure().sliceIndexToHeight(sliceId);
			//Scalar sliceAngle = sliceId * angle;
			slices.push_back( SliceData());
			//SliceData &slice = slices[sliceId];

			bool hazNewPaths = false;
			/*needs to be converted to slicer and tomographize
			bool hazNewPaths = slicy.slice( trianglesForSlice,
											sliceId,
											extruderId,
											tubeSpacing,
											sliceAngle,
											nbOfShells,
											2 * layerW,
											infillShrinking,
											insetDistanceFactor,
											writeDebugScadFiles,
											slice);*/
			if(!hazNewPaths)
			{
		    	cout << "WARNING: Layer " << sliceId << " has no outline!" << endl;
				slices.pop_back();
			}
		}

		t1=clock()-t0;
		double t = t1 / 1000000.0;
		times.push_back(t);
		cout << "Sliced in " << t << " seconds" << endl;
	}

	cout << endl << endl << "MODEL    TIME 2 SLICE (s)" << endl;
	cout << " ----------" <<endl;
	for (unsigned i=0; i < models.size(); i++)
	{
		cout << models[i] << "\t" << times[i] << endl;
	}
}

void ModelReaderTestCase::testMyStls()
{
	std::cout << endl << "Starting: " <<__FUNCTION__ << endl;

	Scalar firstLayerZ = 0.11;
	Scalar layerH = 0.35;
	Scalar layerW = 0.5833333;
	Scalar tubeSpacing = 1.0;

	std::vector<std::string> models;

	models.push_back("inputs/3D_Knot.stl");
	models.push_back("inputs/Water.stl");
	models.push_back("inputs/hexagon.stl");
	models.push_back("inputs/Land.stl");
//	models.push_back("../stls/monitor_simple.stl");
//	models.push_back("../stls/F1.stl");
//	models.push_back("../stls/hexagon.stl");
//	models.push_back("../stls/Land.stl");
//	models.push_back("../stls/Roal10.stl");
//	models.push_back("../stls/soap_highres.stl");
//	models.push_back("../stls/TeaPot.stl");
//	models.push_back("../stls/Water.stl");
//	models.push_back("../stls/Yodsta_Printdata.stl");

	Scalar infillShrinking = 0.35;
	Scalar insetDistanceFactor = 0.8;
	//models.push_back("Pivot-Joint_-_Ball_End_-1X");
	//models.push_back("Toymaker_Skull_1_Million_Polys");
	//models.push_back("part2");

	batchProcess(firstLayerZ, layerH, layerW, tubeSpacing, infillShrinking,
							insetDistanceFactor, outputsDir.c_str(), models);
}

void ModelReaderTestCase::testKnot()
{
	std::cout << endl << "Starting: " <<__FUNCTION__ << endl;
	Scalar firstZ = 0.11;
	Scalar layerH = 0.35;
	Scalar layerW = 0.5833333;
	Scalar tubeSpacing = 1.0;
	Scalar infillShrinking = 0.35;
	Scalar insetDistanceFactor = 0.8;

	std::vector<std::string> models;
	models.push_back("inputs/3D_Knot.stl");
	batchProcess(firstZ, layerH, layerW, tubeSpacing, infillShrinking,
			insetDistanceFactor, outputsDir.c_str(), models);
}

void ModelReaderTestCase::testInputStls()
{
	std::cout << endl << "Starting: " <<__FUNCTION__ << endl;
	Scalar firstZ = 0.11;
	Scalar layerH = 0.35;
	Scalar layerW = 0.5833333;
	Scalar tubeSpacing = 1.0;
	Scalar infillShrinking = 0.35;
	Scalar insetDistanceFactor = 0.8;

	std::string outDir = "test_cases/modelReaderTestCase/output";
	std::vector<std::string> models;

	models.push_back("inputs/3D_Knot.stl");
	models.push_back("inputs/Water.stl");
	models.push_back("inputs/hexagon.stl");
	models.push_back("inputs/Land.stl");

	batchProcess(firstZ, layerH, layerW, tubeSpacing, infillShrinking,
			insetDistanceFactor,outDir.c_str(), models);

}

void ModelReaderTestCase::testTubularInflate()
{
	std::cout << endl << "Starting: " <<__FUNCTION__ << endl;

	Limits l0;

	Point3Type p0(0,0,0);
	Point3Type p1(8,4,1);

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
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	cout << endl;

	Scalar tol = 1e-6;

	Scalar firstZ =0.11;
	double layerH = 0.35;
	int layerIndex = 30;

	LayerMeasure zTapeMeasure(firstZ, layerH, 0.43);

	Meshy mesh(grueCfg);
	//Segmenter seg(firstZ, layerH); // 0.35
    Segmenter seg(grueCfg);
	cout << "LOADING... " << endl;
	mesh.readStlFile( "inputs/3D_Knot.stl");

	Scalar z = zTapeMeasure.sliceIndexToHeight(30);

	const std::vector<Triangle3Type> &allTriangles = mesh.readAllTriangles();
	seg.tablaturize(mesh);
	const SliceTable &sliceTable = seg.readSliceTable();
	const TriangleIndices &trianglesForSlice = sliceTable[30];

	std::vector<Segment2Type> segments;
	// get 2D paths for outline
	segmentationOfTriangles(trianglesForSlice, allTriangles, z, segments);
	SegmentTable outlinesSegments;
	loopsAndHoleOgy(segments, tol, outlinesSegments);

	std::vector<std::pair<double,double> > minsAndMaxes;
	size_t triangleCount = trianglesForSlice.size();
	minsAndMaxes.reserve(triangleCount);
	cout << "triangles for layer "<< layerIndex <<", z=" << z<< endl;

	for (unsigned i=0; i < triangleCount; i++)
	{
		index_t idx = trianglesForSlice[i];
		const Triangle3Type &t = allTriangles[idx];

		Point3Type a,b,c;
		t.zSort(a,b,c);

		minsAndMaxes.push_back(std::pair<Scalar, Scalar>(a.z, c.z) );
		cout << t[0].z << "\t\t" << t[1].z << "\t\t" << t[2].z << "\t\t" << a.z << "\t"<< c.z << endl;
		cout << "  min " << a.z << " max " << c.z << endl;
		CPPUNIT_ASSERT(z >= a.z);
//		CPPUNIT_ASSERT(z <= max);
	}

	for (unsigned i=0; i < triangleCount; i++)
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
	LayerMeasure n(0.11, 0.35, 0.43); // n is for nozzle

	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.11, n.sliceIndexToHeight(0), tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.46, n.sliceIndexToHeight(1), tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.81, n.sliceIndexToHeight(2), tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.16, n.sliceIndexToHeight(3), tol);

	Scalar z = 0;
	unsigned int abode = n.zToLayerAbove(z);
	//cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 0, abode);

	z = 0.1;
	abode = n.zToLayerAbove(z);
	//cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 0, abode);

	z = 0.11;
	abode = n.zToLayerAbove(z);
	//cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 1, abode);

	z = 0.12;
	abode = n.zToLayerAbove(z);
	//cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 1, abode);

	z = 0.45;
	abode = n.zToLayerAbove(z);
	//cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 1, abode);

	z = 0.46;
	abode = n.zToLayerAbove(z);
	//cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 2, abode);

	z = 0.47;
	abode = n.zToLayerAbove(z);
	//cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 2, abode);

	z = 1.15999;
	abode = n.zToLayerAbove(z);
	//cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 3, abode);

	z = 1.1600001;
	abode = n.zToLayerAbove(z);
	cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 4, abode);

	// one million! mouhahahahahaahahahaaha :-)
	z = 350000.11;
	abode = n.zToLayerAbove(z);
	//cout << "h=" << z << " layer=" << abode << endl;
	CPPUNIT_ASSERT_EQUAL( (unsigned int) 1000001, abode);
}


void ModelReaderTestCase::testAlignToPlate() {
    class MeshCfg : public GrueConfig {
    public:
        MeshCfg() {
            layerH = 0.5;
            firstLayerZ = 0.5;
            doPutModelOnPlatform = true;
        }
    };
    MeshCfg grueCfg;
	cout << endl << "Testing object above the bed" << endl;
	string above_file = inputsDir + "above.stl";
	Meshy above(grueCfg);
//	Segmenter segabove(.5, .5);
    Segmenter segabove(grueCfg);
	//10mm cube one mm above the bed
	above.readStlFile(above_file.c_str());

	//before we align
	CPPUNIT_ASSERT_EQUAL(above.readLimits().zMin, 1.0);
	CPPUNIT_ASSERT_EQUAL(above.readLimits().zMax, 11.0);

	above.alignToPlate();
	segabove.tablaturize(above);

	//after alignment
	CPPUNIT_ASSERT_EQUAL(above.readLimits().zMin, 0.0);
	CPPUNIT_ASSERT_EQUAL(above.readLimits().zMax, 10.0);
	//we make a phantom layer above the last real layer
	CPPUNIT_ASSERT_EQUAL(21, (int)segabove.readSliceTable().size());

	cout << endl << "Testing object below the bed" << endl;
	string below_file = inputsDir + "below.stl";
	Meshy below(grueCfg);
//	Segmenter segbelow(.5, .5);
    Segmenter segbelow(grueCfg);
	//10mm cube one mm below the bed
	below.readStlFile(below_file.c_str());

	//before we align
	CPPUNIT_ASSERT_EQUAL(below.readLimits().zMin, -1.0);
	CPPUNIT_ASSERT_EQUAL(below.readLimits().zMax, 9.0);

	below.alignToPlate();
	segbelow.tablaturize(below);

	//after alignment
	CPPUNIT_ASSERT_EQUAL(below.readLimits().zMin, 0.0);
	CPPUNIT_ASSERT_EQUAL(below.readLimits().zMax, 10.0);
	//we make a phantom layer above the last real layer
	CPPUNIT_ASSERT_EQUAL((int)segbelow.readSliceTable().size(), 21);
	
}

void initConfig(Configuration &config)
{
	config["slicer"]["firstLayerZ"] = 0.11;
	config["slicer"]["layerH"] = 0.35;
}

//void slicyTest()
//{
//	cout << endl;
//	string modelFile = "inputs/3D_Knot.stl";
//
//    Configuration config;
//    initConfig(config);
//	Meshy mesh;
//	Segmenter seg(config["slicer"]["firstLayerZ"].asDouble(), config["slicer"]["layerH"].asDouble()); // 0.35
//	mesh.readStlFile(modelFile.c_str());
//	seg.tablaturize(mesh);
//	cout << "file " << modelFile << endl;
//	const SliceTable &sliceTable = seg.readSliceTable();
//	int layerCount = sliceTable.size();
//	cout  << "Slice count: "<< layerCount << endl;
//	const vector<Triangle3> &allTriangles = mesh.readAllTriangles();
//	cout << "Faces: " << allTriangles.size() << endl;
//	cout << "layer " << layerCount-1 << " z: " << seg.readLayerMeasure().sliceIndexToHeight(layerCount-1) << endl;
//
//	int layerIndex = 44;
//	CPPUNIT_ASSERT (layerIndex < layerCount);
//	const TriangleIndices &trianglesInSlice = sliceTable[layerIndex];
//	unsigned int triangleCount = trianglesInSlice.size();
//	Scalar z = seg.readLayerMeasure().sliceIndexToHeight(layerIndex);
//	cout << triangleCount <<" triangles in layer " << layerIndex  << " z = " << z << endl;
//
//	// Load slice connectivity information
//	Connexity connexity(1e-6);
//	for (int i=0; i < triangleCount; i++)
//	{
//		unsigned int triangleIndex = trianglesInSlice[i];
//		const Triangle3& t = allTriangles[triangleIndex];
//		connexity.addTriangle(t);
//	}
//
//	cout << connexity << endl;
//
//	list<index_t> faces;
//	size_t faceCount = connexity.readFaces().size();
//	for(index_t i=0; i< faceCount; i++)
//	{
//		faces.push_back(i);
//
//	}
//
//
//	const Face &face = connexity.readFaces()[0];
//	LineSegment2 cut;
//	bool success = connexity.cutFace(z, face, cut);
//	cout << "FACE cut " << cut.a << " to " << cut.b << endl;
//	CPPUNIT_ASSERT(success);
//
//	list<LineSegment2> loop;
//	connexity.splitLoop(z, faces, loop);
//
//	cout << "First loop has " << loop.size() << " segments" << endl;
//
//	size_t i=0;
//	for(list<LineSegment2>::iterator it = loop.begin(); it != loop.end(); it++)
//	{
//		cout << i << "] " << it->a << ", " << it->b << endl;
//		i++;
//	}
//
//	cout << "loop with " << loop.size() << "faces" << endl;
//	cout << "faces left: "  << faces.size()  << " / " << connexity.readEdges().size() << endl;
//
////	list<index_t> edges;
////	slicy.fillEdgeList(z,edges);
////	dumpIntList(edges);
//
//}
