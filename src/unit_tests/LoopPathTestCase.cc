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
	cout << "No setup" << endl;
}

void LoopPathTestCase::testOpenPathBasic() {
	cout << "Testing basic OpenPath functionality" << endl;

	OpenPath path;

	path.appendPoint(Vector2(5, 5));
	OpenPath::iterator i = path.fromStart();
	Vector2 first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 5.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 5.0);

	path.appendPoint(Vector2(5, 6));
	i = path.fromStart();
	++i;
	Vector2 second = *i;
	CPPUNIT_ASSERT_EQUAL(second.x, 5.0);
	CPPUNIT_ASSERT_EQUAL(second.y, 6.0);

	path.prependPoint(Vector2(6, 5));
	i = path.fromStart();
	first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 6.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 5.0);

	LineSegment2 seg = path.segmentAfterPoint(i);
	CPPUNIT_ASSERT_EQUAL(seg.length(), 1.0);

	OpenPath::iterator ep = path.getEntryPoints();
	Vector2 first_ep = *ep;
	Vector2 start = *(path.fromStart());
	CPPUNIT_ASSERT_EQUAL(first_ep.x, start.x);
	CPPUNIT_ASSERT_EQUAL(first_ep.y, start.y);
	
	Vector2 second_ep = *ep;
	Vector2 end = *(path.fromEnd());
	CPPUNIT_ASSERT_EQUAL(second_ep.x, end.x);
	CPPUNIT_ASSERT_EQUAL(second_ep.y, end.y);
}

void LoopPathTestCase::testLoopBasic() {
	
}

void LoopPathTestCase::testLoopPathBasic() {
	
}

void LoopPathTestCase::testOpenPathJoin() {

}

void LoopPathTestCase::testOpenToLoopPathJoin() {
	

}
