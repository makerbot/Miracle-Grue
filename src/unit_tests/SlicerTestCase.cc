#include <fstream>

#include <cppunit/config/SourcePrefix.h>
#include "SlicerTestCase.h"

#include "../Configuration.h"
#include "../SliceOperation.h"

CPPUNIT_TEST_SUITE_REGISTRATION( SlicerTestCase );





void SlicerTestCase::setUp()
{

}

void SlicerTestCase::example()
{

	SliceOperation slicer;
	MeshData inputData;
	RegionData outputData(0.2, 0.4);

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




