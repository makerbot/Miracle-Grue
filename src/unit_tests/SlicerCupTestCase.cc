
#include <cppunit/config/SourcePrefix.h>
#include "UnitTestUtils.h"
#include "SlicerCupTestCase.h"
#include "mgl/configuration.h"
//#include "mgl/slicy.h"
#include "mgl/miracle.h"


CPPUNIT_TEST_SUITE_REGISTRATION(SlicerCupTestCase);

using namespace std;
using namespace mgl;
using namespace libthing;

MyComputer computer;

string outputDir("outputs/test_cases/slicerCupTestCase/");
string outputDir2("outputs/test_cases/specific_issues/");

string inputDir("./test_cases/slicerCupTestCase/stls/");
string inputDir2("./test_cases/specific_issues/");

void SlicerCupTestCase::setUp() {
	MyComputer computer;
	char pathsep = computer.fileSystem.getPathSeparatorCharacter();
	outputDir = string("outputs") + pathsep + string("test_cases") +
			pathsep + string("slicerCupTestCase") + pathsep;
	outputDir2 = string("outputs") + pathsep + string("test_cases") +
			pathsep + string("specific_issues") + pathsep;
	inputDir = string("test_cases") + pathsep + string("slicerCupTestCase") +
			pathsep + string("stls") + pathsep;
	inputDir = string("test_cases") + pathsep + string("specific_issues") +
			pathsep;
	computer.fileSystem.guarenteeDirectoryExistsRecursive(outputDir.c_str());
	computer.fileSystem.guarenteeDirectoryExistsRecursive(outputDir2.c_str());
	computer.fileSystem.guarenteeDirectoryExistsRecursive(inputDir.c_str());
	computer.fileSystem.guarenteeDirectoryExistsRecursive(inputDir2.c_str());
}

void testModel(const char *model, const char* configFile) {
	//	vector<string> models;
	//	models.push_back(model);
	//	testModels(models, configFile);
	string modelFile = model;
	string scadFile(outputDir);
	scadFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile.c_str()).c_str(), ".scad");
	string gcodeFile(outputDir);
	gcodeFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile.c_str()).c_str(), ".gcode");

	cout << endl;
	cout << modelFile << endl;
	cout << computer.clock.now() << endl;

	Configuration config;
	config.readFromFile(configFile);

	SlicerConfig slicerCfg;
	loadSlicerConfigFromFile(config, slicerCfg);

	GCoderConfig gcoderCfg;
	loadGCoderConfigFromFile(config, gcoderCfg);

	RegionerConfig regionerCfg;
	loadRegionerConfigFromFile(config, regionerCfg);
    
    PatherConfig patherCfg;
    loadPatherConfigFromFile(config, patherCfg);

	ExtruderConfig extruderCfg;
	loadExtruderConfigFromFile(config, extruderCfg);

	RegionList skeleton;
	std::vector< SliceData > slices;

	std::ofstream gcodeFileStream(gcodeFile.c_str());
	try {
		miracleGrue(gcoderCfg, slicerCfg, regionerCfg, patherCfg, 
				extruderCfg, modelFile.c_str(), NULL,
				gcodeFileStream, -1, -1,
				skeleton,
				slices);
	} catch (mgl::Exception mgle) {
		CPPUNIT_FAIL(mgle.error);
	}

	gcodeFileStream.close();
}

void testModels(vector<string>& models, const char* configFile) {

	for (unsigned i = 0; i < models.size(); i++) {
		cout << endl;
		cout << endl;
		cout << endl;
		cout << "---------- " << (i + 1) << "/" << models.size() << " -------------------" << endl;
		string modelFile = models[i];

		testModel(modelFile.c_str(), configFile);

		cout << computer.clock.now() << endl;
		cout << "DONE!" << endl;
	}

	cout << endl;
	cout << endl;
	cout << endl;
}

void testModel(const string &model, const char* configFile) {
	testModel(model.c_str(), configFile);
}

void SlicerCupTestCase::testIndividuals() {
	vector<string> models;
	models.push_back(inputDir + "3D_Knot.stl");
	models.push_back(inputDir + "Hollow_Pyramid.stl");
	models.push_back(inputDir + "linkCup.stl");
	models.push_back(inputDir + "ultimate_calibration_test.stl");
	models.push_back(inputDir + "Cathedral_Crossing_fixed.stl");

	try {

		testModels(models, "miracle.config");
	}	catch (mgl::Exception &e) {
		cout << e.error << endl;
	}	catch (...) {
		CPPUNIT_FAIL("unknown error during slicing");
	}
}

void SlicerCupTestCase::testAllTogeter() {
	testModel(inputDir + "all_together.stl", "miracle.config");
}

void SlicerCupTestCase::testCathedral_Crossing_bad() {
	testModel(inputDir + "Cathedral_Crossing.stl", "miracle.config");
}

void SlicerCupTestCase::testCathedral_Crossing_fixed() {
	testModel(inputDir + "Cathedral_Crossing_fixed.stl", "miracle.config");
}

void SlicerCupTestCase::testSpecificIssues() {
	testModel(inputDir2 + "slumping/full head.stl", "miracle.config");
}

void SlicerCupTestCase::testSpecificIssuesB() {
	testModel(inputDir2 + "slumping/half head.stl", "miracle.config");
}

void SlicerCupTestCase::testSpecificIssuesC() {
	cout << endl;
	testModel(inputDir2 + "insetting/holy_cube.stl", "miracle.config");
}

