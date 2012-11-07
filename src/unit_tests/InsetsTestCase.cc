#include <vector>
#include <iostream>

#include "UnitTestUtils.h"
#include "InsetsTestCase.h"
#include "mgl/regioner.h"
#include "mgl/LineSegment2.h"
#include "mgl/intersection_index.h"
#include "mgl/basic_boxlist.h"
#include "mgl/dump_restore.h"

using namespace std;
using namespace mgl;
using namespace libthing;

CPPUNIT_TEST_SUITE_REGISTRATION( InsetsTestCase );


bool begun = false;

void svgBegin() {
	cerr << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?><svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" x=\"loops\" xmlns:xlink=\"http://www.w3.org/1999/xlink\"  >\"" << endl;
    begun = true;
}

void svgEnd() {
	cerr << "</svg>" << endl;
}

void segToSVG(const LineSegment2 seg, const string &color,
			  const Scalar xoff, const Scalar yoff) {
    if (!begun) return;

	cerr << "<line x1=\"" << (xoff + seg.a.x) * 10 
		 << "\" y1=\""  << (yoff + seg.a.y) * 10
		 << "\" x2=\"" << (xoff + seg.b.x) * 10 
		 << "\" y2=\"" << (yoff + seg.b.y) * 10
		 << "\"	style=\"stroke-width: 1; stroke: " << color << ";\"/>" << endl;
}

void openPathToSVG(const OpenPath &path, const string &color,
				   const Scalar xoff, const Scalar yoff) {
	for (OpenPath::const_iterator point = path.fromStart();
		 point != path.end(); ++point) {
		LineSegment2 seg = path.segmentAfterPoint(point);
		segToSVG(seg, color, xoff, yoff);
	}
}

