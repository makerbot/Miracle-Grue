#include <vector>

#include "UnitTestUtils.h"
#include "LoopPathTestCase.h"

#include "mgl/loop_path.h"
#include "mgl/insets.h"
#include "mgl/loop_utils.h"

#include <iostream>
#include <sstream>
#include <list>

using namespace std;
using namespace mgl;
using namespace libthing;

CPPUNIT_TEST_SUITE_REGISTRATION( LoopPathTestCase );

void LoopPathTestCase::setUp() {
	cout << endl << "No setup" << endl;
}

void LoopPathTestCase::testOpenPathBasic() {
	cout << "Testing basic OpenPath functionality" << endl;

	OpenPath path;

	path.appendPoint(PointType(5, 5));
	OpenPath::iterator i = path.fromStart();
	PointType first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 5.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 5.0);

	++i;
	CPPUNIT_ASSERT(i == path.end());

	path.appendPoint(PointType(5, 6));
	i = path.fromStart();
	++i;
	PointType second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 5.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 6.0);

	++i;
	CPPUNIT_ASSERT(i == path.end());

	path.prependPoint(PointType(6, 5));
	i = path.fromStart();
	first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 6.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 5.0);

	SegmentType seg = path.segmentAfterPoint(i);
	CPPUNIT_ASSERT_EQUAL(seg.length(), 1.0);

	OpenPath::entry_iterator entry = path.entryBegin();
	PointType first_entry = *entry;
	PointType start = *(path.fromStart());
	CPPUNIT_ASSERT_EQUAL(first_entry.x, start.x);
	CPPUNIT_ASSERT_EQUAL(first_entry.y, start.y);
	
	++entry;
	PointType second_entry = *entry;
	PointType end = *(path.fromEnd());
	CPPUNIT_ASSERT_EQUAL(second_entry.x, end.x);
	CPPUNIT_ASSERT_EQUAL(second_entry.y, end.y);

	PointType first_exit = path.getExitPoint(first_entry);
	CPPUNIT_ASSERT_EQUAL(first_exit.x, second_entry.x);
	CPPUNIT_ASSERT_EQUAL(first_exit.y, second_entry.y);

	PointType second_exit = path.getExitPoint(second_entry);
	CPPUNIT_ASSERT_EQUAL(second_exit.x, first_entry.x);
	CPPUNIT_ASSERT_EQUAL(second_exit.y, first_entry.y);
}

void LoopPathTestCase::testLoopBasic() {
	cout << "Testing single point loop" << endl;
	
	Loop loop(PointType(2, 3));

	Loop::cw_iterator i = loop.clockwise();
	PointType first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 3.0);

	++i;
	PointType second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 3.0);

	cout << "Testing two point loop" << endl;
	i = loop.insertPointAfter(PointType(4, 5), i);
	first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 4.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 5.0);

	++i;
	second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 3.0);

	++i;
	first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 4.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 5.0);

	cout << "Testing three point loop" << endl;
	i = loop.insertPointAfter(PointType(6, 7), i);
	Loop::ccw_iterator j = loop.counterClockwise(*i);
	first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 6.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 7.0);
	
	++i;
	second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 3.0);

	++i;
	PointType third = *i;
	CPPUNIT_ASSERT_EQUAL(third.x, 4.0);
	CPPUNIT_ASSERT_EQUAL(third.y, 5.0);

	++i;
	PointType fourth = *i;
	CPPUNIT_ASSERT_EQUAL(fourth.x, 6.0);
	CPPUNIT_ASSERT_EQUAL(fourth.y, 7.0);
	
	cout << "Testing segmentAfterPoint" << endl;
	SegmentType seg = loop.segmentAfterPoint(i);
	CPPUNIT_ASSERT_EQUAL(seg.squaredLength(), 
			SegmentType(PointType(6,7), PointType(2,3)).squaredLength());

	cout << "Testing entry points" << endl;
	int count = 0;
	Loop::entry_iterator entry = loop.entryBegin();
	for (;	entry != loop.entryEnd(); entry++){
		cout << "Entry " << count << " :\t" << *entry << endl;
		++count;
	}

	CPPUNIT_ASSERT_EQUAL(count, 3);

	PointType exitpoint = loop.getExitPoint(entry);
	PointType entrypoint = *entry;
	CPPUNIT_ASSERT_EQUAL(entrypoint.x, exitpoint.x);
	CPPUNIT_ASSERT_EQUAL(entrypoint.y, exitpoint.y);
}

