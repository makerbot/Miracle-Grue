/*
#ifndef SLICER_TEST_CASE_H
#define SLICER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>


class SlicerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( SlicerTestCase );
  CPPUNIT_TEST( testSlicySimple );
  CPPUNIT_TEST( testNormals );
  CPPUNIT_TEST_SUITE_END();



protected:

  void testSlicySimple();
  void testNormals();
};


#endif
*/


#ifndef SLICER_TEST_CASE_H
#define SLICER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>



class SlicerTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( SlicerTestCase );
    CPPUNIT_TEST( testSlicySimple );
    CPPUNIT_TEST( testSlicyKnot_44 );
    CPPUNIT_TEST( testNormals );
    CPPUNIT_TEST( testCut );
    CPPUNIT_TEST( testInset );

  CPPUNIT_TEST_SUITE_END();


public:
  //void setUp();

protected:

  void testBootstrap();
  void testSlicySimple();
  void testSlicyKnot_44();
  void testNormals();

  void testCut();
  void testInset();
};


#endif
