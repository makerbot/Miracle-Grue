#ifndef SLICER_TEST_CASE_H
#define SLICER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>



class SlicerTestCase : public CPPUNIT_NS::TestFixture
{

private:

	CPPUNIT_TEST_SUITE( SlicerTestCase );
        //CPPUNIT_TEST( testSlicyKnot_44 );
        //CPPUNIT_TEST( testNormals );
        //CPPUNIT_TEST( testCut );
//		CPPUNIT_TEST( testAngles );
//		CPPUNIT_TEST( testInset );
        CPPUNIT_TEST( testInset2 );
        CPPUNIT_TEST( testInset3 );
        CPPUNIT_TEST( testInset4 );
        CPPUNIT_TEST(testHexagon);
        CPPUNIT_TEST(testMotorcycles);
       // CPPUNIT_TEST( testSliceTriangle );
    CPPUNIT_TEST_SUITE_END();

protected:

  void testBootstrap();
  void testSlicySimple();
  void testSlicyKnot_44();
  void testNormals();

  void testCut();
  void testAngles();
  void testSliceTriangle();

  void testInset();
  void testInset2();
  void testInset3();
  void testInset4();
  void testHexagon();

  void testMotorcycles();
};


#endif
