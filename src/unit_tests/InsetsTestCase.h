#ifndef INSETSTESTCASE_H
#define	INSETSTESTCASE_H

#include <cppunit/extensions/HelperMacros.h>

#include "mgl/gcoder.h"
#include "mgl/mgl.h"
#include "mgl/regioner.h"
#include "libthing/LineSegment2.h"



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

	/*CPPUNIT_TEST( testSingleSquareInset );
	CPPUNIT_TEST( testSquareSpurRegion );
	CPPUNIT_TEST( testSquareSpurFill );
	CPPUNIT_TEST( testTriangleSpurRegion );
	CPPUNIT_TEST( testTriangleSpurFill );
	CPPUNIT_TEST( testFindWallPairs );
    CPPUNIT_TEST( testCompleteTrapezoid );
    CPPUNIT_TEST( testCompleteTrapezoidRev );
    CPPUNIT_TEST( testBisectWalls );
    CPPUNIT_TEST( testBisectReverseWalls );
    CPPUNIT_TEST( testCompleteParallel );
    CPPUNIT_TEST( testTwoPairFill );*/
    //CPPUNIT_TEST( testThreePairFill );
    //CPPUNIT_TEST( testStretchlet );
    CPPUNIT_TEST( testStretchletWallPairs );

	CPPUNIT_TEST_SUITE_END();
	
public:
	InsetsTestCase() : layermeasure(1, 1, 1) {}
	void setUp();

protected:
	void testSingleSquareInset();
	void testSquareSpurRegion();
	void testTriangleSpurRegion();
	void testTriangleSpurFill();
    void testFindWallPairs();
    void testCompleteTrapezoid();
    void testCompleteTrapezoidRev();
    void testBisectWalls();
    void testBisectReverseWalls();
    void testCompleteParallel();
    void testTwoPairFill();
    void testThreePairFill();
    void testStretchlet();
    void testStretchletWallPairs();

	//not yet implemented
	void testSquareSpurFill() {}
	

private:
	InsetsTestCaseConfig config;
	mgl::Loop square;
	mgl::Loop squareSpurShell;
	mgl::Loop triangleSpurShell;
	mgl::Loop triangleSpurLoop;
    mgl::Loop twoPairShell;
    mgl::Loop threePairShell;
    std::pair<libthing::LineSegment2, libthing::LineSegment2> triangleSpurWalls;
    std::pair<libthing::LineSegment2, libthing::LineSegment2> revTriangleSpurWalls;
    std::pair<libthing::LineSegment2, libthing::LineSegment2> parallelSpurWalls;
	mgl::LayerMeasure layermeasure;

    mgl::LoopList stretchletLoops;
};


#endif	/* LOOPPATHTESTCASE_H */

