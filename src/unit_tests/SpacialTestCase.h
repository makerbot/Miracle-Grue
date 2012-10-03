/* 
 * File:   SpacialTestCase.h
 * Author: Dev
 *
 * Created on October 1, 2012, 12:23 PM
 */

#ifndef SPACIALTESTCASE_H
#define	SPACIALTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

static const size_t SET_SIZE = 1000000;

class SpacialTestCase : public CPPUNIT_NS::TestFixture {
private:
    CPPUNIT_TEST_SUITE( SpacialTestCase );
//    CPPUNIT_TEST( testInsertion );
//    CPPUNIT_TEST( testFilter );
//    CPPUNIT_TEST( testEmpty );
//    CPPUNIT_TEST( testStress );
//    CPPUNIT_TEST( testRtreeFilter );
//    CPPUNIT_TEST( testRtreeEmpty );
    CPPUNIT_TEST( testRtreeStress );
    CPPUNIT_TEST( testPerformance );
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
protected:
    
    void testInsertion();
    void testFilter();
    void testEmpty();
    void testStress();
    void testRtreeFilter();
    void testRtreeEmpty();
    void testRtreeStress();
    void testPerformance(); //brute, boxlist, rtree
    
};



#endif	/* SPACIALTESTCASE_H */

