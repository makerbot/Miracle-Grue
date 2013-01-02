/* 
 * File:   HierarchyTestCase.h
 * Author: Dev
 *
 * Created on January 2, 2013, 11:05 AM
 */

#ifndef HIERARCHYTESTCASE_H
#define	HIERARCHYTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>
#include "mgl/labeled_path.h"
#include <list>

class HierarchyTestCase : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE( HierarchyTestCase );
    CPPUNIT_TEST( test );
    CPPUNIT_TEST_SUITE_END();
public:
    HierarchyTestCase(); //to initialize insets
    void setUp();
protected:
    /* all the test funcs go here */
    void test();
private:
    typedef std::list<mgl::Loop> inset_list;
    typedef std::list<inset_list> inset_table;
    typedef std::list<mgl::OpenPath> infill_list;
    inset_table m_insets;
    infill_list m_infill;
};

#endif	/* HIERARCHYTESTCASE_H */

