#ifndef ROOFING_TEST_CASE_H_
#define ROOFING_TEST_CASE_H_

#include <cppunit/extensions/HelperMacros.h>



class RoofingTestCase : public CPPUNIT_NS::TestFixture
{

private:

	CPPUNIT_TEST_SUITE( RoofingTestCase );
		CPPUNIT_TEST(testSimple);
		CPPUNIT_TEST(testHoly);
		CPPUNIT_TEST(testGrid);
		CPPUNIT_TEST(testSimpleLineTersect);
		CPPUNIT_TEST(testBooleanIntersect);

    CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:

  void testSimple();

  void testHoly();

  void testGrid();

  void testSimpleLineTersect();

  void testBooleanIntersect();


};

#endif
