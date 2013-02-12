#include "UnitTestUtils.h"
#include "DumpRestoreTestCase.h"
#include "mgl/dump_restore.h"

#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/reader.h>
#include <iostream>

using namespace std;
using namespace mgl;
using namespace libthing;

CPPUNIT_TEST_SUITE_REGISTRATION( DumpRestoreTestCase );


void DumpRestoreTestCase::setUp() {
	Loop::cw_iterator at =
		loop1.insertPointAfter(Vector2(10.0, 10.0), loop1.clockwiseEnd());
	at = loop1.insertPointAfter(Vector2(10.0, -10.0), at);
	at = loop1.insertPointAfter(Vector2(-10.0, -10.0), at);
	at = loop1.insertPointAfter(Vector2(-10.0, 10.0), at);

	at =
		loop2.insertPointAfter(Vector2(0,0),
										  loop2.clockwiseEnd());
	at = loop2.insertPointAfter(Vector2(0, 10), at);
	at = loop2.insertPointAfter(Vector2(1, 0), at);

    point.x = 5.5;
    point.y = 6.6;

    pointstr = "{\"type\":\"Point\",\"x\":5.50,\"y\":6.60}\n";
    loopstr = "{\"points\":[{\"type\":\"Point\",\"x\":10.0,\"y\":-10.0},{\"type\":\"Point\",\"x\":-10.0,\"y\":-10.0},{\"type\":\"Point\",\"x\":-10.0,\"y\":10.0},{\"type\":\"Point\",\"x\":10.0,\"y\":10.0}],\"type\":\"Loop\"}\n";
    loopliststr = "{\"loops\":[{\"points\":[{\"type\":\"Point\",\"x\":10.0,\"y\":-10.0},{\"type\":\"Point\",\"x\":-10.0,\"y\":-10.0},{\"type\":\"Point\",\"x\":-10.0,\"y\":10.0},{\"type\":\"Point\",\"x\":10.0,\"y\":10.0}],\"type\":\"Loop\"},{\"points\":[{\"type\":\"Point\",\"x\":0.0,\"y\":10.0},{\"type\":\"Point\",\"x\":1.0,\"y\":0.0},{\"type\":\"Point\",\"x\":0.0,\"y\":0.0}],\"type\":\"Loop\"}],\"type\":\"LoopList\"}\n";
}

void DumpRestoreTestCase::testDumpPoint() {
    Json::Value pointval;
    dumpPoint(point, pointval);

    CPPUNIT_ASSERT(!pointval.isNull());

    Json::FastWriter writer;
    //cout << writer.write(pointval);

    CPPUNIT_ASSERT_EQUAL(pointstr, writer.write(pointval));
}

void DumpRestoreTestCase::testDumpLoop() {
    Json::Value loopval;
    dumpLoop(loop1, loopval);

    CPPUNIT_ASSERT(!loopval.isNull());

    Json::FastWriter writer;
    //cout << writer.write(loopval);

    CPPUNIT_ASSERT_EQUAL(loopstr, writer.write(loopval));
}

void DumpRestoreTestCase::testDumpLoopList() {
    LoopList list;
    list.push_back(loop1);
    list.push_back(loop2);

    Json::Value listval;
    dumpLoopList(list, listval);

    CPPUNIT_ASSERT(!listval.isNull());
    
    Json::FastWriter writer;
    //cout << writer.write(listval);

    CPPUNIT_ASSERT_EQUAL(loopliststr, writer.write(listval));
}

void DumpRestoreTestCase::testRestorePoint() {
    Json::Reader reader;
    Json::Value pointval;
    reader.parse(pointstr, pointval);

    Vector2 point;
    restorePoint(pointval, point);

    CPPUNIT_ASSERT_EQUAL(5.5, point.x);
    CPPUNIT_ASSERT_EQUAL(6.6, point.y);
}

void DumpRestoreTestCase::testRestoreLoop() {
    Json::Reader reader;
    Json::Value loopval;
    reader.parse(loopstr, loopval);

    Loop loop;
    restoreLoop(loopval, loop);

    CPPUNIT_ASSERT_EQUAL((size_t)4, loop.size());
}

void DumpRestoreTestCase::testRestoreLoopList() {
    Json::Reader reader;
    Json::Value listval;
    reader.parse(loopliststr, listval);

    LoopList list;
    restoreLoopList(listval, list);

    CPPUNIT_ASSERT_EQUAL((size_t)2, list.size());
}
