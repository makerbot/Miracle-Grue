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
//  CPPUNIT_TEST( testConfig );
//  CPPUNIT_TEST( testFloatFormat );
  CPPUNIT_TEST( testSingleExtruder );
//  CPPUNIT_TEST( testDualExtruders );
//  CPPUNIT_TEST( testSimplePath );
  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:
  void testSingleExtruder();
  void testDualExtruders();
  void testSimplePath();
  void testConfig();
  void testFloatFormat();

};


#endif

