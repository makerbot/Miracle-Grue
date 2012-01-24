
#include <cppunit/config/SourcePrefix.h>
#include "SlicerCupTestCase.h"

#include "mgl/meshy.h"
#include "GCoderOperation.h"


CPPUNIT_TEST_SUITE_REGISTRATION( SlicerCupTestCase );

using namespace std;
using namespace mgl;

MyComputer computer;


void miracleGrue(const char *configFilePath, const char *modelFilePath)
{

	string configFileName(configFilePath);
	string modelFile(modelFilePath);

    Configuration config;
    config.readFromFile(configFileName.c_str());
    Meshy mesh(config["slicer"]["firstLayerZ"].asDouble(), config["slicer"]["layerH"].asDouble()); // 0.35
    loadMeshyFromStl(mesh, modelFile.c_str());

    string scadFile("./test_cases/slicerCupTestCase/output/");
    scadFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".scad");
    string gcodeFile("./test_cases/slicerCupTestCase/output/");
    gcodeFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".gcode");

    cout << "Slicing: " << modelFilePath << endl;

    //std::vector<TubesInSlice> allTubes;
    std::vector<SliceData> slices;
    sliceAndPath(	mesh,
    				config["slicer"]["layerW"].asDouble(),
    				config["slicer"]["tubeSpacing"].asDouble(),
    				config["slicer"]["angle"].asDouble(),
    				scadFile.c_str(),
    				slices);

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
		miracleGrue(configFileName.c_str(), modelFile.c_str());
		cout << computer.clock.now() << endl;
		cout << "DONE!" << endl;
	}

	cout << endl;
	cout << endl;
	cout << endl;
}

void SlicerCupTestCase::testOddShapes()
{
	miracleGrue("miracle.config","./inputs/holy_cube.stl");
}

void SlicerCupTestCase::testAllTogeter()
{
	cout << endl;
	cout << computer.clock.now() << endl;
	miracleGrue("miracle.config",
				"./test_cases/slicerCupTestCase/stls/all_together.stl");
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}

void SlicerCupTestCase::testCathedral_Crossing_bad()
{
	cout << endl;
	cout << computer.clock.now() << endl;
	miracleGrue("miracle.config",
				"./test_cases/slicerCupTestCase/stls/Cathedral_Crossing.stl");
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}

void SlicerCupTestCase::testCathedral_Crossing_fixed()
{
	cout << endl;
	cout << computer.clock.now() << endl;
	miracleGrue("miracle.config",
				"./test_cases/slicerCupTestCase/stls/Cathedral_Crossing_fixed.stl");
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}
