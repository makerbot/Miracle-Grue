/* 
 * File:   PatherOptimizerTestCase.h
 * Author: Dev
 *
 * Created on July 26, 2012, 1:22 PM
 */

#ifndef PATHEROPTIMIZERTESTCASE_H
#define	PATHEROPTIMIZERTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

class PatherOptimizerTestCase : public CPPUNIT_NS::TestFixture {
	
	CPPUNIT_TEST_SUITE( PatherOptimizerTestCase );
	
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testBoundary );
	
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
protected:
	void testBasics();
	void testBoundary();
	void testCompleteness();
};



#endif	/* PATHEROPTIMIZERTESTCASE_H */

