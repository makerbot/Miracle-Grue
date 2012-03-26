#ifndef CLIPPER_TEST_CASE_H
#define CLIPPER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>



class ClipperTestCase : public CPPUNIT_NS::TestFixture
{

private:

	CPPUNIT_TEST_SUITE( ClipperTestCase );
     //   CPPUNIT_TEST(test_conversion);
     //   CPPUNIT_TEST(testSimpleClipper);
        CPPUNIT_TEST(testSimpleInset);
    CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:

  void test_conversion();
  void testSimpleInset();
  void testSimpleClipper();
};

#endif
