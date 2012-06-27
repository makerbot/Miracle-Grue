#include "UnitTestUtils.h"
#include "LoopPathTestCase.h"

#include "mgl/loop_path.h"

#include <iostream>
#include <sstream>

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

	path.appendPoint(Vector2(5, 5));
	OpenPath::iterator i = path.fromStart();
	Vector2 first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 5.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 5.0);

	++i;
	CPPUNIT_ASSERT(i == path.end());

	path.appendPoint(Vector2(5, 6));
	i = path.fromStart();
	++i;
	Vector2 second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 5.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 6.0);

	++i;
	CPPUNIT_ASSERT(i == path.end());

	path.prependPoint(Vector2(6, 5));
	i = path.fromStart();
	first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 6.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 5.0);

	LineSegment2 seg = path.segmentAfterPoint(i);
	CPPUNIT_ASSERT_EQUAL(seg.length(), 1.0);

	OpenPath::entry_iterator entry = path.getEntryPoints();
	Vector2 first_entry = *entry;
	Vector2 start = *(path.fromStart());
	CPPUNIT_ASSERT_EQUAL(first_entry.x, start.x);
	CPPUNIT_ASSERT_EQUAL(first_entry.y, start.y);
	
	++entry;
	Vector2 second_entry = *entry;
	Vector2 end = *(path.fromEnd());
	CPPUNIT_ASSERT_EQUAL(second_entry.x, end.x);
	CPPUNIT_ASSERT_EQUAL(second_entry.y, end.y);

	Vector2 first_exit = path.getExitPoint(first_entry);
	CPPUNIT_ASSERT_EQUAL(first_exit.x, second_entry.x);
	CPPUNIT_ASSERT_EQUAL(first_exit.y, second_entry.y);

	Vector2 second_exit = path.getExitPoint(second_entry);
	CPPUNIT_ASSERT_EQUAL(second_exit.x, first_entry.x);
	CPPUNIT_ASSERT_EQUAL(second_exit.y, first_entry.y);
}

void LoopPathTestCase::testLoopBasic() {
	cout << "Testing single point loop" << endl;
	
	Loop loop(Vector2(2, 3));

	Loop::cw_iterator i = loop.clockwise();
	Vector2 first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 3.0);

	++i;
	Vector2 second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 3.0);

	cout << "Testing two point loop" << endl;
	i = loop.insertPoint(Vector2(4, 5), i);
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
	i = loop.insertPoint(Vector2(6, 7), i);
	Loop::ccw_iterator j = loop.counterClockwise(*i);
	first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 6.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 7.0);
	
	++i;
	second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 4.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 5.0);

	++i;
	Vector2 third = *i;
	CPPUNIT_ASSERT_EQUAL(third.x, 2.0);
	CPPUNIT_ASSERT_EQUAL(third.y, 3.0);

	++i;
	Vector2 fourth = *i;
	CPPUNIT_ASSERT_EQUAL(fourth.x, 6.0);
	CPPUNIT_ASSERT_EQUAL(fourth.y, 7.0);
	
	cout << "Testing segmentAfterPoint" << endl;
	LineSegment2 seg = loop.segmentAfterPoint(i);
	CPPUNIT_ASSERT_EQUAL(seg.squaredLength(), 8.0);

	cout << "Testing entry points" << endl;
	int count = 1;
	Loop::entry_iterator entry = loop.entryBegin();
	for (;	entry != loop.entryEnd(); entry++)
		count++;

	CPPUNIT_ASSERT_EQUAL(count, 3);

	Vector2 exitpoint = loop.getExitPoint(entry);
	Vector2 entrypoint = *entry;
	CPPUNIT_ASSERT_EQUAL(entrypoint.x, exitpoint.x);
	CPPUNIT_ASSERT_EQUAL(entrypoint.y, exitpoint.y);
}

void LoopPathTestCase::testLoopPathBasic() {
	Loop loop(Vector2(1, 2));

	Loop::cw_iterator last = loop.insertPoint(Vector2(3, 4), loop.clockwise());

	LoopPath lp(loop, loop.clockwise(Vector2(1, 2)), 
			loop.counterClockwise(Vector2(1, 2)));

	LoopPath::iterator i = lp.fromStart();
	Vector2 first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 1.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 2.0);

	++i;
	Vector2 second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 3.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 4.0);

	++i;
	bool res = i == lp.end();
	CPPUNIT_ASSERT(res);
}

void LoopPathTestCase::testOpenPathJoin() {
	OpenPath path1;
	path1.appendPoint(Vector2(1, 1));
	path1.appendPoint(Vector2(2, 2));

	OpenPath path2;
	path2.appendPoint(Vector2(3, 3));
	path2.appendPoint(Vector2(4, 4));

	OpenPath joined;
	joined.appendPoints(path1.fromStart(), path1.end());

	OpenPath::iterator i = joined.fromStart();

	Vector2 point = *i;
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
	open.appendPoint(Vector2(1, 1));
	open.appendPoint(Vector2(2, 2));

	Loop loop(Vector2(3, 3));
	Loop::cw_iterator insert = loop.clockwise();
	insert = loop.insertPoint(Vector2(3, 4), insert);
	insert = loop.insertPoint(Vector2(4, 3), insert);

	LoopPath lp(loop, loop.clockwise(Vector2(3, 3)),
				loop.counterClockwise(Vector2(3, 3)));

	OpenPath joined;
	joined.appendPoints(open.fromStart(), open.end());
	joined.appendPoints(lp.fromStart(), lp.end());

	OpenPath::iterator i = joined.fromStart();

	Vector2 point = *i;
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
