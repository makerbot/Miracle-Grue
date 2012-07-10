#include <fstream>
#include <cstdlib>
#include <cppunit/config/SourcePrefix.h>

#include "UnitTestUtils.h"

#include "GCoderTestCase.h"


#include "mgl/abstractable.h"
#include "mgl/meshy.h"
#include "mgl/configuration.h"
#include "mgl/gcoder.h"
#include "mgl/abstractable.h"

#include <sys/stat.h>
#include <unistd.h>
#include <list>

using namespace std;
using namespace mgl;
using namespace libthing;



CPPUNIT_TEST_SUITE_REGISTRATION(GCoderTestCase);

#ifdef WIN32

static const string testdir = "outputs\\test_cases\\GCoderTestCase";

#define SINGLE_EXTRUDER_CONFIG "outputs\\test_cases\\GCoderTestCase\\single_xtruder.config"
#define SINGLE_EXTRUDER_FILE_NAME "outputs\\test_cases\\GCoderTestCase\\single_xtruder_warmup.gcode"
#define DUAL_EXTRUDER_FILE_NAME "outputs\\test_cases\\GCoderTestCase\\dual_xtruder_warmup.gcode"
#define SINGLE_EXTRUDER_WITH_PATH "outputs\\test_cases\\GCoderTestCase\\single_xtruder_with_path.gcode"
#define SINGLE_EXTRUDER_GRID_PATH "outputs\\test_cases\\GCoderTestCase\\single_xtruder_grid_path.gcode"
#define SINGLE_EXTRUDER_MULTI_GRID_PATH "outputs\\test_cases\\GCoderTestCase\\single_xtruder_multigrid_path.gcode"
#define SINGLE_EXTRUDER_KNOT "outputs\\test_cases\\GCoderTestCase\\knot.gcode"
#else

static const string testdir = "outputs/test_cases/GCoderTestCase";

#define SINGLE_EXTRUDER_CONFIG "outputs/test_cases/GCoderTestCase/single_xtruder.config"
#define SINGLE_EXTRUDER_FILE_NAME "outputs/test_cases/GCoderTestCase/single_xtruder_warmup.gcode"
#define DUAL_EXTRUDER_FILE_NAME "outputs/test_cases/GCoderTestCase/dual_xtruder_warmup.gcode"
#define SINGLE_EXTRUDER_WITH_PATH "outputs/test_cases/GCoderTestCase/single_xtruder_with_path.gcode"
#define SINGLE_EXTRUDER_GRID_PATH "outputs/test_cases/GCoderTestCase/single_xtruder_grid_path.gcode"
#define SINGLE_EXTRUDER_MULTI_GRID_PATH "outputs/test_cases/GCoderTestCase/single_xtruder_multigrid_path.gcode"
#define SINGLE_EXTRUDER_KNOT "outputs/test_cases/GCoderTestCase/knot.gcode"
#endif


string outputDir("outputs/test_cases/GCoderTestCase/");

void configurePlatform(Configuration& config, bool automaticBuildPlatform, double platformTemp) {
	std::cout << "Starting:" << __FUNCTION__ << endl;
	config["scalingFactor"] = 1.0;
	config["platform"]["temperature"] = platformTemp;
	config["platform"]["automated"] = automaticBuildPlatform;
	//	config["platform"]["waitingPositionX"] = 52.0;
	//	config["platform"]["waitingPositionY"] = -57.0;
	//	config["platform"]["waitingPositionZ"] = 10.0;
	std::cout << "Exiting:" << __FUNCTION__ << endl;

}

// fills a configuration object with the data
// for a single extruder

void configureExtruder(Configuration& config, double temperature, double speed, double offsetX) {
	Json::Value extruder;
	std::cout << "Starting:" << __FUNCTION__ << endl;
	extruder["leadIn"] = 0.25;
	extruder["leadOut"] = 0.35;
	extruder["defaultExtrusionSpeed"] = speed;
	extruder["extrusionTemperature"] = temperature;
	extruder["coordinateSystemOffsetX"] = offsetX;
	extruder["slowFeedRate"] = 1800;
	extruder["slowExtrusionSpeed"] = 4.47;
	extruder["fastFeedRate"] = 3000;
	extruder["fastExtrusionSpeed"] = 4.47;
	extruder["nozzleZ"] = 0.0;
	extruder["reversalExtrusionSpeed"] = 35.0;
	config["extruders"].append(extruder);
	std::cout << "Exiting:" << __FUNCTION__ << endl;
}

