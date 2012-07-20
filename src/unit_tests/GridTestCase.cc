
#include "UnitTestUtils.h"
#include "GridTestCase.h"
#include "mgl/grid.h"

using namespace std;
using namespace mgl;

CPPUNIT_TEST_SUITE_REGISTRATION( GridTestCase );


void GridTestCase::setUp() {
	cout << "no setup" << endl;
}


void GridTestCase::testGridRangesToOpenPaths() {
	Grid grid;

	ScalarRangeTable rays;
	rays.resize(2);

	ScalarRangeTable::iterator ray = rays.begin();

	ray->push_back(ScalarRange(0, 1));
	ray->push_back(ScalarRange(2, 3));

	++ray;

	ray->push_back(ScalarRange(0, 1));
	ray->push_back(ScalarRange(2, 3));

	vector<Scalar> values;
	values.push_back(0);
	values.push_back(1);

	OpenPathList paths;

	grid.gridRangesToOpenPaths(rays, values, X_AXIS, paths);

	cout << "blah" << endl;
	OpenPathList::iterator path = paths.begin();
	CPPUNIT_ASSERT(paths.size() == 4);
	CPPUNIT_ASSERT_EQUAL(path->fromStart()->x, 0.0);
	CPPUNIT_ASSERT_EQUAL(path->fromStart()->y, 0.0);

	CPPUNIT_ASSERT_EQUAL(path->fromEnd()->x, 1.0);
	CPPUNIT_ASSERT_EQUAL(path->fromEnd()->y, 0.0);

	++path;
	
	CPPUNIT_ASSERT_EQUAL(path->fromStart()->x, 2.0);
	CPPUNIT_ASSERT_EQUAL(path->fromStart()->y, 0.0);

	CPPUNIT_ASSERT_EQUAL(path->fromEnd()->x, 3.0);
	CPPUNIT_ASSERT_EQUAL(path->fromEnd()->y, 0.0);

	++path;
	
	CPPUNIT_ASSERT_EQUAL(path->fromStart()->x, 0.0);
	CPPUNIT_ASSERT_EQUAL(path->fromStart()->y, 1.0);

	CPPUNIT_ASSERT_EQUAL(path->fromEnd()->x, 1.0);
	CPPUNIT_ASSERT_EQUAL(path->fromEnd()->y, 1.0);
	
	++path;
	
	CPPUNIT_ASSERT_EQUAL(path->fromStart()->x, 2.0);
	CPPUNIT_ASSERT_EQUAL(path->fromStart()->y, 1.0);

	CPPUNIT_ASSERT_EQUAL(path->fromEnd()->x, 3.0);
	CPPUNIT_ASSERT_EQUAL(path->fromEnd()->y, 1.0);
	
}
	
	

	
