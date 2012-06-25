#ifndef LOOPPATHTESTCASE_H
#define	LOOPPATHTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

class LoopPathTestCase : public CPPUNIT_NS::TestFixture{
	
	CPPUNIT_TEST_SUITE( LoopPathTestCase );
	
	CPPUNIT_TEST( testOpenPathBasic );
	CPPUNIT_TEST( testLoopBasic );
	CPPUNIT_TEST( testLoopPathBasic );
	CPPUNIT_TEST( testOpenPathJoin );
	CPPUNIT_TEST( testOpenToLoopPathJoin );
	
	CPPUNIT_TEST_SUITE_END();
	
public:
	void setUp();
protected:
	void testOpenPathBasic();
	void testLoopBasic();
	void testLoopPathBasic();
	void testOpenPathJoin();
	void testOpenToLoopPathJoin();
};


#endif	/* LOOPPATHTESTCASE_H */

