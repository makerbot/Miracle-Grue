#include <fstream>

#include <cppunit/config/SourcePrefix.h>
#include "RegionerTestCase.h"

#include "../Configuration.h"
#include "../RegionerOperation.h"

CPPUNIT_TEST_SUITE_REGISTRATION( RegionerTestCase );

#define SINGLE_EXTRUDER_FILE_NAME "v29_single_xtruder_warmup.gcode"
#define DUAL_EXTRUDER_FILE_NAME "v29_dual_xtruder_warmup.gcode"

void configurePathTest(Configuration& config)
{
	config["FileWriterOperation"]["filename"]= SINGLE_EXTRUDER_FILE_NAME;
	config["FileWriterOperation"]["format"]= ".gcode";
}

void RegionerTestCase::setUp()
{

}

void RegionerTestCase::example()
{

	RegionerOperation operation;
	SliceData *inputData = new SliceData(0.2, 0.4);
	RegionData *outputData = new RegionData(0.2, 0.4);

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
  inputData->release();
  outputData->release();

  CPPUNIT_ASSERT( 12L == 12L );
//  CPPUNIT_ASSERT_EQUAL( 12, 13 );
//  CPPUNIT_ASSERT_DOUBLES_EQUAL( 12.0, 11.99, 0.5 );

}




