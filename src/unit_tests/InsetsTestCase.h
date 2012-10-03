#ifndef INSETSTESTCASE_H
#define	INSETSTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

#include "mgl/regioner.h"
#include "mgl/mgl.h"

class InsetsTestCase : public CPPUNIT_NS::TestFixture{

	CPPUNIT_TEST_SUITE( InsetsTestCase );

	CPPUNIT_TEST( testSingleSquareInset );
	CPPUNIT_TEST( testSquareSpur );
	
	CPPUNIT_TEST_SUITE_END();
	
public:
	InsetsTestCase() : layermeasure(1, 1, 1) {}
	void setUp();

protected:
	void testSingleSquareInset();
	void testSquareSpur();

private:
	mgl::RegionerConfig regionerCfg;
	mgl::Loop square;
	mgl::Loop squareSpur;
	mgl::LayerMeasure layermeasure;
};


#endif	/* LOOPPATHTESTCASE_H */

