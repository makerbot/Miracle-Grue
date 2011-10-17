#include <cppunit/config/SourcePrefix.h>
#include "GCoderTestCase.h"

#include "../Configuration.h"
#include "../GCoderOperation.h"

CPPUNIT_TEST_SUITE_REGISTRATION( GCoderTestCase );


void configureSingleExtruder(Configuration& config)
{
	config.machineName = "TOM";
	config.firmware ="v2.9";

	Extruder e;
	config.extruders.push_back(e);
	config.extruders[0].defaultSpeed = 6;
	config.extruders[0].extrusionTemperature = 200;
	config.extruders[0].coordinateSystemOffsetX = 0;

}


void GCoderTestCase::setUp()
{

}

void GCoderTestCase::example()
{
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
}


void GCoderTestCase::singleExtruder()
{
	CPPUNIT_ASSERT (2 == 2);
	Configuration config;

	configureSingleExtruder(config);
	GCoderOperation tooler;

	tooler.init(config);
	tooler.start();

	DataEnvelope d = DataEnvelope();
	d.typeID =  TYPE_PATH_ASCII;
	d.setLast();
	tooler.collect(d);

}

void GCoderTestCase::dualExtruders()
{
  double result = 6;
  CPPUNIT_ASSERT( result == 6.0 );
}


