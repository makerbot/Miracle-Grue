#ifndef SLICER_TEST_CASE_H
#define SLICER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>



class SlicerTestCase : public CPPUNIT_NS::TestFixture
{

private:

	CPPUNIT_TEST_SUITE( SlicerTestCase );

        // DOES NOT WORK YET!
        //        CPPUNIT_TEST(testFutureSlice);
        //CPPUNIT_TEST( testSlicyKnot_44 );
        //CPPUNIT_TEST( testNormals );
        //CPPUNIT_TEST( testCut );
        //		CPPUNIT_TEST( testAngles );
        //		CPPUNIT_TEST( testInset );
        //		CPPUNIT_TEST( testInset2 );
        //        CPPUNIT_TEST( testInset3 );
        //        CPPUNIT_TEST( testInset4 );
        //        CPPUNIT_TEST(testHexagon);
        //
        //        CPPUNIT_TEST( testSliceTriangle );
        //
        //  		CPPUNIT_TEST( testOpenPoly );
        //  		CPPUNIT_TEST( testSquareBug );
        //  		CPPUNIT_TEST( testHexaBug );
        //	CPPUNIT_TEST( testKnotBug );
		//CPPUNIT_TEST( testKnot89 );
		CPPUNIT_TEST( scratch );
		CPPUNIT_TEST( test_slice_0_loop_0 );
    CPPUNIT_TEST_SUITE_END();




protected:

  void testBootstrap();
  void testSlicySimple();
  void testSlicyKnot_44();
  void testNormals();

  void testCut();
  void testAngles();
  void testSliceTriangle();
  void testSliceTriangle2();

  void testInset();
  void testInset2();
  void testInset3();
  void testInset4();
  void testHexagon();

  void testFutureSlice();

  void testOpenPoly();

  void testSquareBug();
  void testHexaBug();
  void testKnotBug();
  void testKnot89();

  void scratch();

  void test_slice_0_loop_0();
};


#endif
