/* 
 * File:   GantryTestCase.h
 * Author: Dev
 *
 * Created on June 13, 2012, 3:58 PM
 */

#ifndef GANTRYTESTCASE_H
#define	GANTRYTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

class GantryTestCase : public CPPUNIT_NS::TestFixture{
	
	CPPUNIT_TEST_SUITE( GantryTestCase );
	
	CPPUNIT_TEST( testInitConfig );
	CPPUNIT_TEST( testManualMove );
	CPPUNIT_TEST( testG1Move );
	CPPUNIT_TEST( testG1Extrude );
	CPPUNIT_TEST( testSquirtSnort );
	CPPUNIT_TEST( testConfig );
	
	CPPUNIT_TEST_SUITE_END();
	
public:
	void setUp();
protected:
	void testInitConfig();
	void testManualMove();
	void testG1Move();
	void testG1Extrude();
	void testSquirtSnort();
	void testConfig();
};


#endif	/* GANTRYTESTCASE_H */

