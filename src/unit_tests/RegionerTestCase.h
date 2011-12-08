
#ifndef REGIONER_TEST_CASE_H
#define REGIONER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class RegionerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( RegionerTestCase );
//  CPPUNIT_TEST( example );
  CPPUNIT_TEST(testMp);
  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:

  void example();
  void testMp();
};


#endif
