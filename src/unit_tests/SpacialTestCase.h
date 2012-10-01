/* 
 * File:   SpacialTestCase.h
 * Author: Dev
 *
 * Created on October 1, 2012, 12:23 PM
 */

#ifndef SPACIALTESTCASE_H
#define	SPACIALTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

class SpacialTestCase : public CPPUNIT_NS::TestFixture {
private:
    CPPUNIT_TEST_SUITE( SpacialTestCase );
    CPPUNIT_TEST( testInsertion );
    CPPUNIT_TEST( testFilter );
    CPPUNIT_TEST( testEmpty );
    CPPUNIT_TEST( testStress );
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
protected:
    
    void testInsertion();
    void testFilter();
    void testEmpty();
    void testStress();
    
};



#endif	/* SPACIALTESTCASE_H */

