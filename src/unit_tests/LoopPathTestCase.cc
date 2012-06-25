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

	path.appendPoint(Vector2(0, 0));
	OpenPath::iterator i = path.fromStart();
	Vector2 first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 0.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 0.0);

	path.appendPoint(Vector2(0, 1));
	++i;
	Vector2 second = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 0.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 1.0);

	path.prependPoint(Vector2(1, 0));
	i = path.fromStart();
	first = *i;
	CPPUNIT_ASSERT_EQUAL(first.x, 1.0);
	CPPUNIT_ASSERT_EQUAL(first.y, 0.0);

	//LineSegment2 seg = path.segmentAfterPoint(i);
	//CPPUNIT_ASSERT_EQUAL(seg.length(), 1.0);
}

void LoopPathTestCase::testLoopBasic() {
	
}

void LoopPathTestCase::testLoopPathBasic() {
	
}

void LoopPathTestCase::testOpenPathJoin() {

}

void LoopPathTestCase::testOpenToLoopPathJoin() {
	

}
