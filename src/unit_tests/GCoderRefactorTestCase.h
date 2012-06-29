/* 
 * File:   GcoderRefactorTestCase.h
 * Author: Dev
 *
 * Created on June 28, 2012, 6:00 PM
 */

#ifndef GCODERREFACTORTESTCASE_H
#define	GCODERREFACTORTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

class GCoderRefactorTestCase : public CPPUNIT_NS::TestFixture {
private:
	CPPUNIT_TEST_SUITE( GCoderRefactorTestCase );
	CPPUNIT_TEST( testPathGcode );
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();
protected:
	void testPathGcode();
};



#endif	/* GCODERREFACTORTESTCASE_H */

