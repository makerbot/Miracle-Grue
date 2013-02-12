#ifndef MGL_CORE_TEST_CASE_H_
#define GCODER_TEST_CASE_H_

#include <cppunit/extensions/HelperMacros.h>

#include <jsoncpp/json/value.h>

#include "mgl/JsonConverter.h"

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */
class JsonConverterTestCase : public CPPUNIT_NS::TestFixture
{

  CPPUNIT_TEST_SUITE( JsonConverterTestCase );

  CPPUNIT_TEST( testScalarToJson );
  CPPUNIT_TEST( testPolygonToJson );
  CPPUNIT_TEST( testPolygonsToJson );
  CPPUNIT_TEST( testPolygonsGroupToJson );
  //CPPUNIT_TEST( testExtruderSliceToJson );

  CPPUNIT_TEST( testJsonToScalar );
  CPPUNIT_TEST( testJsonToVector2 );
  CPPUNIT_TEST( testJsonToPolygon );
  CPPUNIT_TEST( testJsonToPolygons );
  CPPUNIT_TEST( testJsonToPolygonsGroup );


  CPPUNIT_TEST( testReadScalarToJson );
  CPPUNIT_TEST( testReadVector2ToJson );
  CPPUNIT_TEST( testReadPolygonToJson );
  CPPUNIT_TEST( testReadPolygonsToJson );
  CPPUNIT_TEST( testReadPolygonsGroupToJson );

  CPPUNIT_TEST_SUITE_END();



public:
  void setUp();

protected:
  // Load values
  void testScalarToJson();
  void testVector2ToJson();
  void testPolygonToJson();
  void testPolygonsToJson();
  void testPolygonsGroupToJson();
  //void testExtruderSliceToJson();

  void testJsonToScalar();
  void testJsonToVector2();
  void testJsonToPolygon();
  void testJsonToPolygons();
  void testJsonToPolygonsGroup();


  void testReadScalarToJson();
  void testReadVector2ToJson();
  void testReadPolygonToJson();
  void testReadPolygonsToJson();
  void testReadPolygonsGroupToJson();

};


#endif /*MGL_CORE_TEST_CASE_H_*/

