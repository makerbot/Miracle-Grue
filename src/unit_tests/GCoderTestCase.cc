#include <fstream>

#include <cppunit/config/SourcePrefix.h>
#include "GCoderTestCase.h"

#include "../Configuration.h"
#include "../GCoderOperation.h"
#include "../PathData.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( GCoderTestCase );

#define SINGLE_EXTRUDER_FILE_NAME "v29_single_xtruder_warmup.gcode"
#define DUAL_EXTRUDER_FILE_NAME "v29_dual_xtruder_warmup.gcode"
#define SINGLE_EXTRUDER_WITH_PATH "v29_single_xtruder_with_path.gcode"

void configureSingleExtruder(Configuration& config)
{
	config.machineName = "TOM";
	config.firmware ="v2.9";

	Extruder e;
	config.extruders.push_back(e);
	config.extruders[0].defaultSpeed = 6;
	config.extruders[0].extrusionTemperature = 200;
	config.extruders[0].coordinateSystemOffsetX = 0;

	config.platform.temperature = 30;

}

void configureDualExtruder(Configuration& config)
{
	config.machineName = "TOM";
	config.firmware ="v2.9";

	Extruder e;
	config.extruders.push_back(e);
	config.extruders[0].defaultSpeed = 6;
	config.extruders[0].extrusionTemperature = 200;
	config.extruders[0].coordinateSystemOffsetX = 0;

	config.platform.temperature = 30;

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

void empty_tool_chain(Configuration &config)
{
	CPPUNIT_ASSERT(config.extruders.size()==1);
	GCoderOperation &tooler = *new GCoderOperation();

	tooler.init(config);

	tooler.start();
	tooler.finish();


	delete &tooler;
}

void path_tool_chain(Configuration &config, const DataEnvelope &d)
{
	CPPUNIT_ASSERT(config.extruders.size()==1);
	GCoderOperation &tooler = *new GCoderOperation();

	tooler.init(config);
	tooler.start();

	tooler.collect(d);

	tooler.finish();


	delete &tooler;
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

	empty_tool_chain(config);

	// verify that gcode file has been generated
	CPPUNIT_ASSERT( ifstream(SINGLE_EXTRUDER_FILE_NAME) );
}

void GCoderTestCase::dualExtruders()
{
	Configuration config;
	config.gcodeFilename = DUAL_EXTRUDER_FILE_NAME;

	configureDualExtruder(config);
	empty_tool_chain(config);

	CPPUNIT_ASSERT( ifstream(DUAL_EXTRUDER_FILE_NAME) );

}

//
// 	This tests generates gcode for a simple rectangular path.
//
void GCoderTestCase::simplePath()
{
	Configuration config;
	config.gcodeFilename = SINGLE_EXTRUDER_WITH_PATH;

	configureSingleExtruder(config);
	PathData &d = *new PathData();
	d.setLast();


	d.extrusionPaths.push_back(Paths());

	d.extrusionPaths[0].push_back(Polygon());
	Polygon &poly = d.extrusionPaths[0][0];

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

	path_tool_chain(config, d);

	cout << "totot" << endl;
	delete &d;
	// CPPUNIT_ASSERT(0==4);
	// verify that gcode file has been generated
	CPPUNIT_ASSERT( ifstream(SINGLE_EXTRUDER_WITH_PATH) );
}