void LoopPathTestCase::testLoopPathBasic() {
	Loop loop(PointType(1, 2));

	Loop::cw_iterator last = loop.insertPointAfter(PointType(3, 4), loop.clockwise());
	
	cout << "Check that we are properly finding iterators" << endl;
	Loop::const_cw_iterator cwstart(loop.clockwise(PointType(1,2)));
	Loop::const_ccw_iterator ccwstart(loop.counterClockwise(PointType(1,2)));
	CPPUNIT_ASSERT(cwstart != loop.clockwiseEnd());
	CPPUNIT_ASSERT(ccwstart != loop.counterClockwiseEnd());
	cout << "Finding iterators works!" << endl;
	LoopPath lp(loop, cwstart, ccwstart);
	
	cout << "Loop Path:" << endl;
	for(LoopPath::iterator i = lp.fromStart(); 
			i != lp.end(); 
			++i) {
		cout << *i << endl;
	}

	LoopPath::iterator i = lp.fromStart();
	PointType first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 1.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 2.0);

	++i;
	PointType second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 3.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 4.0);
	
	++i;
	PointType third = *i;
	CPPUNIT_ASSERT_EQUAL(third.x, 1.0);
	CPPUNIT_ASSERT_EQUAL(third.y, 2.0);

	++i;
	bool res = i == lp.end();
	CPPUNIT_ASSERT(res);
}

void LoopPathTestCase::testOpenPathJoin() {
	OpenPath path1;
	path1.appendPoint(PointType(1, 1));
	path1.appendPoint(PointType(2, 2));
	
	cout << "Path 1:" << endl;
	for(OpenPath::iterator i = path1.fromStart(); 
			i != path1.end(); 
			++i) {
		cout << *i << endl;
	}

	OpenPath path2;
	path2.appendPoint(PointType(3, 3));
	path2.appendPoint(PointType(4, 4));
	
	cout << "Path 2:" << endl;
	for(OpenPath::iterator i = path2.fromStart(); 
			i != path2.end(); 
			++i) {
		cout << *i << endl;
	}

	OpenPath joined;
	joined.appendPoints(path1.fromStart(), path1.end());
	joined.appendPoints(path2.fromStart(), path2.end());
	
	cout << "Joined Path:" << endl;
	for(OpenPath::iterator i = joined.fromStart(); 
			i != joined.end(); 
			++i) {
		cout << *i << endl;
	}

	OpenPath::iterator i = joined.fromStart();

	PointType point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 1.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 1.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 2.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 3.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 3.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 4.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 4.0);

	OpenPath reversed;
	reversed.appendPoints(path2.fromEnd(), path2.rend());
	reversed.appendPoints(path1.fromEnd(), path1.rend());
	
	cout << "Reversed Path:" << endl;
	for(OpenPath::iterator i = reversed.fromStart(); 
			i != reversed.end(); 
			++i) {
		cout << *i << endl;
	}

	i = reversed.fromStart();

	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 4.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 4.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 3.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 3.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 2.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 1.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 1.0);

	OpenPath mixed;
	mixed.appendPoints(path1.fromStart(), path1.end());
	mixed.appendPoints(path2.fromEnd(), path2.rend());

	i = mixed.fromStart();

	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 1.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 1.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 2.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 4.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 4.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 3.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 3.0);
	
}

