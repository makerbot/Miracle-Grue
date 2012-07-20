#ifndef GRID_TEST_CASE_H_
#define GRID_TEST_CASE_H_

#include <cppunit/extensions/HelperMacros.h>



class GridTestCase : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( GridTestCase );
	CPPUNIT_TEST( testGridRangesToOpenPaths );
    CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:
	void testGridRangesToOpenPaths();

};

#endif
