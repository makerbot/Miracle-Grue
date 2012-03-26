
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

void SlicerCupTestCase::setUp()
{
	MyComputer computer;
	computer.fileSystem.mkpath(outputDir.c_str());
	computer.fileSystem.mkpath(outputDir2.c_str());
}



void SlicerCupTestCase::testIndividuals()
{


	string stlDir("./test_cases/slicerCupTestCase/stls/");

	vector<string> models;
	models.push_back(stlDir + "3D_Knot.stl");
	models.push_back(stlDir + "Hollow_Pyramid.stl");
	models.push_back(stlDir + "linkCup.stl");
	models.push_back(stlDir + "ultimate_calibration_test.stl");
	models.push_back(stlDir + "Cathedral_Crossing_fixed.stl");


	string configFileName("miracle.config");
	Configuration config;
	config.readFromFile(configFileName.c_str());

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
		miracleGrue(	gcoder,
						slicer,
						modelFile.c_str(),
						scadFile.c_str(),
						gcodeFile.c_str(),
						-1,-1,
						slices);

		cout << computer.clock.now() << endl;
		cout << "DONE!" << endl;
	}

	cout << endl;
	cout << endl;
	cout << endl;
}



void SlicerCupTestCase::testAllTogeter()
{
	string configFileName("miracle.config");
	Configuration config;
	config.readFromFile(configFileName.c_str());
	GCoder gcoder;
	loadGCoderData(config, gcoder);
	Slicer slicer;
	loadSlicerData(config, slicer);

	cout << endl;
	cout << computer.clock.now() << endl;
	std::vector<mgl::SliceData> slices;
	miracleGrue(gcoder, slicer,
				"./test_cases/slicerCupTestCase/stls/all_together.stl",
				"./outputs/test_cases/slicerCupTestCase/all_together.scad",
				"./outputs/test_cases/slicerCupTestCase/all_together.gcode",
				-1,-1,
				slices);
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}

void SlicerCupTestCase::testCathedral_Crossing_bad()
{
	string configFileName("miracle.config");
	Configuration config;
	config.readFromFile(configFileName.c_str());
	GCoder gcoder;
	loadGCoderData(config, gcoder);
	Slicer slicer;
	loadSlicerData(config, slicer);

	cout << endl;
	cout << computer.clock.now() << endl;

	std::vector<mgl::SliceData> slices;
	miracleGrue(gcoder, slicer,
				"./test_cases/slicerCupTestCase/stls/Cathedral_Crossing.stl",
				"./outputs/test_cases/slicerCupTestCase/Cathedral_Crossing.scad",
				"./outputs/test_cases/slicerCupTestCase/Cathedral_Crossing.gcode",
				-1,-1,
				slices);
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}

void SlicerCupTestCase::testCathedral_Crossing_fixed()
{
	string configFileName("miracle.config");
	Configuration config;
	config.readFromFile(configFileName.c_str());
	GCoder gcoder;
	loadGCoderData(config, gcoder);
	Slicer slicer;
	loadSlicerData(config, slicer);

	cout << endl;
	cout << computer.clock.now() << endl;
	std::vector<mgl::SliceData> slices;
	miracleGrue(gcoder, slicer,
				"./test_cases/slicerCupTestCase/stls/Cathedral_Crossing_fixed.stl",
				"./outputs/test_cases/slicerCupTestCase/Cathedral_Crossing_fixed.scad",
				"./outputs/test_cases/slicerCupTestCase/Cathedral_Crossing_fixed.gcode",
				-1,-1,
				slices);
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}


void SlicerCupTestCase::testSpecificIssues()
{
	cout << endl;
	string configFileName("miracle.config");
	Configuration config;
	config.readFromFile(configFileName.c_str());
	GCoder gcoder;
	loadGCoderData(config, gcoder);
	Slicer slicer;
	loadSlicerData(config, slicer);

	cout << "Slumping: full head.stl" << endl;
	std::vector<mgl::SliceData> slices_full;
	miracleGrue(gcoder, slicer,
				"./test_cases/specific_issues/slumping/full head.stl",
				"./outputs/test_cases/specific_issues/full head.scad",
				"./outputs/test_cases/specific_issues/full head.gcode",
				-1,-1,
				slices_full);

	cout << "Slumping: half head.stl" << endl;

	std::vector<mgl::SliceData> slices_half;
	miracleGrue(gcoder, slicer,
				"./test_cases/specific_issues/slumping/half head.stl",
				"./outputs/test_cases/specific_issues/half head.scad",
				"./outputs/test_cases/specific_issues/half head.gcode",
				-1,-1,
				slices_half);

	cout << "Insetting: holy_cube.stl" << endl;
	std::vector<mgl::SliceData> slices_cube;
	miracleGrue(gcoder, slicer,
				"./test_cases/specific_issues/insetting/holy_cube.stl",
				"./outputs/test_cases/specific_issues/holy_cube.scad",
				"./outputs/test_cases/specific_issues/holy_cube.gcode",
				-1,-1,
				slices_cube);
}



