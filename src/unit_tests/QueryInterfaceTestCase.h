#ifndef INTERROGATE_TEST_CASE_H
#define INTERROGATE_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>
/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */
class QueryInterfaceTestCase: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( QueryInterfaceTestCase);
  CPPUNIT_TEST( simpleInterrogationTest );
  CPPUNIT_TEST( testConfgurationVsInformation );
  CPPUNIT_TEST_SUITE_END();
//
//
//public:
//  void setUp();
//
protected:
  void simpleInterrogationTest();
  void testConfgurationVsInformation();
//  void dualExtruders();

};


#endif /* CONFIG_TEST_CASE_H */
