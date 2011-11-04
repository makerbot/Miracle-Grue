#ifndef CONFIG_TEST_CASE_H
#define CONFIG_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>
/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */
class ConfigTestCase: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ConfigTestCase);
  CPPUNIT_TEST( configWrite );
//  CPPUNIT_TEST( dualExtruders );
//  CPPUNIT_TEST( simplePath );
//  CPPUNIT_TEST( spikeBed );
//  CPPUNIT_TEST( example );
  CPPUNIT_TEST_SUITE_END();
//
//
//public:
//  void setUp();
//
protected:
  void configWrite();
//  void singleExtruder();
//  void dualExtruders();
//  void simplePath();
//  void spikeBed();
//
//  void example();

};


#endif /* CONFIG_TEST_CASE_H */

