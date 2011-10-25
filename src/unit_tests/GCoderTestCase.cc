#include <fstream>

#include <cppunit/config/SourcePrefix.h>
#include "GCoderTestCase.h"

#include "../Configuration.h"
#include "../GCoderOperation.h"
#include "../FileWriterOperation.h"

#include "../PathData.h"
#include "../GCodeData.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( GCoderTestCase );

#define SINGLE_EXTRUDER_FILE_NAME "v29_single_xtruder_warmup.gcode"
#define DUAL_EXTRUDER_FILE_NAME "v29_dual_xtruder_warmup.gcode"
#define SINGLE_EXTRUDER_WITH_PATH "v29_single_xtruder_with_path.gcode"


void configureTOM(Configuration& config, bool automaticBuildPlatform, double platformTemp )
{
	config.machineName = "TOM";
	config.firmware ="v2.9";

	config.platform.temperature = platformTemp;
	config.platform.automated = automaticBuildPlatform;
	config.platform.waitingPositionX = 52.0;
	config.platform.waitingPositionY = -57.0;
	config.platform.waitingPositionZ = 10.0;

	config.fastFeed = 3300;
}

// fills a configuration object with the data
// for a single extruder
void configureExtruder(Configuration& config, double temperature, double speed, double offsetX)
{
	Extruder e;

	e.defaultSpeed = speed;
	e.extrusionTemperature = temperature;
	e.coordinateSystemOffsetX = offsetX;

	config.extruders.push_back(e);
}

void configureSingleExtruder(Configuration &config)
{
	configureTOM(config, true, 110);
	configureExtruder(config, 220, 6, 0);

}

// fills a configuration object with data for 2 extruders
void configureDualExtruder(Configuration& config)
{
	configureTOM(config, true, 110);
	configureExtruder(config, 220, 6, 0);
	configureExtruder(config, 220, 6, 0);
}




void GCoderTestCase::setUp()
{

}

void GCoderTestCase::example()
{
	/*
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
  CPPUNIT_ASSERT( 1 == 0 );
  CPPUNIT_ASSERT( 1 == 1 );

  long* l1 = new long(12);
  long* l2 = new long(12);

  CPPUNIT_ASSERT_EQUAL( 12, 12 );
  CPPUNIT_ASSERT_EQUAL( 12L, 12L );
  CPPUNIT_ASSERT_EQUAL( *l1, *l2 );

  delete l1;
  delete l2;

  CPPUNIT_ASSERT( 12L == 12L );
  CPPUNIT_ASSERT_EQUAL( 12, 13 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 12.0, 11.99, 0.5 );
*/
}

void simple_tool_chain(Configuration &config)
{
	GCoderOperation &tooler = *new GCoderOperation();
	FileWriterOperation &fileWriter = *new FileWriterOperation();

	vector<Operation*> empty;

	vector<Operation*> tool;
	tool.push_back(&tooler);

	vector<Operation*> writer;
	writer.push_back(&fileWriter);

	tooler.init(config, empty, writer);
	fileWriter.init(config, tool, empty);

	fileWriter.start();
	tooler.start();

	tooler.finish();
	fileWriter.finish();

	delete &tooler;
	delete &fileWriter;
}

void path_tool_chain(Configuration &config, const DataEnvelope &d)
{

	GCoderOperation &tooler = *new GCoderOperation();
	FileWriterOperation &fileWriter = *new FileWriterOperation();

	vector<Operation*> empty;

	vector<Operation*> tool;
	tool.push_back(&tooler);

	vector<Operation*> writer;
	writer.push_back(&fileWriter);

	tooler.init(config, empty, writer);
	fileWriter.init(config, tool, empty);

	fileWriter.start();
	tooler.start();

	tooler.collect(d);

	tooler.finish();
	fileWriter.finish();

	delete &tooler;
	delete &fileWriter;
}

// a function that adds 4 points to a polygon within the list paths for
// a new extruder.
void initSimplePath(PathData &d)
{
	d.setLast();
	d.paths.push_back(Paths());
	d.paths[0].push_back(Polygon());
	Polygon &poly = d.paths[0][0];

	double lower_x = 10;
	double lower_y = 10;
	double dx = 50;
	double dy = 30;


	Point2D p0(lower_x, lower_y);
	Point2D p1(p0.x, p0.y + dy);
	Point2D p2(p1.x + dx, p1.y);
	Point2D p3(p2.x, p2.y - dy);

	poly.push_back(p0);
	poly.push_back(p1);
	poly.push_back(p2);
	poly.push_back(p3);

}



//
// This test creates a gcode file for single extruder machine
// The file contains code to home the tool and heat the extruder/platform
//
void GCoderTestCase::singleExtruder()
{
	Configuration config;
	config.gcodeFilename = SINGLE_EXTRUDER_FILE_NAME;

	configureSingleExtruder(config);
	CPPUNIT_ASSERT_EQUAL((size_t)1, config.extruders.size());

	cout << " YYY" << endl;
	simple_tool_chain(config);
	cout << " ZZZ" << endl;
	// verify that gcode file has been generated
	CPPUNIT_ASSERT( ifstream(SINGLE_EXTRUDER_FILE_NAME) );
}

//
// This test creates a gcode file for a dual extruder machine
//
void GCoderTestCase::dualExtruders()
{
	// cerate an empty configuration object
	Configuration config;
	// set the output fie
	config.gcodeFilename = DUAL_EXTRUDER_FILE_NAME;
	// add extruder information
	configureDualExtruder(config);
	CPPUNIT_ASSERT_EQUAL((size_t)2,config.extruders.size());
	// create a simple Gcode operation (no paths), initialize it and run it
	simple_tool_chain(config);

	CPPUNIT_ASSERT( ifstream(DUAL_EXTRUDER_FILE_NAME) );

}


//
// 	This tests generates gcode for a simple rectangular path.
//
void GCoderTestCase::simplePath()
{
	// create empty configuration and set the file name
	Configuration config;
	config.gcodeFilename = SINGLE_EXTRUDER_WITH_PATH;

	// load 1 extruder
	configureSingleExtruder(config);
	CPPUNIT_ASSERT_EQUAL((size_t)1, config.extruders.size());

	// create a path message as if received by a pather operation
	PathData &d = *new PathData(0.2, 0.4);
	// add a simple rectangular path for the single extruder
	initSimplePath(d);

	// instaniate a gcoder and send it the path as an envelope.
	path_tool_chain(config, d);


	// cleanup the data
	delete &d;

	// verify that gcode file has been generated
	CPPUNIT_ASSERT( ifstream(SINGLE_EXTRUDER_WITH_PATH) );
}