void LoopPathTestCase::testOpenToLoopPathJoin() {
	OpenPath open;
	open.appendPoint(PointType(1, 1));
	open.appendPoint(PointType(2, 2));

	Loop loop(PointType(3, 3));
	Loop::cw_iterator insert = loop.clockwise();
	insert = loop.insertPointAfter(PointType(3, 4), insert);
	insert = loop.insertPointAfter(PointType(4, 3), insert);

	LoopPath lp(loop, loop.clockwise(PointType(3, 3)),
				loop.counterClockwise(PointType(3, 3)));

	OpenPath joined;
	joined.appendPoints(open.fromStart(), open.end());
	joined.appendPoints(lp.fromStart(), lp.end());
	
	cout << "Open Path:" << endl;
	for(OpenPath::iterator i = open.fromStart(); 
			i != open.end(); 
			++i) {
		cout << *i << endl;
	}
	cout << "Loop:" << endl;
	bool moved = false;
	for(Loop::cw_iterator i = loop.clockwise(PointType(3,3)); 
			i != loop.clockwise(PointType(3,3)) || !moved; 
			++i, moved = true) {
		cout << *i << endl;
	}
	cout << "Joined Path:" << endl;
	for(OpenPath::iterator i = joined.fromStart(); 
			i != joined.end(); 
			++i) {
		cout << *i << endl;
	}
	OpenPath::iterator i = joined.fromStart();

	PointType point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 1.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 1.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 2.0);
	
	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 3.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 3.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 3.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 4.0);

	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 4.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 3.0);
					 
	++i;
	point = *i;
	CPPUNIT_ASSERT_EQUAL(point.x, 3.0);
	CPPUNIT_ASSERT_EQUAL(point.y, 3.0);

	++i;
	CPPUNIT_ASSERT(i == joined.end());
}

void LoopPathTestCase::testConstPath() {
	OpenPath path;
	const OpenPath& constpath = path;
	
	for(Scalar x = 0.0; x < 10.0; ++x)
		path.appendPoint(PointType(x,x));
	
	OpenPath::iterator regular = path.end();
	OpenPath::const_iterator consted = constpath.end();
	
	for(regular = path.fromStart(), consted = constpath.fromStart(); 
			regular != path.end() && 
			consted != constpath.end(); 
			++regular, ++consted){
		cout << "Normal : " << *regular << endl;
		cout << "Consted: " << *consted << endl;
		CPPUNIT_ASSERT_EQUAL(*regular, *consted);
	}
}

void LoopPathTestCase::testConstLoop() {
	Loop loop;
	const Loop& constloop = loop;
	Loop::cw_iterator iter = loop.clockwise();
	for(Scalar x = 1.0; x < 10.0; ++x)
		iter = loop.insertPointAfter(PointType(x,x), iter);
	
	Loop::cw_iterator regulariter = ++loop.clockwise();
	Loop::const_cw_iterator constiter = ++constloop.clockwise();
	
	for(; regulariter != loop.clockwise() && 
			constiter != constloop.clockwise();
			++regulariter, ++constiter){
		cout << "Normal : " << *regulariter << endl;
		cout << "Consted: " << *constiter << endl;
		CPPUNIT_ASSERT_EQUAL(*regulariter, *constiter);
	}
}

void LoopPathTestCase::testConstLoopPath() {
	Loop loop;
	Loop::cw_iterator iter = loop.clockwiseEnd();
	for(Scalar x = 1.0; x < 10.0; ++x)
		iter = loop.insertPointAfter(PointType(x,x), iter);
	
	LoopPath looppath(loop, loop.clockwise(*loop.clockwise()), 
			loop.counterClockwise(*loop.clockwise()));
	const LoopPath& constlooppath = looppath;
	
	LoopPath::iterator regular = looppath.fromStart();
	LoopPath::const_iterator consted = constlooppath.fromStart();
	
	for(; regular != looppath.end() && consted != constlooppath.end(); 
			++regular, ++consted){
		cout << "Normal : " << *regular << endl;
		cout << "Consted: " << *consted << endl;
		CPPUNIT_ASSERT_EQUAL(*regular, *consted);
	}
}

