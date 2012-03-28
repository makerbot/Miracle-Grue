
#include <cppunit/config/SourcePrefix.h>
#include "SlicerCupTestCase.h"

//
#include "mgl/configuration.h"
//#include "mgl/slicy.h"
#include "mgl/miracle.h"


CPPUNIT_TEST_SUITE_REGISTRATION( SlicerCupTestCase );

using namespace std;
using namespace mgl;

MyComputer computer;

string outputDir ("outputs/test_cases/slicerCupTestCase/");
string outputDir2("outputs/test_cases/specific_issues/");

string inputDir("./test_cases/slicerCupTestCase/stls/");
string inputDir2("./test_cases/specific_issues/");


void SlicerCupTestCase::setUp()
{
	MyComputer computer;
	computer.fileSystem.mkpath(outputDir.c_str());
	computer.fileSystem.mkpath(outputDir2.c_str());
}


void testModels(vector<string>& models, const char* configFile)
{

	Configuration config;
	config.readFromFile(configFile);

	for(int i=0; i< models.size(); i++)
	{
		string modelFile = models[i];
		string scadFile(outputDir);
		scadFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile.c_str()).c_str(), ".scad");
		string gcodeFile(outputDir);
		gcodeFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile.c_str()).c_str(), ".gcode");

		cout << endl;
		cout << endl;
		cout << endl;
		cout << "---------- " << (i+1) << "/"<<  models.size() << " -------------------" << endl;

		cout << endl;
		cout << modelFile << endl;
		cout << computer.clock.now() << endl;

		GCoder gcoder;
		loadGCoderData(config, gcoder);

		Slicer slicer;
		loadSlicerData(config, slicer);

		std::vector< SliceData >  slices;
		std::vector<Scalar> zIndicies;

		Meshy mesh(slicer.firstLayerZ, slicer.layerH); // 0.35
		mesh.readStlFile( modelFile.c_str());

		miracleGrue(gcoder, slicer, modelFile.c_str(), NULL, gcodeFile.c_str(), -1, -1, slices);
		cout << computer.clock.now() << endl;
		cout << "DONE!" << endl;
	}

	cout << endl;
	cout << endl;
	cout << endl;
}

void testModel(const char *model, const char* configFile)
{
	vector<string> models;
	models.push_back(model);
	testModels(models, configFile);
}

void testModel(string model, const char* configFile)
{
	testModel(model.c_str(), configFile);
}

void SlicerCupTestCase::testIndividuals()
{
	vector<string> models;
	models.push_back(inputDir + "3D_Knot.stl");
	models.push_back(inputDir + "Hollow_Pyramid.stl");
	models.push_back(inputDir + "linkCup.stl");
	models.push_back(inputDir + "ultimate_calibration_test.stl");
	models.push_back(inputDir + "Cathedral_Crossing_fixed.stl");
	testModels(models, "miracle.config");
}

void SlicerCupTestCase::testAllTogeter()
{
	testModel(inputDir + "all_together.stl", "miracle.config");

}

void SlicerCupTestCase::testCathedral_Crossing_bad()
{
	testModel(inputDir + "Cathedral_Crossing.stl", "miracle.config");
}

void SlicerCupTestCase::testCathedral_Crossing_fixed()
{
	testModel(inputDir + "Cathedral_Crossing_fixed.stl", "miracle.config");
}

void SlicerCupTestCase::testSpecificIssues()
{
	testModel(inputDir2 + "slumping/full head.stl", "miracle.config");
}

void SlicerCupTestCase::testSpecificIssuesB()
{
	testModel(inputDir2 + "slumping/half head.stl", "miracle.config");
}

void SlicerCupTestCase::testSpecificIssuesC()
{
	cout << endl;
	testModel(inputDir2 + "insetting/holy_cube.stl", "miracle.config");
}



void SlicerCupTestCase::testSliceShifting()
{
	testModel(inputDir2 + "insetting/holy_cube.stl", "miracle.config");
}


