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
		CPPUNIT_TEST(testIntersectRange);
		CPPUNIT_TEST(testSimpleLineTersect);
		CPPUNIT_TEST(testLineTersect2);
		CPPUNIT_TEST(testBooleanIntersect);
		CPPUNIT_TEST(testSimpleDifference);



		CPPUNIT_TEST(testSubRangeDifference);
		CPPUNIT_TEST(testRangeDifference);
		CPPUNIT_TEST(testDifferenceRangeEmpty);

		CPPUNIT_TEST(testSimpleUnion);
		CPPUNIT_TEST(testSubRangeUnion);
		CPPUNIT_TEST(testRangeUnion);
        CPPUNIT_TEST(testGridStruct);

        CPPUNIT_TEST(testFlatsurface);
        CPPUNIT_TEST(testSkeleton);

        CPPUNIT_TEST(test3dKnotPlatform);

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

  void testDifferenceRangeEmpty();

  void testSimpleUnion();

  void testSubRangeUnion();

  void testRangeUnion();

  void testGridStruct();

  void testFlatsurface();

  void testSkeleton();

  void test3dKnotPlatform();


};

#endif
