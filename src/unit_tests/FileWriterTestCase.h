#ifndef FILE_WRITER_TEST_CASE_H
#define FILE_WRITER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>
/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */
class FileWriterTestCase: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( FileWriterTestCase);
// CPPUNIT_TEST( create_FileWriter );
//  CPPUNIT_TEST( createConfigRunDestroy_FileWriter );
  CPPUNIT_TEST( streamFileTo_FileWriter );
  CPPUNIT_TEST_SUITE_END();

//
//
//public:
//  void setUp();
//

protected:
  void create_FileWriter();
  void createConfigRunDestroy_FileWriter( );
  void streamFileTo_FileWriter( );
};


#endif /* CONFIG_TEST_CASE_H */