void openPathListToSVG(const OpenPathList &paths, const string &color, 
					   const Scalar xoff, const Scalar yoff) {
	for (OpenPathList::const_iterator path = paths.begin();
		 path != paths.end(); ++path) {
		openPathToSVG(*path, color, xoff, yoff);
	}
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

//needed because svg is stupid
void makeLoopsPositive(LoopList &loops) {
    Scalar minX = 9999999;
    Scalar minY = 9999999;

    for (LoopList::const_iterator loop = loops.begin();
         loop != loops.end(); ++loop) {
        for (Loop::const_finite_cw_iterator pn = loop->clockwiseFinite();
             pn != loop->clockwiseEnd(); ++pn) {
            Vector2 point = pn->getPoint();
            if (point.x < minX)
                minX = point.x;
            if (point.y < minY)
                minY = point.y;
        }
    }

    for (LoopList::iterator loop = loops.begin();
         loop != loops.end(); ++loop) {
        for (Loop::finite_cw_iterator pn = loop->clockwiseFinite();
             pn != loop->clockwiseEnd(); ++pn) {
            Vector2 point = pn->getPoint();

            point.x -= minX;
            point.y -= minY;

            pn->setPoint(point);
        }
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

	at =
		triangleSpurLoop.insertPointAfter(Vector2(0,0),
										  triangleSpurLoop.clockwiseEnd());
	at = triangleSpurLoop.insertPointAfter(Vector2(0, 10), at);
	at = triangleSpurLoop.insertPointAfter(Vector2(1, 0), at);

    triangleSpurWalls.first = LineSegment2(Vector2(0, 0), Vector2(0, 10));
    triangleSpurWalls.second = LineSegment2(Vector2(0, 10), Vector2(1, 0));
    
    triangleSpurWalls.first = LineSegment2(Vector2(0, 0), Vector2(0, 10));
    triangleSpurWalls.second = LineSegment2(Vector2(1, 0), Vector2(0, 10));
    
    revTriangleSpurWalls.first = LineSegment2(Vector2(0, 0), Vector2(0, 10));
    revTriangleSpurWalls.second = LineSegment2(Vector2(1, 10), Vector2(0, 0));

    parallelSpurWalls.first = LineSegment2(Vector2(0, 0), Vector2(.5, 5));
    parallelSpurWalls.second = LineSegment2(Vector2(2, 10), Vector2(1, 0));

    at = twoPairShell.insertPointAfter(Vector2(0, 0),
                                       twoPairShell.clockwiseEnd());
    at = twoPairShell.insertPointAfter(Vector2(0, 10), at);
    at = twoPairShell.insertPointAfter(Vector2(2, 20), at);
    at = twoPairShell.insertPointAfter(Vector2(1, 9), at);
    at = twoPairShell.insertPointAfter(Vector2(1, 0), at);

    at = threePairShell.insertPointAfter(Vector2(0, 0),
                                       threePairShell.clockwiseEnd());
    at = threePairShell.insertPointAfter(Vector2(1, 10), at);
    at = threePairShell.insertPointAfter(Vector2(1, 20), at);
    at = threePairShell.insertPointAfter(Vector2(2.5, 25), at);
    at = threePairShell.insertPointAfter(Vector2(3.1, 25), at);
    at = threePairShell.insertPointAfter(Vector2(2, 20), at);
    at = threePairShell.insertPointAfter(Vector2(2, 10), at);

    Json::Value stretchletval;
    Json::Reader reader;
    ifstream file("inputs/Stretchlet_layer.json");
    reader.parse(file, stretchletval);
    restoreLoopList(stretchletval, stretchletLoops);
    makeLoopsPositive(stretchletLoops);
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

	//svgBegin();

	LoopList outlines;
	outlines.push_back(squareSpurShell);
	std::list<LoopList> insets;
	LoopList interiors;

	regioner.insetsForSlice(outlines, layermeasure, insets, interiors);

	std::list<LoopList> spurs;
	regioner.spurLoopsForSlice(outlines, insets, layermeasure, spurs);

	/*loopToSVG(squareSpurShell, "black", 20, 20);
	loopTableToSVG(insets, "red", 20, 20);
	loopTableToSVG(spurs, "green", 20, 20);
	svgEnd();*/

	CPPUNIT_ASSERT_EQUAL(3, (int)spurs.size());
	CPPUNIT_ASSERT_EQUAL(1, (int)spurs.front().size());

}

void InsetsTestCase::testTriangleSpurRegion() {
	Regioner regioner(config);

	//svgBegin();

	LoopList outlines;
	outlines.push_back(triangleSpurShell);
	std::list<LoopList> insets;
	LoopList interiors;

	regioner.insetsForSlice(outlines, layermeasure, insets, interiors);

	std::list<LoopList> spurs;

	regioner.spurLoopsForSlice(outlines, insets, layermeasure, spurs);

	/*loopToSVG(triangleSpurShell, "black", 20, 20);
	loopTableToSVG(insets, "red", 20, 20);
	loopTableToSVG(spurs, "green", 20, 20);
	svgEnd();*/

	CPPUNIT_ASSERT_EQUAL(3, (int)spurs.size());
	CPPUNIT_ASSERT_EQUAL(1, (int)spurs.front().size());
}

void InsetsTestCase::testTriangleSpurFill() {
	Regioner regioner(config);

	LoopList triangleSpurLoops;
	triangleSpurLoops.push_back(triangleSpurLoop);

	OpenPathList spurs;
	regioner.fillSpurLoops(triangleSpurLoops, layermeasure, spurs);

	/*svgBegin();
    loopToSVG(triangleSpurLoop, "black", 20, 20);
	openPathListToSVG(spurs, "red", 20, 20);
	svgEnd();*/

	CPPUNIT_ASSERT_EQUAL(1, (int)spurs.size());
}

typedef pair<LineSegment2, LineSegment2> SegmentPair;
bool VectorLess(const Vector2 &first, const Vector2 &second);

bool SegLess(const LineSegment2 &first, const LineSegment2 &second);

struct SegPairLess {
	bool operator()(const SegmentPair first, const SegmentPair second) {
		if (SegLess(first.first, second.first))
			return true;
		else if (SegLess(second.first, first.first))
			return false;
		else if (SegLess(first.second, second.second))
			return true;
		else 
			return false;
	}
};

typedef set<SegmentPair, SegPairLess> SegmentPairSet;
typedef vector<LineSegment2> SegmentList;
typedef basic_boxlist<LineSegment2> SegmentIndex;

void findWallPairs(const Scalar span, const SegmentList segs,
				   SegmentIndex &index, SegmentPairSet &walls);

void InsetsTestCase::testFindWallPairs() {
	Regioner regioner(config);

    //get loop line segments
	SegmentList segs;
	SegmentIndex index;

    for (Loop::finite_cw_iterator pn = triangleSpurLoop.clockwiseFinite();
         pn != triangleSpurLoop.clockwiseEnd(); ++pn) {
        segs.push_back(LineSegment2());
        LineSegment2 &seg = segs.back();
        seg = triangleSpurLoop.segmentAfterPoint(pn);

        index.insert(seg);
    }

	//find wall pairs
	SegmentPairSet allWalls;
	findWallPairs(layermeasure.getLayerW() * 1.5, segs, index, allWalls);
    
    /*svgBegin();
    for (SegmentPairSet::iterator walls = allWalls.begin();
         walls != allWalls.end(); ++walls) {

        segToSVG(walls->first, "black", 20, 20);
        segToSVG(walls->second, "blue", 20, 20);
    }
    svgEnd();*/

    CPPUNIT_ASSERT_EQUAL(1, (int)allWalls.size());
}
Vector2 midPoint(const LineSegment2 &seg);
LineSegment2 getSegmentNormal(const LineSegment2 &orig,
							  const Vector2 &startingPoint,
							  const Scalar length);

void InsetsTestCase::testStretchletWallPairs() {
	Regioner regioner(config);

    LayerMeasure reallayer(0.27, 0.27, 1.6);

    //get loop line segments
	SegmentList segs;
	SegmentIndex index;

    for (LoopList::const_iterator loop = stretchletLoops.begin();
         loop != stretchletLoops.end(); ++loop) {
        for (Loop::const_finite_cw_iterator pn = loop->clockwiseFinite();
             pn != loop->clockwiseEnd(); ++pn) {
            LineSegment2 seg = loop->segmentAfterPoint(pn);

            if (seg.squaredLength() > 0.00001) {
                segs.push_back(seg);
                index.insert(seg);
            }
        }
    }

	//find wall pairs
	SegmentPairSet allWalls;
	findWallPairs(reallayer.getLayerW() * 1.5, segs, index, allWalls);
    
    svgBegin();
    loopsToSVG(stretchletLoops, "red", 0, 0);

    for (SegmentList::const_iterator seg = segs.begin();
         seg != segs.end(); ++seg) {
        segToSVG(getSegmentNormal(*seg, seg->a,
                                  reallayer.getLayerW() * 1.5),
                                  "orange", 0, 0);
        segToSVG(getSegmentNormal(*seg, seg->b,
                                  reallayer.getLayerW() * 1.5),
                                  "orange", 0, 0);
        /*segToSVG(getSegmentNormal(*seg, seg->a,
                                  reallayer.getLayerW() * -1.5),
                                  "orange", 0, 0);
        segToSVG(getSegmentNormal(*seg, seg->b,
                                  reallayer.getLayerW() * -1.5),
                                  "orange", 0, 0);*/
    }
        

    for (SegmentPairSet::iterator walls = allWalls.begin();
         walls != allWalls.end(); ++walls) {

        segToSVG(walls->first, "black", 0, 0);
        segToSVG(walls->second, "blue", 0, 0);

        segToSVG(LineSegment2(midPoint(walls->first), midPoint(walls->second)),
               "green", 0, 0);
        /*segToSVG(getSegmentNormal(walls->first, walls->first.a,
                                  reallayer.getLayerW() * 1.5),
                                  "green", 0, 0);
        segToSVG(getSegmentNormal(walls->first, walls->first.b,
                                  reallayer.getLayerW() * 1.5),
                                  "green", 0, 0);
        segToSVG(getSegmentNormal(walls->first, walls->second.a,
                                  reallayer.getLayerW() * 1.5),
                                  "green", 0, 0);
        segToSVG(getSegmentNormal(walls->first, walls->second.b,
                                  reallayer.getLayerW() * 1.5),
                                  "green", 0, 0);

        segToSVG(getSegmentNormal(walls->first, walls->first.a,
                                  reallayer.getLayerW() * -1.5),
                                  "green", 0, 0);
        segToSVG(getSegmentNormal(walls->first, walls->first.b,
                                  reallayer.getLayerW() * -1.5),
                                  "green", 0, 0);
        segToSVG(getSegmentNormal(walls->first, walls->second.a,
                                  reallayer.getLayerW() * -1.5),
                                  "green", 0, 0);
        segToSVG(getSegmentNormal(walls->first, walls->second.b,
                                  reallayer.getLayerW() * -1.5),
                                  "green", 0, 0);*/
    }
    svgEnd();

    CPPUNIT_ASSERT_EQUAL(1, (int)allWalls.size());
}

double sigdig(double subject, int digits) {
    int power  = 1;
    for (int i = 0; i < digits; i++)
        power *= 10;

    int tmp = (int)(subject * power);
    return (double)tmp / power;
}

bool completeTrapezoid(const Scalar toplen, Scalar bottomlen,
                       const SegmentPair &sides, SegmentPair &spans);

void InsetsTestCase::testCompleteTrapezoid() {
    SegmentPair spans;
    completeTrapezoid(layermeasure.getLayerW() *0.5,
                      layermeasure.getLayerW() *1.5, triangleSpurWalls, spans);

    /*svgBegin();
    segToSVG(triangleSpurWalls.first, "black", 20, 20);
    segToSVG(triangleSpurWalls.second, "black", 20, 20);

    segToSVG(spans.first, "red", 20, 20);
    segToSVG(spans.second, "red", 20, 20);
    svgEnd();*/

    CPPUNIT_ASSERT_EQUAL(layermeasure.getLayerW() *0.5,
                         sigdig(spans.first.length(), 5));
    CPPUNIT_ASSERT_EQUAL(layermeasure.getLayerW() *1.5,
                         sigdig(spans.second.length(), 5));
}

void InsetsTestCase::testCompleteTrapezoidRev() {
    SegmentPair spans;
    completeTrapezoid(layermeasure.getLayerW() *0.5,
                      layermeasure.getLayerW() *1.5, revTriangleSpurWalls, spans);

    /*svgBegin();
    segToSVG(revTriangleSpurWalls.first, "black", 20, 20);
    segToSVG(revTriangleSpurWalls.second, "black", 20, 20);

    segToSVG(spans.first, "red", 20, 20);
    segToSVG(spans.second, "red", 20, 20);
    svgEnd();*/

    CPPUNIT_ASSERT_EQUAL(layermeasure.getLayerW() *0.5,
                         sigdig(spans.first.length(), 5));
    CPPUNIT_ASSERT_EQUAL(layermeasure.getLayerW() *1.5,
                         sigdig(spans.second.length(), 5));
}

void InsetsTestCase::testCompleteParallel() {
    SegmentPair spans;
    completeTrapezoid(layermeasure.getLayerW() *0.5,
                      layermeasure.getLayerW() *1.5, parallelSpurWalls, spans);

    /*svgBegin();
    segToSVG(parallelSpurWalls.first, "black", 20, 20);
    segToSVG(parallelSpurWalls.second, "black", 20, 20);

    segToSVG(spans.first, "red", 20, 20);
    segToSVG(spans.second, "red", 20, 20);
    svgEnd();*/

    CPPUNIT_ASSERT(layermeasure.getLayerW() - 
                     sigdig(spans.first.length(), 5) < 0.01);
    CPPUNIT_ASSERT(layermeasure.getLayerW() -
                     sigdig(spans.second.length(), 5) < 0.01);
}

bool bisectWalls(Scalar minSpurWidth, Scalar maxSpurWidth,
                 const SegmentPair &walls, LineSegment2 &bisect);

void InsetsTestCase::testBisectWalls() {
    LineSegment2 bisect;
    bisectWalls(layermeasure.getLayerW() * 0.5,
                layermeasure.getLayerW() * 1.5,
                triangleSpurWalls, bisect);

    /*svgBegin();
    segToSVG(triangleSpurWalls.first, "black", 20, 20);
    segToSVG(triangleSpurWalls.second, "black", 20, 20);

    segToSVG(bisect, "red", 20, 20);
    svgEnd();*/

    //check that we haven't reversed anything
    CPPUNIT_ASSERT(bisect.a.x > 0);
    CPPUNIT_ASSERT(bisect.a.y > 0);
    CPPUNIT_ASSERT(bisect.b.x > 0);
    CPPUNIT_ASSERT(bisect.b.y < 0); //this is actually supposed to be negative
}

void InsetsTestCase::testBisectReverseWalls() {
    LineSegment2 bisect;
    bisectWalls(layermeasure.getLayerW() * 0.5,
                layermeasure.getLayerW() * 1.5,
                revTriangleSpurWalls, bisect);

    /*svgBegin();
    segToSVG(triangleSpurWalls.first, "black", 20, 20);
    segToSVG(triangleSpurWalls.second, "black", 20, 20);

    segToSVG(bisect, "red", 20, 20);
    svgEnd();*/

    //check that we haven't reversed anything
    CPPUNIT_ASSERT(bisect.a.x > 0);
    CPPUNIT_ASSERT(bisect.a.y > 0);
    CPPUNIT_ASSERT(bisect.b.x > 0);
    CPPUNIT_ASSERT(bisect.b.y > 0);
}

void InsetsTestCase::testTwoPairFill() {
	Regioner regioner(config);

	LoopList loops;
	loops.push_back(twoPairShell);

	OpenPathList spurs;
	regioner.fillSpurLoops(loops, layermeasure, spurs);

	/*svgBegin();
    loopToSVG(twoPairShell, "black", 20, 20);
	openPathListToSVG(spurs, "red", 20, 20);
	svgEnd();*/

	CPPUNIT_ASSERT_EQUAL(3, (int)spurs.size());
}

void InsetsTestCase::testThreePairFill() {
	Regioner regioner(config);
	svgBegin();

	LoopList loops;
	loops.push_back(threePairShell);

	OpenPathList spurs;
	regioner.fillSpurLoops(loops, layermeasure, spurs);

    loopToSVG(threePairShell, "black", 20, 20);
	openPathListToSVG(spurs, "red", 20, 20);
	svgEnd();

	CPPUNIT_ASSERT_EQUAL(5, (int)spurs.size());
}

void InsetsTestCase::testStretchlet() {
    Regioner regioner(config);

    LayerMeasure reallayer(0.27, 0.27, 1.6);

    OpenPathList spurs;
    regioner.fillSpurLoops(stretchletLoops, reallayer, spurs);

    svgBegin();
    loopsToSVG(stretchletLoops, "black", 0, 0);
    openPathListToSVG(spurs, "red", 0, 0);
    svgEnd();
}
