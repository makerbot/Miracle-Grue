
#ifndef SLICER_TEST_CASE_H
#define SLICER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class SlicerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( SlicerTestCase );
  CPPUNIT_TEST( example );
  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:

  void example();

};


#endif