void configureSlicer(Configuration &config) {
	config["slicer"]["firstLayerZ"] = 0.11;

	config["slicer"]["layerH"] = 0.35;
	config["slicer"]["layerW"] = 0.7;
	config["slicer"]["tubeSpacing"] = 0.8;
	config["slicer"]["angle"] = M_PI / 2;

}

void configureSingleExtruder(Configuration &config) {
	std::cout << "Starting:" << __FUNCTION__ << endl;
	configurePlatform(config, true, 110);
	configureExtruder(config, 220, 6, 0);
	std::cout << "Exiting:" << __FUNCTION__ << endl;
}

// fills a configuration object with data for 2 extruders

void configureDualExtruder(Configuration& config) {
	std::cout << "Starting:" << __FUNCTION__ << endl;
	configurePlatform(config, true, 110);
	configureExtruder(config, 220, 6, 0);
	configureExtruder(config, 220, 6, 0);
	std::cout << "Exiting:" << __FUNCTION__ << endl;
	CPPUNIT_ASSERT(config["extruders"].size() == 2);
}

void GCoderTestCase::setUp() {
	std::cout << "Setup for :" << __FUNCTION__ << endl;
	MyComputer computer;

	computer.fileSystem.guarenteeDirectoryExistsRecursive(testdir.c_str());

	mkDebugPath(outputDir.c_str());
	std::cout << "Setup for :" << __FUNCTION__ << " Done" << endl;
}

void rectangle(mgl::Polygon& poly, double lower_x, double lower_y, double dx, double dy) {
	Vector2 p0(lower_x, lower_y);
	Vector2 p1(p0.x, p0.y + dy);
	Vector2 p2(p1.x + dx, p1.y);
	Vector2 p3(p2.x, p2.y - dy);

	poly.push_back(p0);
	poly.push_back(p1);
	poly.push_back(p2);
	poly.push_back(p3);
	poly.push_back(p0);
}


// a function that adds 4 points to a polygon within the list paths for
// a new extruder.

void initSimplePath(SliceData &d) {
	std::cout << "Starting:" << __FUNCTION__ << endl;
	d.extruderSlices.push_back(ExtruderSlice());

	unsigned int last = d.extruderSlices.size() - 1;
	Polygons &polys = d.extruderSlices[last].infills;


	for (int i = 0; i < 4; i++) {
		polys.push_back(mgl::Polygon());
		size_t index = polys.size() - 1;
		mgl::Polygon &poly = polys[index];

		double lower_x = -40 + 20 * i;
		double lower_y = -30;
		double dx = 10;
		double dy = 40;

		// randomize
		lower_x += 10.0 * ((double) rand()) / RAND_MAX;
		lower_y += 10.0 * ((double) rand()) / RAND_MAX;
		rectangle(poly, lower_x, lower_y, dx, dy);
	}
	std::cout << "Exiting:" << __FUNCTION__ << endl;
}

//
// This test creates a gcode file for single extruder machine
// The file contains code to home the tool and heat the extruder/platform
//

void GCoderTestCase::testSingleExtruder() {
	std::cout << "Starting:" << __FUNCTION__ << endl;
	Configuration config;

	configureSingleExtruder(config);
	//	CPPUNIT_ASSERT_EQUAL((size_t)1, config.extruders.size());

	Json::StyledWriter w;
	string confstr = w.write(config.root);
	cout << confstr << endl;

	GCoderConfig cfg;
	cfg.extruders.push_back(Extruder());

	GCoder gcoder(cfg);


	std::ofstream gout(SINGLE_EXTRUDER_FILE_NAME, std::ios::out);
	CPPUNIT_ASSERT(gout);

	gcoder.writeStartDotGCode(gout, SINGLE_EXTRUDER_FILE_NAME);
	gcoder.writeEndDotGCode(gout);

	// verify that gcode file has been generatedperror();
	ifstream sefn(SINGLE_EXTRUDER_FILE_NAME);
	CPPUNIT_ASSERT(sefn);
	std::cout << "Exiting:" << __FUNCTION__ << endl;
}

//
// This test creates a gcode file for a dual extruder machine
//

