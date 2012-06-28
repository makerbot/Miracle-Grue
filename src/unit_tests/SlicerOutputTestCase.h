/* 
 * File:   SlicerOutputTestCase.h
 * Author: Dev
 *
 * Created on June 27, 2012, 4:59 PM
 */

#ifndef SLICEROUTPUTTESTCASE_H
#define	SLICEROUTPUTTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>
#include "../mgl/mgl.h"
#include "../mgl/meshy.h"

class SlicerOutputTestCase : public CPPUNIT_NS::TestFixture {
	CPPUNIT_TEST_SUITE( SlicerOutputTestCase );
	CPPUNIT_TEST(testLoopLayer);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
protected:
	void testLoopLayer();
};



#endif	/* SLICEROUTPUTTESTCASE_H */

