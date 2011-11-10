#ifndef FILE_WRITER_TEST_CASE_H
#define FILE_WRITER_TEST_CASE_H

#include <cppunit/extensions/HelperMacros.h>
/*
 * A test case that is designed to produce
 * example errors and failures
 *
 */
class ExampleOpTestCase: public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( ExampleOpTestCase);
	CPPUNIT_TEST( create_ExampleOp );
	CPPUNIT_TEST( createNoDenit_ExampleOp );
	CPPUNIT_TEST( createNoStreamEnd_ExampleOp );
	CPPUNIT_TEST_SUITE_END();

//
//
//public:
//  void setUp();
//

protected:
  void create_ExampleOp();
  void createNoDenit_ExampleOp();
  void createNoStreamEnd_ExampleOp();
};


#endif /* CONFIG_TEST_CASE_H */

