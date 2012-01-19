
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
    std::vector<TubesInSlice> allTubes;
    string scadFile("./test_cases/slicerCupTestCase/");
    scadFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".scad");
    string gcodeFile("./test_cases/slicerCupTestCase/");
    gcodeFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile), ".gcode");

    cout << "Slicing the model" << endl;
    sliceAndPath(mesh, config["slicer"]["layerW"].asDouble(), config["slicer"]["tubeSpacing"].asDouble(), config["slicer"]["angle"].asDouble(), scadFile.c_str(), allTubes);

    cout << "Writing the gcode to \"" << gcodeFile << "\""<< endl;
    writeGcodeFile(config, gcodeFile.c_str(), allTubes);
}

void SlicerCupTestCase::testIndividuals()
{
	string configFileName("miracle.config");
	vector<string> models;

	models.push_back("./slicer_cup/3D_Knot.stl");
	models.push_back("./slicer_cup/Hollow_Pyramid.stl");
	models.push_back("./slicer_cup/linkCup.stl");
	models.push_back("./slicer_cup/ultimate_calibration_test.stl");
	models.push_back("./slicer_cup/Cathedral_Crossing.stl");

	for(int i=0; i< models.size(); i++)
	{
		cout << endl;
		cout << endl;
		cout << endl;
		cout << "********************" << endl;
		string modelFile = models[i];
		cout << endl;
		cout << modelFile << endl;
		cout << computer.clock.now() << endl;
		miracleGrue(configFileName.c_str(), modelFile.c_str());
		cout << computer.clock.now() << endl;
		cout << "DONE!" << endl;
	}
}

void SlicerCupTestCase::testAllTogeter()
{
	string configFileName("miracle.config");
	vector<string> models;

	models.push_back("./slicer_cup/all_together.stl");

	for(int i=0; i< models.size(); i++)
	{
		string modelFile = models[i];
		cout << endl;
		cout << modelFile << endl;
		cout << computer.clock.now() << endl;
		miracleGrue(configFileName.c_str(), modelFile.c_str());
	}
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;

}

void SlicerCupTestCase::testCathedral_Crossing()
{
	string configFileName("miracle.config");
	vector<string> models;

	models.push_back("./slicer_cup/Cathedral_Crossing.stl");

	for(int i=0; i< models.size(); i++)
	{

		string modelFile = models[i];
		cout << endl;
		cout << modelFile << endl;
		cout << computer.clock.now() << endl;
		miracleGrue(configFileName.c_str(), modelFile.c_str());
	}
	cout << computer.clock.now() << endl;
	cout << "DONE!" << endl;
}

