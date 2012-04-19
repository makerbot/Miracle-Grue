#ifndef ROOFING_TEST_CASE_H_
#define ROOFING_TEST_CASE_H_

#include <cppunit/extensions/HelperMacros.h>



class RoofingTestCase : public CPPUNIT_NS::TestFixture
{

private:

	CPPUNIT_TEST_SUITE( RoofingTestCase );

//		CPPUNIT_TEST(testSimple);
//		CPPUNIT_TEST(testHoly);
//		CPPUNIT_TEST(testGrid);
//		CPPUNIT_TEST(testIntersectRange);
//		CPPUNIT_TEST(testSimpleLineTersect);
//		CPPUNIT_TEST(testLineTersect2);
//		CPPUNIT_TEST(testBooleanIntersect);
//		CPPUNIT_TEST(testSimpleDifference);
//		CPPUNIT_TEST(testSubRangeDifference);
//		CPPUNIT_TEST(testRangeDifference);
//
//		CPPUNIT_TEST(testSimpleUnion);
		CPPUNIT_TEST(testSubRangeUnion);
		CPPUNIT_TEST(testRangeUnion);
    CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:

  void testSimple();

  void testHoly();

  void testGrid();

  void testIntersectRange();

  void testSimpleLineTersect();

  void testBooleanIntersect();

  void testLineTersect2();

  void testSimpleDifference();

  void testSubRangeDifference();

  void testRangeDifference();

  void testSimpleUnion();

  void testSubRangeUnion();

  void testRangeUnion();
};

#endif