void addPoints(mgl::Polygon& pol) {
	pol.push_back(Vector2());
	pol.push_back(Vector2());
	pol.push_back(Vector2());
	pol.push_back(Vector2());
	pol.push_back(Vector2());
	pol.push_back(Vector2());
	pol.push_back(Vector2());
}

void addPolys(Polygons& pol) {
	mgl::Polygon p0, p1, p2, p3;
	addPoints(p0);
	addPoints(p1);
	addPoints(p2);
	addPoints(p3);
	pol.push_back(p0);
	pol.push_back(p1);
	pol.push_back(p2);
	pol.push_back(p3);
}

void addPolysGroup(PolygonsGroup &pg) {
	Polygons p0, p1, p2;
	addPolys(p0);
	addPolys(p1);
	addPolys(p2);
	pg.push_back(p0);
	pg.push_back(p1);
	pg.push_back(p2);
}

void addExtruderSlice(ExtruderSlice &slice) {
	mgl::Polygon p1, p2;
	Polygons pg1;
	addPoints(p1);
	addPoints(p2);
	addPolys(pg1);
	slice.boundary.push_back(p1);
	slice.infills.push_back(p2);
	slice.insetLoopsList.push_back(pg1);
}

void testStackCreation(std::vector<SliceData>& slices) {
	mgl::Polygon pol;
	addPoints(pol);
	CPPUNIT_ASSERT(7 == pol.size());

	pol.erase(pol.begin(), pol.end());
	CPPUNIT_ASSERT(0 == pol.size());

	Polygons pols;
	addPolys(pols);
	CPPUNIT_ASSERT(4 == pols.size());
	pols.erase(pols.begin(), pols.end());
	CPPUNIT_ASSERT(0 == pols.size());

	addPolys(pols);
	pols.erase(pols.begin() + 1, pols.end());
	CPPUNIT_ASSERT(1 == pols.size());

	PolygonsGroup pg;
	addPolysGroup(pg);
	CPPUNIT_ASSERT(3 == pg.size());
	pg.erase(pg.begin(), pg.end());
	CPPUNIT_ASSERT(0 == pg.size());

	ExtruderSlice eSlice;
	addExtruderSlice(eSlice);

	ExtruderSlice eSlice2 = eSlice;

	/// Either
	SliceData& sliceData = *slices.begin();
	sliceData.extruderSlices.push_back(eSlice);
	/// Or
	//	(*slices.begin()).extruderSlices.push_back(eSlice);

	CPPUNIT_FAIL("this throws failsauce when run with valgrind");


	//	LayerMeasure lm(0.1, 0.5);
	//	slices.push_back(SliceData(lm.sliceIndexToHeight(0),0));
	//
	//	ExtruderSlices& s0 = *slices.rbegin();
	//	s0.extruderSlices.push_back(ExtruderSlice());
	//	ExtruderSlice& a0 = *s0.rbegin();
	//	a0.boundary.push_back(Polygons());
	//	Polygons &p0 = *a0.boundary.rbegin();
	//
	//
	//
	//
	//	ExtruderSlices& s1 = *slices.rbegin();
	//	s1.extruderSlices.push_back(ExtruderSlice());
	//	ExtruderSlices& s2 = *slices.rbegin();
	//	s2.extruderSlices.push_back(ExtruderSlice());
	//	slices.push_back(SliceData(lm.sliceIndexToHeight(1),1));
	//	slices.push_back(SliceData(lm.sliceIndexToHeight(2),2));
	//



}


//testStackCreation(slices);

////	for(size_t c = 0; c < slices.size(); c++)
////	{
////		SliceData& x = slices[c];
////		cout << x << endl;
//	}

/*

void SlicerCupTestCase::testSliceShifting()
{
	std::vector<SliceData> slices;
	LayerMeasure lm(0.1, 0.5);
	slices.push_back(SliceData(lm.sliceIndexToHeight(0),0));
	slices.push_back(SliceData(lm.sliceIndexToHeight(1),1));
	slices.push_back(SliceData(lm.sliceIndexToHeight(2),2));


	//
	// remove none
	adjustSlicesToPlate(slices,lm,0,2);
	CPPUNIT_ASSERT_EQUAL((size_t)3, slices.size() );
	CPPUNIT_ASSERT_EQUAL((size_t)0, slices[0].getIndex());
	CPPUNIT_ASSERT_EQUAL((size_t)1, slices[1].getIndex());
	CPPUNIT_ASSERT_EQUAL((size_t)2, slices[2].getIndex());


	// remove from top
	adjustSlicesToPlate(slices,lm,0,1);
	CPPUNIT_ASSERT_EQUAL((size_t)2, slices.size() );
	CPPUNIT_ASSERT_EQUAL((size_t)0, slices[0].getIndex());
	CPPUNIT_ASSERT_EQUAL((Scalar)0.1, slices[0].getZHeight());
	CPPUNIT_ASSERT_EQUAL((size_t)1, slices[1].getIndex());
	CPPUNIT_ASSERT_EQUAL((Scalar)0.6, slices[1].getZHeight());

	// remove from bottom
	slices.push_back(SliceData(1.1,3));
	adjustSlicesToPlate(slices,lm,1,2);
	CPPUNIT_ASSERT_EQUAL((size_t)2, slices.size() );
	CPPUNIT_ASSERT_EQUAL((size_t)0, slices[0].getIndex());
	CPPUNIT_ASSERT_EQUAL((size_t)1, slices[1].getIndex());


//	testModel(inputDir2 + "insetting/holy_cube.stl", "miracle.config");
}
 */

