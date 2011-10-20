#ifndef GCODER_TEST_CASE_H_
#define GCODER_TEST_CASE_H_

#include <cppunit/extensions/HelperMacros.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class GCoderTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( GCoderTestCase );
  CPPUNIT_TEST( singleExtruder );
  CPPUNIT_TEST( dualExtruders );
  CPPUNIT_TEST( simplePath );
  CPPUNIT_TEST( example );
  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:
  void singleExtruder();
  void dualExtruders();
  void simplePath();

  void example();

};


#endif

