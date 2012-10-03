#include <vector>
#include <iostream>

#include "UnitTestUtils.h"
#include "InsetsTestCase.h"
#include "mgl/regioner.h"
#include "libthing/LineSegment2.h"

using namespace std;
using namespace mgl;
using namespace libthing;

CPPUNIT_TEST_SUITE_REGISTRATION( InsetsTestCase );

void svgBegin() {
	cout << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?><svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" x=\"loops\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"  >\"" << endl;
}

void svgEnd() {
	cout << "</svg>" << endl;
}

void segToSVG(const LineSegment2 seg) {
	cout << "<line x1=\"" << seg.a.x * 10 << "\" y1=\"" << seg.a.y * 10
		 << "\" x2=\"" << seg.b.x * 10 << "\" y2=\"" << seg.b.y * 10
		 << "\"	style=\"stroke-width: 2; stroke: black;\"/>" << endl;
}

void loopToSVG(const Loop loop) {
	for (Loop::const_finite_cw_iterator cw = loop.clockwiseFinite();
		 cw != loop.clockwiseEnd();
		 ++cw) {
		LineSegment2 seg = loop.segmentAfterPoint(cw);
		segToSVG(seg);
	}
}

void loopsToSVG(const LoopList loops) {
	for (LoopList::const_iterator loop = loops.begin();
		 loop != loops.end(); ++loop) {
		loopToSVG(*loop);
	}
}

void loopTableToSVG(const list<LoopList> table) {
	for (list<LoopList>::const_iterator loops = table.begin();
		 loops != table.end(); ++loops) {
		loopsToSVG(*loops);
	}
}

void InsetsTestCase::setUp() {
	regionerCfg.nbOfShells = 2;
	regionerCfg.insetDistanceMultiplier = .9;

	Loop::cw_iterator at =
		square.insertPointAfter(Vector2(10.0, 10.0), square.clockwiseEnd());
	at = square.insertPointAfter(Vector2(10.0, -10.0), at);
	at = square.insertPointAfter(Vector2(-10.0, -10.0), at);
	at = square.insertPointAfter(Vector2(-10.0, 10.0), at);

	at =
		squareSpur.insertPointAfter(Vector2(10.0, 15.0),
									squareSpur.clockwiseEnd());
	at = squareSpur.insertPointAfter(Vector2(10.0, -10.0), at);
	at = squareSpur.insertPointAfter(Vector2(-10.0, -10.0), at);
	at = squareSpur.insertPointAfter(Vector2(-10.0, 10.0), at);
	at = squareSpur.insertPointAfter(Vector2(8.5, 10.0), at);
	at = squareSpur.insertPointAfter(Vector2(8.5, 15.0), at);
}

void InsetsTestCase::testSingleSquareInset() {
	Regioner regioner(regionerCfg);

	LoopList squarelist;
	squarelist.push_back(square);
	std::list<LoopList> insets;
	LoopList interiors;

	regioner.insetsForSlice(squarelist, layermeasure, insets, interiors);

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


void InsetsTestCase::testSquareSpur() {
	Regioner regioner(regionerCfg);

	svgBegin();
	loopToSVG(squareSpur);
	svgEnd();

	LoopList outlines;
	outlines.push_back(squareSpur);
	std::list<LoopList> insets;
	LoopList interiors;

	regioner.insetsForSlice(outlines, layermeasure, insets, interiors);

	std::list<LoopList> spurs;
	regioner.spurLoopsForSlice(outlines, insets, layermeasure, spurs);

	svgBegin();
	loopTableToSVG(spurs);
	svgEnd();

	cout << "Shells with spurs" << endl;
	CPPUNIT_ASSERT_EQUAL(1, (int)spurs.size());
	cout << "Spurs for shell" << endl;
	CPPUNIT_ASSERT_EQUAL(1, (int)spurs.back().size());

}