void GCoderTestCase::testDualExtruders() {
	std::cout << "Starting:" << __FUNCTION__ << endl;
	// create an empty configuration object
	Configuration config;
	// set the output fie

	// add extruder information
	configureDualExtruder(config);
	CPPUNIT_ASSERT_EQUAL((size_t) 2, (size_t) config["extruders"].size());
	// create a simple Gcode operation (no paths), initialize it and run it

	std::ofstream gout(DUAL_EXTRUDER_FILE_NAME);

	GCoderConfig gcoderCfg;
	gcoderCfg.extruders.push_back(Extruder());
	gcoderCfg.extruders.push_back(Extruder());
	//	loadGCoderData(config, gcoder);
	GCoder gcoder(gcoderCfg);

	gcoder.writeStartDotGCode(gout, DUAL_EXTRUDER_FILE_NAME);
	dbg__
	gcoder.writeEndDotGCode(gout);
	dbg__
	CPPUNIT_ASSERT(ifstream(DUAL_EXTRUDER_FILE_NAME));
	dbg__
	std::cout << "Exiting:" << __FUNCTION__ << endl;
}


//
// 	This tests generates gcode for a simple rectangular path.
//

void GCoderTestCase::testSimplePath() {
	std::cout << "Starting:" << __FUNCTION__ << endl;
	// create empty configuration and set the file name
	//Configuration config;
	// load 1 extruder
	//configureSingleExtruder(config);
	//	CPPUNIT_ASSERT_EQUAL((size_t)1, config.extruders.size());
	// create a path message as if received by a pather operation

	SliceData path;
	// add a simple rectangular path for the single extruder
	initSimplePath(path);
	std::vector<SliceData> slices;
	slices.push_back(path);
	// instaniate a gcoder and send it the path as an envelope.

	std::ofstream gout(SINGLE_EXTRUDER_WITH_PATH);

	GCoderConfig gcoderCfg;
	gcoderCfg.extruders.push_back(Extruder());

	GCoder gcoder(gcoderCfg);

	gcoder.writeStartDotGCode(gout, SINGLE_EXTRUDER_WITH_PATH);
	gcoder.writeEndDotGCode(gout);
	for (int i = 0; i < slices.size(); i++) {
		cout.flush();
		SliceData &slice = slices[i];
		Scalar z = 0.27 * i;
		slice.updatePosition(z, i);
		//gcoder.writeSlice(gout, slice);
	}

	gcoder.writeEndDotGCode(gout);
	gout.close();

	// verify that gcode file has been generated
	CPPUNIT_ASSERT(ifstream(SINGLE_EXTRUDER_WITH_PATH));
	std::cout << "Exiting:" << __FUNCTION__ << endl;
}

/*
void GCoderTestCase::testConfig()
{
	std::cout<< "Starting:" <<__FUNCTION__ << endl;

	Configuration conf;
	std::string p = conf.root["programName"].asString();
	cout << endl << endl << endl << "PROGRAM NAME: " << p << endl;
	CPPUNIT_ASSERT(p == "Miracle-Grue");

	configureSingleExtruder(conf);
	Json::StyledWriter w;
	string confstr = w.write(conf.root);
	cout << confstr << endl;

	CPPUNIT_ASSERT(conf.root["extruders"].isArray());
	CPPUNIT_ASSERT(conf.root["extruders"].isValidIndex(0));

	cout << "ExtruderCount " << conf.root["extruders"].size() << endl;
	GCoder single;
//	loadGCoderData(conf, single);

	cout << endl << endl << endl << "READ!" << endl;

	CPPUNIT_ASSERT(single.readExtruders().size() ==1);

	// save config for single extruder
	Configuration config;
	configureSingleExtruder(config);
	configureSlicer(config);
	string s = config.asJson();

	ofstream outfile;
	outfile.open(SINGLE_EXTRUDER_CONFIG);
	outfile << s;
	outfile.close();

	Configuration phoenix;
	phoenix.readFromFile(SINGLE_EXTRUDER_CONFIG);
	cout << "phoenix:" << phoenix.root["programName"].asString() << endl;
	CPPUNIT_ASSERT(phoenix.root["programName"] == "Miracle-Grue");

	std::cout<< "Exiting:" <<__FUNCTION__ << endl;
}
 */
void gcodeStreamFormat(ostream &ss) {
	try {
		ss.imbue(std::locale("en_US.UTF-8"));
	} catch (...) {
		ss.imbue(std::locale("C"));
	}
	ss.setf(ios_base::floatfield, ios::floatfield); // floatfield not set
	ss.precision(4);
}