void LoopPathTestCase::testFiniteSegment() {
	Loop loop;
	Loop::cw_iterator iter = loop.clockwiseEnd();
	std::list<SegmentType> lines;
	for(Scalar x = 1.0; x < 10.0; ++x){
		PointType current(x,x);
		iter = loop.insertPointAfter(current, iter);
	}
	lines.push_back(SegmentType(PointType(2,2), PointType(3,3)));
	lines.push_back(SegmentType(PointType(3,3), PointType(4,4)));
	lines.push_back(SegmentType(PointType(4,4), PointType(5,5)));
	lines.push_back(SegmentType(PointType(5,5), PointType(6,6)));
	lines.push_back(SegmentType(PointType(6,6), PointType(7,7)));
	lines.push_back(SegmentType(PointType(7,7), PointType(8,8)));
	lines.push_back(SegmentType(PointType(8,8), PointType(9,9)));
	lines.push_back(SegmentType(PointType(9,9), PointType(1,1)));
	lines.push_back(SegmentType(PointType(1,1), PointType(2,2)));
	std::list<SegmentType>::iterator liter;
	for(iter = loop.clockwiseFinite(), 
			liter = lines.begin(); 
			iter != loop.clockwiseEnd() && liter != lines.end(); 
			++iter, ++liter) {
		SegmentType line = loop.segmentAfterPoint(iter);
		//cout << "Point: \t" << *iter << endl;
		cout << "Actual Line:   \t" << line.a << line.b << endl;
		cout << "Expected Line: \t" << liter->a << liter->b << endl;
		CPPUNIT_ASSERT_EQUAL(liter->a, line.a);
		CPPUNIT_ASSERT_EQUAL(liter->b, line.b);
	}
}

void LoopPathTestCase::testConvex() {
	std::list<Loop> looplist;
	cout << endl;
	for(Scalar x = 0; x < 4; x+=2) for(Scalar y = 0; y < 4; y+=2) {
		Loop currentLoop;
		cout << endl;
		PointType currentPoint;
		currentPoint = PointType(x + 0, y + 0);
		cout << currentPoint << endl;
		currentLoop.insertPointBefore(currentPoint, currentLoop.clockwiseEnd());
		currentPoint = PointType(x + 0, y + 1);
		cout << currentPoint << endl;
		currentLoop.insertPointBefore(currentPoint, currentLoop.clockwiseEnd());
		currentPoint = PointType(x + 1, y + 1);
		cout << currentPoint << endl;
		currentLoop.insertPointBefore(currentPoint, currentLoop.clockwiseEnd());
		currentPoint = PointType(x + 1, y + 0);
		cout << currentPoint << endl;
		currentLoop.insertPointBefore(currentPoint, currentLoop.clockwiseEnd());
		
		looplist.push_back(currentLoop);
	}
	Loop convexLoop = createConvexLoop(looplist);
	
	SegmentTable outsetSegs;
	outsetSegs.push_back(std::vector<SegmentType>());
	
	for(Loop::finite_cw_iterator iter = convexLoop.clockwiseFinite(); 
			iter != convexLoop.clockwiseEnd(); 
			++iter) {
		outsetSegs.back().push_back(convexLoop.segmentAfterPoint(iter));
	}
	
	ClipperInsetter().inset(outsetSegs, -0.2, outsetSegs);
	
	convexLoop = Loop();
	
	for(std::vector<SegmentType>::const_iterator iter = 
			outsetSegs.back().begin(); 
			iter != outsetSegs.back().end(); 
			++iter) {
		convexLoop.insertPointBefore(iter->b, convexLoop.clockwiseEnd());
	}
	
	
	
	
	
	Loop expectedLoop;
	expectedLoop.insertPointBefore(PointType(3.2,3.2), expectedLoop.clockwiseEnd());
	expectedLoop.insertPointBefore(PointType(3.2,-0.2), expectedLoop.clockwiseEnd());
	expectedLoop.insertPointBefore(PointType(-0.2,-0.2), expectedLoop.clockwiseEnd());
	expectedLoop.insertPointBefore(PointType(-0.2,3.2), expectedLoop.clockwiseEnd());
	
	cout << endl;
	
	for(Loop::finite_cw_iterator iter = convexLoop.clockwiseFinite(); 
			iter != convexLoop.clockwiseEnd(); 
			++iter) {
		cout << iter->getPoint() << endl;
	}
	
	for(Loop::finite_cw_iterator iter = convexLoop.clockwiseFinite(), 
			iterExpected = expectedLoop.clockwiseFinite(); 
			iter != convexLoop.clockwiseEnd() && 
			iterExpected != expectedLoop.clockwiseEnd();
			++iter, ++iterExpected) {
		CPPUNIT_ASSERT_EQUAL(iterExpected->getPoint(), iter->getPoint());
	}
}


