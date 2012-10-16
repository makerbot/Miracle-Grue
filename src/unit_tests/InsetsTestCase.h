#ifndef INSETSTESTCASE_H
#define	INSETSTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

#include "mgl/gcoder.h"
#include "mgl/mgl.h"
#include "mgl/regioner.h"


class InsetsTestCaseConfig : public mgl::GrueConfig {
public:
	InsetsTestCaseConfig() {};
	InsetsTestCaseConfig(unsigned p_nbOfShells,
						 Scalar p_insetDistanceMultiplier) {
		nbOfShells = p_nbOfShells;
		insetDistanceMultiplier = p_insetDistanceMultiplier;
	}
};

class InsetsTestCase : public CPPUNIT_NS::TestFixture{

	CPPUNIT_TEST_SUITE( InsetsTestCase );

	CPPUNIT_TEST( testSingleSquareInset );
	CPPUNIT_TEST( testSquareSpurRegion );
	CPPUNIT_TEST( testSquareSpurFill );
	CPPUNIT_TEST( testTriangleSpurRegion );
	CPPUNIT_TEST( testTriangleSpurFill );
	
	CPPUNIT_TEST_SUITE_END();
	
public:
	InsetsTestCase() : layermeasure(1, 1, 1) {}
	void setUp();

protected:
	void testSingleSquareInset();
	void testSquareSpurRegion();
	void testTriangleSpurRegion();
	void testTriangleSpurFill();

	//not yet implemented
	void testSquareSpurFill() {}
	

private:
	InsetsTestCaseConfig config;
	mgl::Loop square;
	mgl::Loop squareSpurShell;
	mgl::Loop triangleSpurShell;
	std::list<mgl::LoopList> triangleSpurLoops;
	mgl::LayerMeasure layermeasure;
};


#endif	/* LOOPPATHTESTCASE_H */

