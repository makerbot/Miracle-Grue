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
	cerr << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?><svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" x=\"loops\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"  >\"" << endl;
}

void svgEnd() {
	cerr << "</svg>" << endl;
}

void segToSVG(const LineSegment2 seg, const string &color,
			  const Scalar xoff, const Scalar yoff) {
	cerr << "<line x1=\"" << (xoff + seg.a.x) * 10 
		 << "\" y1=\""  << (yoff + seg.a.y) * 10
		 << "\" x2=\"" << (xoff + seg.b.x) * 10 
		 << "\" y2=\"" << (yoff + seg.b.y) * 10
		 << "\"	style=\"stroke-width: 1; stroke: " << color << ";\"/>" << endl;
}

void loopToSVG(const Loop loop, const string &color,
			   const Scalar xoff, const Scalar yoff) {
	for (Loop::const_finite_cw_iterator cw = loop.clockwiseFinite();
		 cw != loop.clockwiseEnd();
		 ++cw) {
		LineSegment2 seg = loop.segmentAfterPoint(cw);
		segToSVG(seg, color, xoff, yoff);
	}
}

void loopsToSVG(const LoopList loops, const string &color,
				const Scalar xoff, const Scalar yoff) {
	for (LoopList::const_iterator loop = loops.begin();
		 loop != loops.end(); ++loop) {
		loopToSVG(*loop, color, xoff, yoff);
	}
}

void loopTableToSVG(const list<LoopList> table, const string &color,
					const Scalar xoff, const Scalar yoff) {
	for (list<LoopList>::const_iterator loops = table.begin();
		 loops != table.end(); ++loops) {
		loopsToSVG(*loops, color, xoff, yoff);
	}
}

void InsetsTestCase::setUp() {
	config = InsetsTestCaseConfig(2, .9);

	Loop::cw_iterator at =
		square.insertPointAfter(Vector2(10.0, 10.0), square.clockwiseEnd());
	at = square.insertPointAfter(Vector2(10.0, -10.0), at);
	at = square.insertPointAfter(Vector2(-10.0, -10.0), at);
	at = square.insertPointAfter(Vector2(-10.0, 10.0), at);

	at =
		squareSpurShell.insertPointAfter(Vector2(10.0, 15.0),
									squareSpurShell.clockwiseEnd());
	at = squareSpurShell.insertPointAfter(Vector2(10.0, -10.0), at);
	at = squareSpurShell.insertPointAfter(Vector2(-10.0, -10.0), at);
	at = squareSpurShell.insertPointAfter(Vector2(-10.0, 10.0), at);
	at = squareSpurShell.insertPointAfter(Vector2(9, 10.0), at);
	at = squareSpurShell.insertPointAfter(Vector2(9, 15.0), at);

	at =
		triangleSpurShell.insertPointAfter(Vector2(10.0, 20.0),
									triangleSpurShell.clockwiseEnd());
	at = triangleSpurShell.insertPointAfter(Vector2(10.0, -10.0), at);
	at = triangleSpurShell.insertPointAfter(Vector2(-10.0, -10.0), at);
	at = triangleSpurShell.insertPointAfter(Vector2(-10.0, 10.0), at);
	at = triangleSpurShell.insertPointAfter(Vector2(9, 10.0), at);
}

void InsetsTestCase::testSingleSquareInset() {
	Regioner regioner(config);

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


void InsetsTestCase::testSquareSpurRegion() {
	Regioner regioner(config);

	svgBegin();

	LoopList outlines;
	outlines.push_back(squareSpurShell);
	std::list<LoopList> insets;
	LoopList interiors;

	regioner.insetsForSlice(outlines, layermeasure, insets, interiors);

	std::list<LoopList> spurs;
	regioner.spurLoopsForSlice(outlines, insets, layermeasure, spurs);

	loopToSVG(squareSpurShell, "black", 20, 20);
	loopTableToSVG(insets, "red", 20, 20);
	loopTableToSVG(spurs, "green", 20, 20);
	svgEnd();

	cout << "Shells with spurs" << endl;
	CPPUNIT_ASSERT_EQUAL(3, (int)spurs.size());
	cout << "Spurs for shell" << endl;
	CPPUNIT_ASSERT_EQUAL(1, (int)spurs.front().size());

}

void InsetsTestCase::testTriangleSpurRegion() {
	Regioner regioner(config);

	svgBegin();

	LoopList outlines;
	outlines.push_back(triangleSpurShell);
	std::list<LoopList> insets;
	LoopList interiors;

	regioner.insetsForSlice(outlines, layermeasure, insets, interiors);

	std::list<LoopList> &spurs = triangleSpurLoops;

	regioner.spurLoopsForSlice(outlines, insets, layermeasure, spurs);

	loopToSVG(triangleSpurShell, "black", 20, 20);
	loopTableToSVG(insets, "red", 20, 20);
	loopTableToSVG(spurs, "green", 20, 20);
	svgEnd();

	cout << "Shells with spurs" << endl;
	CPPUNIT_ASSERT_EQUAL(3, (int)spurs.size());
	cout << "Spurs for shell" << endl;
	CPPUNIT_ASSERT_EQUAL(1, (int)spurs.front().size());
}

void InsetsTestCase::testTriangleSpurFill() {
	Regioner regioner(config);

	OpenPathList spurs;
	regioner.fillSpurLoops(triangleSpurLoops.front(), layermeasure, spurs);
}
