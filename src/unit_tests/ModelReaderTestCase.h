
#ifndef MODEL_READER_TEST_CASE_H
#define MODEL_READER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class ModelReaderTestCase : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ModelReaderTestCase );
//	  CPPUNIT_TEST( testLayerSplit );
//	  CPPUNIT_TEST( testSlicySimple );

	  CPPUNIT_TEST( testMeshyCycleNull);
  	  CPPUNIT_TEST( testMeshyCycleMin );
	  CPPUNIT_TEST( testMeshyCycle );
////	  CPPUNIT_TEST( testMeshyLoad );
//// //	  CPPUNIT_TEST( testLargeMeshy );
////	  CPPUNIT_TEST( testSlicyWater );
//
//	  CPPUNIT_TEST( testTubularInflate);
//	  CPPUNIT_TEST( testCutTriangle);
//
//	  /// //	  CPPUNIT_TEST( testRotate);
//	  CPPUNIT_TEST( testInputStls );
//// //	  CPPUNIT_TEST( fixContourProblem );
	  CPPUNIT_TEST( testLayerMeasure );
//// //	  CPPUNIT_TEST( testMyStls );
//
//	  CPPUNIT_TEST( testMeshySimple );
//	  CPPUNIT_TEST( testKnot);
	CPPUNIT_TEST( testAlignToPlate );
  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:

  void testMeshyCycleNull();
  void testMeshyCycleMin();
  void testMeshyCycle();

//  void testSlicySimple();

  void testLayerSplit();
  void testMeshySimple();

  void testLayerMeasure();

  void testMeshyLoad();
  void testLargeMeshy();
//  void testSlicyWater();

  // cutting a triangle along z line
  void testCutTriangle();
  // rotation nad translation of 2D points
  void testRotate();
  void testTubularInflate();
  void testInputStls();
  void testMyStls();
  void fixContourProblem();
  void testKnot();
	void testAlignToPlate();
};


#endif
