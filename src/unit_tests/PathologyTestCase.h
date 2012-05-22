#ifndef ROOFING_TEST_CASE_H_
#define ROOFING_TEST_CASE_H_

#include <cppunit/extensions/HelperMacros.h>



class PathologyTestCase : public CPPUNIT_NS::TestFixture
{

private:

	CPPUNIT_TEST_SUITE( PathologyTestCase );
		CPPUNIT_TEST(testSimple);
    CPPUNIT_TEST_SUITE_END();


public:
  void setUp();

protected:

  void testSimple();

};

#endif
