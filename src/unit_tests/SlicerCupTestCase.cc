
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



//void miracleGrue(const char *configFilePath,
//					const char *modelFilePath,
//					const char *scadFile,
//					const char *gcodeFile)
//
//{
//	string configFileName(configFilePath);
//
//    Configuration config;
//    config.readFromFile(configFileName.c_str());
//
////    cout << config.asJson() << endl;
//
//	GCoder gcoder;
//	loadGCoderData(config, gcoder);
//
//	Slicer slicer;
//	loadSlicerDaTa(config, slicer);
//
//	Meshy mesh(slicer.firstLayerZ, slicer.layerH); // 0.35
//	loadMeshyFromStl(mesh, modelFilePath);
//
//	unsigned int sliceCount = mesh.readSliceTable().size();
//	unsigned int extruderId = 0;
//
//	Slicy slicy(mesh.readAllTriangles(), mesh.readLimits(), slicer.layerW, slicer.layerH, sliceCount, scadFile);
//
//	std::vector< SliceData >  slices;
//	slices.reserve( mesh.readSliceTable().size());
//
//	Scalar cuttOffLength = slicer.insetCuttOffMultiplier * slicer.layerW;
//
//	ProgressBar progress(sliceCount);
//	cout << "Slicing" << endl;
//
//    std::ofstream gout(gcodeFile);
//    gcoder.writeStartOfFile(gout);
//
//	for(unsigned int sliceId=0; sliceId < sliceCount; sliceId++)
//	{
//		progress.tick();
//        cout.flush();
//		const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
//		Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(sliceId);
//		Scalar sliceAngle = sliceId * slicer.angle;
//		slices.push_back( SliceData(z,sliceId));
//		SliceData &slice = slices[sliceId];
//
//		bool hazNewPaths = slicy.slice( trianglesForSlice,
//										z,
//										sliceId,
//										extruderId,
//										slicer.tubeSpacing,
//										sliceAngle,
//										slicer.nbOfShells,
//										cuttOffLength,
//										slicer.infillShrinkingMultiplier,
//										slicer.insetDistanceMultiplier,
//										slice);
//		// cout << slice;
//		if(hazNewPaths)
//		{
//			gcoder.writeSlice(gout, slice);
//		}
//		else
//		{
//	    	cout << "WARNING: Layer " << sliceId << " has no outline!" << endl;
////			slices.pop_back();
//		}
//	}
//    gout.close();
//
//    cout << endl << computer.clock.now() << endl;
//    cout << "Done!" << endl;
//}


void SlicerCupTestCase::testIndividuals()
{


	string stlDir("./test_cases/slicerCupTestCase/stls/");

	vector<string> models;
	models.push_back(stlDir + "3D_Knot.stl");
	models.push_back(stlDir + "Hollow_Pyramid.stl");
	models.push_back(stlDir + "linkCup.stl");
	models.push_back(stlDir + "ultimate_calibration_test.stl");
	models.push_back(stlDir + "Cathedral_Crossing_fixed.stl");

	string outputDirectory ("./test_cases/slicerCupTestCase/output/");

	string configFileName("miracle.config");
	Configuration config;
	config.readFromFile(configFileName.c_str());

	for(int i=0; i< models.size(); i++)
	{
		string modelFile = models[i];
		string scadFile(outputDirectory);
		scadFile += computer.fileSystem.ChangeExtension(computer.fileSystem.ExtractFilename(modelFile.c_str()).c_str(), ".scad");
		string gcodeFile(outputDirectory);
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
		loadSlicerDaTa(config, slicer);

		miracleGrue(	gcoder,
						slicer,
						modelFile.c_str(),
						scadFile.c_str(),
						gcodeFile.c_str());

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
	loadSlicerDaTa(config, slicer);

	cout << endl;
	cout << computer.clock.now() << endl;
	miracleGrue(gcoder, slicer,
				"./test_cases/slicerCupTestCase/stls/all_together.stl",
				"./test_cases/slicerCupTestCase/output/all_together.scad",
				"./test_cases/slicerCupTestCase/output/all_together.gcode");
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
	loadSlicerDaTa(config, slicer);

	cout << endl;
	cout << computer.clock.now() << endl;
	miracleGrue(gcoder, slicer,
				"./test_cases/slicerCupTestCase/stls/Cathedral_Crossing.stl",
				"./test_cases/slicerCupTestCase/output/Cathedral_Crossing.scad",
				"./test_cases/slicerCupTestCase/output/Cathedral_Crossing.gcode");
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
	loadSlicerDaTa(config, slicer);

	cout << endl;
	cout << computer.clock.now() << endl;
	miracleGrue(gcoder, slicer,
				"./test_cases/slicerCupTestCase/stls/Cathedral_Crossing_fixed.stl",
				"./test_cases/slicerCupTestCase/output/Cathedral_Crossing_fixed.scad",
				"./test_cases/slicerCupTestCase/output/Cathedral_Crossing_fixed.gcode");
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
	loadSlicerDaTa(config, slicer);

	cout << "Slumping: full head.stl" << endl;
	miracleGrue(gcoder, slicer,
				"./test_cases/specific_issues/slumping/full head.stl",
				"./test_cases/specific_issues/output/full head.scad",
				"./test_cases/specific_issues/output/full head.gcode");

	cout << "Slumping: half head.stl" << endl;
	miracleGrue(gcoder, slicer,
				"./test_cases/specific_issues/slumping/half head.stl",
				"./test_cases/specific_issues/output/half head.scad",
				"./test_cases/specific_issues/output/half head.gcode");

	cout << "Insetting: holy_cube.stl" << endl;
	miracleGrue(gcoder, slicer,
				"./test_cases/specific_issues/insetting/holy_cube.stl",
				"./test_cases/specific_issues/output/holy_cube.scad",
				"./test_cases/specific_issues/output/holy_cube.gcode");
}