void GCoderTestCase::testFloatFormat() {
	stringstream ss;
	gcodeStreamFormat(ss);

	ss << endl;
	ss << "loc: " << ss.getloc().name() << endl;

	CPPUNIT_ASSERT_EQUAL(ss.getloc().name(), string("en_US.UTF-8")); // std::string("C") );

	//locale myloc(  locale(),    // C++ default locale
	//       new WithComma);// Own numeric facet
	ss << endl;
	// ss << "LOCALE name: " << myloc.name() << endl;
	ss << "num: " << 3.1415927 << endl;
	cout << ss.str() << endl;
	std::cout << "Exiting:" << __FUNCTION__ << endl;

}

void initHorizontalGridPath(SliceData &d, 
		double lowerX, 
		double lowerY, 
		double dx, 
		double dy, 
		int lineCount) {
	d.extruderSlices.push_back(ExtruderSlice());
	Polygons &polys = d.extruderSlices[0].infills;

	bool flip = false;
	for (int i = 0; i < lineCount; i++) {
		polys.push_back(mgl::Polygon());
		size_t index = polys.size() - 1;
		mgl::Polygon &poly = polys[index];

		double y = lowerY + i * dy / lineCount;
		Vector2 p0(lowerX, y);
		Vector2 p1(p0.x + dx, y);
		if (!flip) {
			poly.push_back(p0);
			poly.push_back(p1);
		} else {
			poly.push_back(p1);
			poly.push_back(p0);
		}
		flip = !flip;
	}
}

void initHorizontalGridPath(LayerPaths::Layer &d, 
		double lowerX, 
		double lowerY, 
		double dx, 
		double dy, 
		int lineCount) {
	LayerPaths::Layer::ExtruderLayer exlayer;
	LayerPaths::Layer::ExtruderLayer::InfillList& infills = exlayer.infillPaths;

	bool flip = false;
	for (int i = 0; i < lineCount; i++) {
		
		OpenPath currentInfill;

		double y = lowerY + i * dy / lineCount;
		Vector2 p0(lowerX, y);
		Vector2 p1(p0.x + dx, y);
		if(flip)
			std::swap(p0, p1);
		currentInfill.appendPoint(p0);
		currentInfill.appendPoint(p1);
		infills.push_back(currentInfill);
		flip = !flip;
	}
	d.extruders.push_back(exlayer);
}

void initVerticalGridPath(SliceData &d, 
		double lowerX, 
		double lowerY, 
		double dx, 
		double dy, 
		int lineCount) {
	d.extruderSlices.push_back(ExtruderSlice());
	Polygons &polys = d.extruderSlices[0].infills;

	bool flip = false;

	for (int i = 0; i < lineCount; i++) {
		polys.push_back(mgl::Polygon());
		size_t index = polys.size() - 1;
		mgl::Polygon &poly = polys[index];

		double x = lowerX + i * dx / lineCount;
		Vector2 p0(x, lowerY);
		Vector2 p1(x, lowerY + dy);
		if (!flip) {
			poly.push_back(p0);
			poly.push_back(p1);
		} else {
			poly.push_back(p1);
			poly.push_back(p0);
		}
		flip = !flip;
	}
}

void initVerticalGridPath(LayerPaths::Layer &d, 
		double lowerX, 
		double lowerY, 
		double dx, 
		double dy, 
		int lineCount) {
	LayerPaths::Layer::ExtruderLayer exlayer;
	LayerPaths::Layer::ExtruderLayer::InfillList& infills = exlayer.infillPaths;

	bool flip = false;
	for (int i = 0; i < lineCount; i++) {
		
		OpenPath currentInfill;

		double x = lowerX + i * dx / lineCount;
		Vector2 p0(x, lowerY);
		Vector2 p1(x, lowerY + dy);
		if(flip)
			std::swap(p0, p1);
		currentInfill.appendPoint(p0);
		currentInfill.appendPoint(p1);
		infills.push_back(currentInfill);
		flip = !flip;
	}
	d.extruders.push_back(exlayer);
}

