#ifndef MGL_CORE_TEST_CASE_H_
#define GCODER_TEST_CASE_H_

#include <cppunit/extensions/HelperMacros.h>

/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */
class MglCoreTestCase : public CPPUNIT_NS::TestFixture
{

  CPPUNIT_TEST_SUITE( MglCoreTestCase );

  // test base exception class
  CPPUNIT_TEST( ExceptionTest );

  // test base scalar value
  CPPUNIT_TEST( ScalarBasics );
  CPPUNIT_TEST( ScalarMaths );

  CPPUNIT_TEST( Vector2Basics );
//  CPPUNIT_TEST( Vector2Maths );

  CPPUNIT_TEST(LineSegment2dBasics);

  CPPUNIT_TEST( Vector3Basics );

  CPPUNIT_TEST( Triangle3Maths );
  CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:
 void ExceptionTest();

 void  ScalarBasics();
 void  ScalarMaths();

 void  Vector2Basics();
// void  Vector2Maths();

 void LineSegment2dBasics();

 void Vector3Basics();

 void Triangle3Maths();
};


#endif /*MGL_CORE_TEST_CASE_H_*/

