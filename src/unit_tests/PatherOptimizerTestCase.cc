

#include <cppunit/config/SourcePrefix.h>
#include <list>
#include "UnitTestUtils.h"
#include "PatherOptimizerTestCase.h"
#include "mgl/pather_optimizer.h"

CPPUNIT_TEST_SUITE_REGISTRATION( PatherOptimizerTestCase );

using namespace std;
using namespace mgl;

void PatherOptimizerTestCase::setUp() {
	// does nothing
	cout << endl;
}

void PatherOptimizerTestCase::testBasics() {
	//assert that line intersection works
	Segment2Type seg1(Point2Type(-1,0), Point2Type(1,0));
	Segment2Type seg2(Point2Type(0,-1), Point2Type(0,1));
	cout << "Testing that lines intersect properly..." << endl;
	CPPUNIT_ASSERT(seg1.intersects(seg2));
	pather_optimizer optimizer;
	
	//make a normal triangle loop
	Loop loop;
	loop.insertPointBefore(Point2Type(-1,1), loop.clockwiseEnd());
	loop.insertPointBefore(Point2Type(1,1), loop.clockwiseEnd());
	loop.insertPointBefore(Point2Type(1,-1), loop.clockwiseEnd());
	//loop.insertPointBefore(Point2Type(-1,-1), loop.clockwiseEnd());
	//add loop to optimizer
	cout << "Testing adding of a normal loop..." << endl;
	CPPUNIT_ASSERT_NO_THROW(optimizer.addBoundary(loop));
	CPPUNIT_ASSERT_NO_THROW(optimizer.addPath(loop));
	//make a degenerate loop
	Loop badLoop;
	badLoop.insertPointBefore(Point2Type(-1, 0), badLoop.clockwiseEnd());
	badLoop.insertPointBefore(Point2Type(1, 0), badLoop.clockwiseEnd());
	cout << "Testing adding of a bad loop..." << endl;
	CPPUNIT_ASSERT_THROW(optimizer.addBoundary(badLoop), mgl::Exception);
	CPPUNIT_ASSERT_THROW(optimizer.addPath(badLoop), mgl::Exception);
	
	OpenPath path;
	path.appendPoint(Point2Type(-1, 0));
	path.appendPoint(Point2Type(1, 0));
	//add a path to optimizer
	cout << "Testing adding of a normal path..." << endl;
	CPPUNIT_ASSERT_NO_THROW(optimizer.addBoundary(path));
	CPPUNIT_ASSERT_NO_THROW(optimizer.addPath(path));
	//make a degenerate path
	OpenPath badPath;
	badPath.appendPoint(Point2Type());
	cout << "Testing adding of a bad path..." << endl;
	CPPUNIT_ASSERT_THROW(optimizer.addBoundary(badPath), mgl::Exception);
	CPPUNIT_ASSERT_THROW(optimizer.addPath(badPath), mgl::Exception);
}

