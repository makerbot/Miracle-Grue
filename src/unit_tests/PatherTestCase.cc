#include <fstream>

#include <cppunit/config/SourcePrefix.h>
#include "PatherTestCase.h"

#include "../Configuration.h"
#include "../PatherOperation.h"

CPPUNIT_TEST_SUITE_REGISTRATION( PatherTestCase );

#define SINGLE_EXTRUDER_FILE_NAME "v29_single_xtruder_warmup.gcode"
#define DUAL_EXTRUDER_FILE_NAME "v29_dual_xtruder_warmup.gcode"

void configurePathTest(Configuration& config)
{
	config.gcodeFilename = SINGLE_EXTRUDER_FILE_NAME;
	config.machineName = "TOM";
	config.firmware ="v2.9";

	Extruder e;
	config.extruders.push_back(e);
	config.extruders[0].defaultSpeed = 6;
	config.extruders[0].extrusionTemperature = 200;
	config.extruders[0].coordinateSystemOffsetX = 0;

	config.platform.temperature = 30;

}





void PatherTestCase::setUp()
{

}

void PatherTestCase::example()
{

//  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, 1.1, 0.05 );
//  CPPUNIT_ASSERT( 1 == 0 );
  CPPUNIT_ASSERT( 1 == 1 );

  long* l1 = new long(12);
  long* l2 = new long(12);

  CPPUNIT_ASSERT_EQUAL( 12, 12 );
  CPPUNIT_ASSERT_EQUAL( 12L, 12L );
  CPPUNIT_ASSERT_EQUAL( *l1, *l2 );

  delete l1;
  delete l2;

  CPPUNIT_ASSERT( 12L == 12L );
//  CPPUNIT_ASSERT_EQUAL( 12, 13 );
//  CPPUNIT_ASSERT_DOUBLES_EQUAL( 12.0, 11.99, 0.5 );

}




