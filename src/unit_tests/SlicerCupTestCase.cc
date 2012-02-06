
#include <cppunit/config/SourcePrefix.h>
#include "SlicerCupTestCase.h"


#include "mgl/configuration.h"
#include "mgl/slicy.h"
#include "mgl/gcoder.h"


CPPUNIT_TEST_SUITE_REGISTRATION( SlicerCupTestCase );

using namespace std;
using namespace mgl;

MyComputer computer;


void miracleGrue(const char *configFilePath,
					const char *modelFilePath,
						const char* outputDirectory)
{
	unsigned int nbOfShells = 0;

	string configFileName(configFilePath);
	string modelFile(modelFilePath);

    Configuration config;
    config.readFromFile(configFileName.c_str());

    Meshy mesh(config["slicer"]["firstLayerZ"].asDouble(), config["slicer"]["layerH"].asDouble()); // 0.35
    loadMeshyFromStl(mesh, modelFile.c_str());

    string scadFile(outputDirectory);
    scadFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".scad");
    string gcodeFile("./test_cases/slicerCupTestCase/output/");
    gcodeFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".gcode");

    cout << "Slicing: " << modelFilePath << endl;

    //std::vector<TubesInSlice> allTubes;
    std::vector<SliceData> slices;
    Scalar layerW = config["slicer"]["layerW"].asDouble();
    Scalar tubeSpacing = config["slicer"]["tubeSpacing"].asDouble();
    Scalar angle = config["slicer"]["angle"].asDouble();

    assert(layerW >0);
    assert(tubeSpacing >0);

	Slicy slicy(mesh,layerW, scadFile.c_str());
	slicy.sliceAndPath(tubeSpacing, angle, nbOfShells, slices);

    cout << "Writing the gcode to \"" << gcodeFile << "\""<< endl;
    writeGcodeFile(config, gcodeFile.c_str(), slices);
}

void SlicerCupTestCase::testIndividuals()
{
	string configFileName("miracle.config");
	vector<string> models;

	string stlDir("./test_cases/slicerCupTestCase/stls/");
	models.push_back(stlDir + "3D_Knot.stl");
	models.push_back(stlDir + "Hollow_Pyramid.stl");
	models.push_back(stlDir + "linkCup.stl");
	models.push_back(stlDir + "ultimate_calibration_test.stl");
	models.push_back(stlDir + "Cathedral_Crossing_fixed.stl");

	for(int i=0; i< models.size(); i++)
	{
		cout << endl;
		cout << endl;
		cout << endl;
		cout << "---------- " << (i+1) << "/"<<  models.size() << " -------------------" << endl;
		string modelFile = models[i];
		cout << endl;
		cout << modelFile << endl;
		cout << computer.clock.now() << endl;
		miracleGrue(configFileName.c_str(),
						modelFile.c_str(),
							"./test_cases/slicerCupTestCase/output/");
		cout << computer.clock.now() << endl;
		cout << "DONE!" << endl;
	}

	cout << endl;
	cout << endl;
	cout << endl;
}



void SlicerCupTestCase::testAllTogeter()
{
	cout << endl;
	cout << computer.clock.now() << endl;
	miracleGrue("miracle.config",
				"./test_cases/slicerCupTestCase/stls/all_together.stl",
				"./test_cases/slicerCupTestCase/output/");
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}

void SlicerCupTestCase::testCathedral_Crossing_bad()
{
	cout << endl;
	cout << computer.clock.now() << endl;
	miracleGrue("miracle.config",
				"./test_cases/slicerCupTestCase/stls/Cathedral_Crossing.stl",
				"./test_cases/slicerCupTestCase/output/");
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}

void SlicerCupTestCase::testCathedral_Crossing_fixed()
{
	cout << endl;
	cout << computer.clock.now() << endl;
	miracleGrue("miracle.config",
				"./test_cases/slicerCupTestCase/stls/Cathedral_Crossing_fixed.stl",
				"./test_cases/slicerCupTestCase/output/");
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}


void SlicerCupTestCase::testSpecificIssues()
{
	cout << endl;

	cout << "Slumping" << endl;
	miracleGrue("miracle.config",
				"./test_cases/specific_issues/slumping/full head.stl",
				"./test_cases/specific_issues/output/");

	miracleGrue("miracle.config",
				"./test_cases/specific_issues/slumping/half head.stl",
				"./test_cases/specific_issues/output/");

	cout << "Insetting" << endl;
	miracleGrue("miracle.config",
				"./test_cases/specific_issues/insetting/holy_cube.stl",
				"./test_cases/specific_issues/output/");
}



