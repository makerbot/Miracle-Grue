#include "UnitTestUtils.h"
#include "GCoderRefactorTestCase.h"
#include "../mgl/gcoder.h"
#include "../mgl/pather.h"
#include "../mgl/mgl.h"
#include "../mgl/configuration.h"

#include <vector>
#include <list>

CPPUNIT_TEST_SUITE_REGISTRATION( GCoderRefactorTestCase );

using namespace std;
using namespace libthing;
using namespace mgl;

string inputsDir;

void GCoderRefactorTestCase::setUp() {
	cout << "\nSetting up inputs path" << endl;
	MyComputer computer;
	char pathsep = computer.fileSystem.getPathSeparatorCharacter();
	inputsDir = string("inputs") + pathsep;
}

void GCoderRefactorTestCase::testPathGcode() {
	//make a canned Polygon and equivalent open path
	//output both to gcode
	//???
	//profit
	vector<SliceData> slices;
	SliceData slice;
	ExtruderSlice oneExSlice;
	mgl::Polygon onePoly;
	
	LayerPaths layerpaths;
	LayerPaths::Layer oneLayer;
	LayerPaths::Layer::ExtruderLayer oneExtruder;
	OpenPath onePath;
	Loop oneLoop;
	
	//Make a square
	onePoly.push_back(PointType(0.0, 0.0));
	onePoly.push_back(PointType(1.0, 0.0));
	onePoly.push_back(PointType(1.0, 1.0));
	onePoly.push_back(PointType(0.0, 1.0));
	onePoly.push_back(PointType(0.0, 0.0));
	//Same OpenPath
	onePath.appendPoint(PointType(0.0, 0.0));
	onePath.appendPoint(PointType(1.0, 0.0));
	onePath.appendPoint(PointType(1.0, 1.0));
	onePath.appendPoint(PointType(0.0, 1.0));
	
	
	cout << "Test that OpenPath and Polygon behave identically" << endl;
	cout << "Polygon \tOpenPath \tLoop" << endl;
	mgl::Polygon::const_iterator piter;
	OpenPath::const_iterator opiter;
	Loop::cw_iterator liter = oneLoop.clockwise();
	
	for(piter = onePoly.begin(), opiter = onePath.fromStart(); 
			piter != onePoly.end() && opiter != onePath.end(); 
			++piter, ++opiter) {
		liter = oneLoop.insertPoint(*opiter, liter);
		cout << *piter << "  \t" << *opiter << "  \t" << *liter << endl;
		CPPUNIT_ASSERT_EQUAL(*piter, *opiter);
	}
	//construct the data structures
	//lowest layer: boundary
	oneExSlice.boundary.push_back(onePoly);
	LoopPath lp(oneLoop, oneLoop.clockwise(),
				oneLoop.counterClockwise());
	oneExtruder.outlinePaths.push_back(lp);
	//next layer: slice/layer
	slice.extruderSlices.push_back(oneExSlice);
	oneLayer.extruders.push_back(oneExtruder);
	//ancillary slice setup
	slice.updatePosition(oneLayer.layerHeight, oneLayer.layerId);
	//next layer: all the things
	slices.push_back(slice);
	layerpaths.push_back(oneLayer);
	GCoderConfig gcoderCfg;
	
	Configuration config;
	config.readFromDefault();
	loadGCoderConfigFromFile(config, gcoderCfg);
	LayerMeasure layermeasure(layerpaths.begin()->layerZ, 
			layerpaths.begin()->layerHeight);
	
	gcoderCfg.doInfills = false;
	gcoderCfg.doInfillsFirst = false;
	gcoderCfg.doInsets = false;
	gcoderCfg.doOutlines = true;
	gcoderCfg.doPrintLayerMessages = false;
	
	GCoder gcoder(gcoderCfg, NULL);
	
	stringstream expected, actual;
	try {
		gcoder.writeGcodeFile(slices, layermeasure, expected, "Title");
		gcoder.writeGcodeFile(layerpaths, layermeasure, actual, "Title");
	} catch (Exception mixup) {
		CPPUNIT_FAIL(mixup.error);
	}
	
	cout << "\n\nExpected: " << endl;
	cout << expected.str() << endl << endl;
	cout << "\n\nActual: " << endl;
	cout << actual.str() << endl << endl;
	
	
}