void GCoderTestCase::testGridPath() {
	std::cout << "Starting:" << __FUNCTION__ << endl;

	Configuration config;

	// load 1 extruder
	configureSingleExtruder(config);

	SliceData path;
	LayerPaths::Layer layer;

	srand((unsigned int) time(NULL));
	int lineCount = 20;
	double lowerX = -30 + 10.0 * ((double) rand()) / RAND_MAX;
	double lowerY = -30 + 10.0 * ((double) rand()) / RAND_MAX;

	double dx = 20.0;
	double dy = 20.0;

	initHorizontalGridPath(path, lowerX, lowerY, dx, dy, 20);
	initHorizontalGridPath(layer, lowerX, lowerY, dx, dy, 20);

	vector<SliceData> slices;
	LayerPaths layerpaths;
	
	slices.push_back(path);
	layerpaths.push_back(layer);

	std::ofstream gout(SINGLE_EXTRUDER_GRID_PATH);

	GCoderConfig gcoderCfg;
	gcoderCfg.extruders.push_back(Extruder());

	GCoder gcoder(gcoderCfg);

	gcoder.writeStartDotGCode(gout, SINGLE_EXTRUDER_GRID_PATH);
//	for (int i = 0; i < slices.size(); i++) {
//		cout.flush();
//		SliceData &slice = slices[i];
//		Scalar z = 0.27 * i;
//		slice.updatePosition(z, i);
//		gcoder.writeSlice(gout, slice);
//	}
	size_t lid = 0;
	for(LayerPaths::layer_iterator iter = layerpaths.begin(); 
			iter != layerpaths.end();
			++iter) {
		LayerPaths::Layer& currentLayer = *iter;
		Scalar z = 0.27 * lid;
		currentLayer.layerZ = z;
		currentLayer.measure_index = lid;
		gcoder.writeSlice(gout, layerpaths, iter, lid);
		++lid;
	}

	gcoder.writeEndDotGCode(gout);
	gout.close();

	CPPUNIT_ASSERT(ifstream(SINGLE_EXTRUDER_WITH_PATH));
	std::cout << "Exiting:" << __FUNCTION__ << endl;
}

int random(int start, int range) {
	int r = rand();
	r = r / (RAND_MAX / range);
	return start + r;
}

void GCoderTestCase::testMultiGrid() {
	std::cout << "Starting:" << __FUNCTION__ << endl;

	//	Configuration config;
	//
	//	// load 1 extruder
	//	configureSingleExtruder(config);

	vector<SliceData> slices;
	LayerPaths layerpaths;
	srand((unsigned int) time(NULL));

	int lineCount = 20;
	double lowerX = -35 + random(-10, 20);
	double lowerY = -35 + random(-10, 20); // 10.0 * ((double) rand()) / RAND_MAX;
	double firstLayerH = 0.11;
	double layerH = 0.35;
	bool horizontal = true;
	double dx = 20.0;
	double dy = 20.0;

	for (int currentLayer = 0; currentLayer < 200; currentLayer++) {
		SliceData path;
		LayerPaths::Layer layer;
		if (horizontal) {
			initHorizontalGridPath(path, lowerX, lowerY, dx, dy, 20);
			initHorizontalGridPath(layer, lowerX, lowerY, dx, dy, 20);
		} else {
			initVerticalGridPath(path, lowerX, lowerY, dx, dy, 20);
			initVerticalGridPath(layer, lowerX, lowerY, dx, dy, 20);
		}
		slices.push_back(path);
		layerpaths.push_back(layer);
		horizontal = !horizontal;
	}

	std::ofstream gout(SINGLE_EXTRUDER_MULTI_GRID_PATH);

	GCoderConfig gcoderCfg;
	gcoderCfg.extruders.push_back(Extruder());

	GCoder gcoder(gcoderCfg);

	// loadGCoderData(config, gcoder);
	gcoder.writeStartDotGCode(gout, SINGLE_EXTRUDER_MULTI_GRID_PATH);

//	for (int i = 0; i < slices.size(); i++) {
//		cout.flush();
//		SliceData& currentSlice = slices[i];
//		Scalar z = i * layerH + firstLayerH;
//		currentSlice.updatePosition(z, i);
//		gcoder.writeSlice(gout, currentSlice);
//	}
	size_t lid = 0;
	for (LayerPaths::layer_iterator iter = layerpaths.begin();
			iter != layerpaths.end();
			++iter) {
		LayerPaths::Layer& currentLayer = *iter;
		currentLayer.measure_index = lid;
		currentLayer.layerZ = firstLayerH + lid * layerH;
		gcoder.writeSlice(gout, layerpaths, iter, lid);
		++lid;
	}

	gcoder.writeEndDotGCode(gout);
	gout.close();



	CPPUNIT_ASSERT(ifstream(SINGLE_EXTRUDER_WITH_PATH));
	std::cout << "Exiting:" << __FUNCTION__ << endl;
}
