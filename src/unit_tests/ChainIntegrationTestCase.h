
#ifndef PATHER_TEST_CASE_H
#define PATHER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class ChainIntegrationTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ChainIntegrationTestCase );
  	  CPPUNIT_TEST( testChain );
  CPPUNIT_TEST_SUITE_END();


public:


protected:

  void testChain();

};


#endif
