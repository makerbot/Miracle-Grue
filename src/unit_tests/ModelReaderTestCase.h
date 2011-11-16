
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

  //CPPUNIT_TEST( testMeshySimple );
  CPPUNIT_TEST( testSlicySimple );
  CPPUNIT_TEST( testSlicyWater );
  //CPPUNIT_TEST( testMeshyLoad );
  //CPPUNIT_TEST( testLargeMeshy );

  CPPUNIT_TEST_SUITE_END();


public:
  //void setUp();

protected:
  void testMeshySimple();
  void testSlicySimple();
  void testMeshyLoad();
  void testLargeMeshy();
  void testSlicyWater();
};


#endif