void PatherOptimizerTestCase::testBoundary() {
	//make a square +-1 both axes
	Loop loop;
	loop.insertPointBefore(Point2Type(-1,1), loop.clockwiseEnd());
	loop.insertPointBefore(Point2Type(1,1), loop.clockwiseEnd());
	loop.insertPointBefore(Point2Type(1,-1), loop.clockwiseEnd());
	loop.insertPointBefore(Point2Type(-1,-1), loop.clockwiseEnd());
	
	pather_optimizer optimizer;
	
	//make paths that cross boundary and don't
	
	OpenPath insideBoundary;
	insideBoundary.appendPoint(Point2Type(-0.5, 0));
	insideBoundary.appendPoint(Point2Type(0.5, 0));
	OpenPath outsideBoundary;
	outsideBoundary.appendPoint(Point2Type(-0.5, 2));
	outsideBoundary.appendPoint(Point2Type(0.5, 2));
	
	cout << "Adding simple square boundary to optimizer" << endl;
	CPPUNIT_ASSERT_NO_THROW(optimizer.addBoundary(loop));
	cout << "Adding various OpenPaths to optimizer" << endl;
	CPPUNIT_ASSERT_NO_THROW(optimizer.addPath(insideBoundary));
	CPPUNIT_ASSERT_NO_THROW(optimizer.addPath(outsideBoundary));
	
	//get back out a list of OpenPaths
	std::list<OpenPath> optimizedList;
	cout << "Optimizing paths" << endl;
	optimizer.optimize(optimizedList);
	
	cout << "Make sure we get something not empty out" << endl;
	CPPUNIT_ASSERT(!optimizedList.empty());
	
	cout << "Make sure nothing crosses a boundary" << endl;
	for(std::list<OpenPath>::const_iterator iter = optimizedList.begin(); 
			iter != optimizedList.end(); 
			++iter) {
		const OpenPath& currentPath = *iter;
		for(OpenPath::const_iterator iter = currentPath.fromStart(); 
				iter != currentPath.end(); 
				++iter) {
			Segment2Type currentSegment = currentPath.segmentAfterPoint(iter);
			//make sure it doesn't cross a boundary
			for(Loop::finite_cw_iterator loopIter = loop.clockwiseFinite(); 
					loopIter != loop.clockwiseEnd(); 
					++loopIter) {
				Segment2Type boundarySegment = loop.segmentAfterPoint(loopIter);
				CPPUNIT_ASSERT(!currentSegment.intersects(boundarySegment));
			}
		}
	}
}

void PatherOptimizerTestCase::testCompleteness() {
	//make a square +-1 both axes
	Loop loop;
	loop.insertPointBefore(Point2Type(-1,1), loop.clockwiseEnd());
	loop.insertPointBefore(Point2Type(1,1), loop.clockwiseEnd());
	loop.insertPointBefore(Point2Type(1,-1), loop.clockwiseEnd());
	loop.insertPointBefore(Point2Type(-1,-1), loop.clockwiseEnd());
	
	pather_optimizer optimizer;
	
	//make paths that cross boundary and don't
	std::list<Point2Type> points;	//keeps track of points I expect to get out
	
	OpenPath insideBoundary;
	insideBoundary.appendPoint(Point2Type(-0.5, 0));
	points.push_back(Point2Type(-0.5, 0));
	insideBoundary.appendPoint(Point2Type(0.5, 0));
	points.push_back(Point2Type(0.5, 0));
	OpenPath outsideBoundary;
	outsideBoundary.appendPoint(Point2Type(-0.5, 2));
	points.push_back(Point2Type(-0.5, 2));
	outsideBoundary.appendPoint(Point2Type(0.5, 2));
	points.push_back(Point2Type(0.5, 2));
	
	cout << "Adding simple square boundary to optimizer" << endl;
	CPPUNIT_ASSERT_NO_THROW(optimizer.addBoundary(loop));
	cout << "Adding various OpenPaths to optimizer" << endl;
	CPPUNIT_ASSERT_NO_THROW(optimizer.addPath(insideBoundary));
	CPPUNIT_ASSERT_NO_THROW(optimizer.addPath(outsideBoundary));
	
	//get back out a list of OpenPaths
	std::list<OpenPath> optimizedList;
	cout << "Optimizing paths" << endl;
	optimizer.optimize(optimizedList);
	
	cout << "Make sure we get something not empty out" << endl;
	CPPUNIT_ASSERT(!optimizedList.empty());
	
	cout << "Make sure we get everything out" << endl;
	for(std::list<OpenPath>::const_iterator iter = optimizedList.begin(); 
			iter != optimizedList.end(); 
			++iter) {
		const OpenPath& currentPath = *iter;
		for(OpenPath::const_iterator pointIter = currentPath.fromStart(); 
				pointIter != currentPath.end(); 
				++pointIter) {
			points.remove(*pointIter);
		}
	}
	CPPUNIT_ASSERT_MESSAGE("Not all points were traversed!", points.empty());
}



