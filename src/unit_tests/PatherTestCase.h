
#ifndef PATHER_TEST_CASE_H
#define PATHER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class PatherTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( PatherTestCase );
  CPPUNIT_TEST( testLinea );
  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:

  void example();

  void testLinea();

};


#endif
