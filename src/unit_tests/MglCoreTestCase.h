#ifndef MGL_CORE_TEST_CASE_H_
#define GCODER_TEST_CASE_H_

#include <cppunit/extensions/HelperMacros.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */
class MglCoreTestCase : public CPPUNIT_NS::TestFixture
{

  CPPUNIT_TEST_SUITE( MglCoreTestCase );

  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

//protected:

};


#endif /*MGL_CORE_TEST_CASE_H_*/

