#include <vector>

#include "UnitTestUtils.h"
#include "InsetsTestCase.h"
#include "mgl/regioner.h"

using namespace std;
using namespace mgl;
using namespace libthing;

CPPUNIT_TEST_SUITE_REGISTRATION( InsetsTestCase );

void InsetsTestCase::setUp() {
	regionerCfg.nbOfShells = 2;
	regionerCfg.insetDistanceMultiplier = .9;

	Loop::cw_iterator at =
		square.insertPointAfter(Vector2(10.0, 10.0), square.clockwiseEnd());
	at = square.insertPointAfter(Vector2(10.0, -10.0), at);
	at = square.insertPointAfter(Vector2(-10.0, -10.0), at);
	at = square.insertPointAfter(Vector2(-10.0, 10.0), at);
}

void InsetsTestCase::testSingleSquareInset() {
	Regioner regioner(regionerCfg);

	LoopList squarelist;
	squarelist.push_back(square);
	std::list<LoopList> insets;
	LoopList interiors;

	regioner.insetsForSlice(squarelist, insets, interiors, layermeasure);

	Loop &inset = insets.back().back();

	Loop::finite_cw_iterator pn = inset.clockwiseFinite();
	CPPUNIT_ASSERT_EQUAL(-8.6, pn->getPoint().x);
	CPPUNIT_ASSERT_EQUAL(8.6, pn->getPoint().y);

	pn++;
	CPPUNIT_ASSERT_EQUAL(8.6, pn->getPoint().x);
	CPPUNIT_ASSERT_EQUAL(8.6, pn->getPoint().y);
	
	pn++;
	CPPUNIT_ASSERT_EQUAL(8.6, pn->getPoint().x);
	CPPUNIT_ASSERT_EQUAL(-8.6, pn->getPoint().y);

	pn++;
	CPPUNIT_ASSERT_EQUAL(-8.6, pn->getPoint().x);
	CPPUNIT_ASSERT_EQUAL(-8.6, pn->getPoint().y);
}


