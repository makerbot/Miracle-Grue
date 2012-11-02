/* 
 * File:   GantryTestCase.h
 * Author: Dev
 *
 * Created on June 13, 2012, 3:58 PM
 */

#ifndef CONTAINMENTTESTCASE_H
#define	CONTAINMENTTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>
#include "mgl/loop_path.h"

class ContainmentTestCase : public CPPUNIT_NS::TestFixture{
	
	CPPUNIT_TEST_SUITE( ContainmentTestCase );
	
    CPPUNIT_TEST( testInsideOutside );
    CPPUNIT_TEST( testHierarchy );
    CPPUNIT_TEST( testReverseOrder );
	
	CPPUNIT_TEST_SUITE_END();
	
public:
	void setUp();
protected:
	void testInsideOutside();
	void testHierarchy();
    void testReverseOrder();
private:
    
    mgl::Loop m_loopOuter;
    mgl::Loop m_loopInner;
    mgl::Point2Type outsidePoint;
    mgl::Point2Type outerPoint;
    mgl::Point2Type innerPoint;
};


#endif	/* GANTRYTESTCASE_H */

