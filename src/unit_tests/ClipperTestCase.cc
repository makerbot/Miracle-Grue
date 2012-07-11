#include <list>
#include <limits>

#include <algorithm>

#include <cppunit/config/SourcePrefix.h>
#include "ClipperTestCase.h"
#include "UnitTestUtils.h"
#include "mgl/abstractable.h"
#include "mgl/clipper.h"
#include "mgl/ScadDebugFile.h"

CPPUNIT_TEST_SUITE_REGISTRATION( ClipperTestCase );

using namespace std;
using namespace mgl;
using namespace libthing;

string outputDir("outputs/test_cases/ClipperTestCase/");




void clipperToMgl(const ClipperLib::Polygons &polys, SegmentVector& outlinesSegments);
void mglToClipper(const SegmentVector &segmentTable, ClipperLib::Polygons &out_polys );
void dumpSegmentTable(const char* name, const SegmentTable & outTable);
void dumpClipperPolys(const char*name, const ClipperLib::Polygons  &polys);

class ClipperInsetter
{

public:
	void inset( const SegmentVector & inputPolys,
				Scalar insetDist,
				SegmentVector & outputPolys);
};

void ClipperTestCase::setUp()
{
	MyComputer computer;
	char pathsep = computer.fileSystem.getPathSeparatorCharacter();
	outputDir = string("outputs") + pathsep + string("test_cases") + 
		pathsep + string("ClipperTestCase") + pathsep;
	computer.fileSystem.guarenteeDirectoryExistsRecursive(outputDir.c_str());
	mkDebugPath(outputDir.c_str());
}

void ClipperTestCase::test_conversion()
{
	cout << endl;
	SegmentVector table;
	table.push_back(vector<LineSegment2>());
	vector<LineSegment2> &segs = *table.rbegin();

	segs.push_back(LineSegment2(Vector2(-25.867, 20.72951), Vector2(-24.93132, 20.72951)));
	segs.push_back(LineSegment2(Vector2(-24.93132, 20.72951), Vector2(25.867, 20.72951)));
	segs.push_back(LineSegment2(Vector2(25.867, 20.72951), Vector2(25.867, -22.01515)));
	segs.push_back(LineSegment2(Vector2(25.867, -22.01515), Vector2(25.867, -22.80249)));
	segs.push_back(LineSegment2(Vector2(25.867, -22.80249), Vector2(24.93132, -22.80249)));
	segs.push_back(LineSegment2(Vector2(24.93132, -22.80249), Vector2(-25.867, -22.80249)));
	segs.push_back(LineSegment2(Vector2(-25.867, -22.80249), Vector2(-25.867, 19.94217)));
	segs.push_back(LineSegment2(Vector2(-25.867, 19.94217), Vector2(-25.867, 20.72951)));
	/*
	segs.push_back(LineSegment2(Vector2(1,0), Vector2(0,1)) );
	segs.push_back(LineSegment2(Vector2(0,1), Vector2(1,1)) );
	segs.push_back(LineSegment2(Vector2(1,1), Vector2(1,0)) );
	*/
	dumpSegmentTable("table", table);

	ClipperLib::Polygons cPolys;
	mglToClipper(table, cPolys);
	dumpClipperPolys("cpoly", cPolys);

	SegmentTable outTable;
	clipperToMgl(cPolys, outTable);
	dumpSegmentTable("outTable", outTable);

	CPPUNIT_ASSERT(table.size() == outTable.size());
	CPPUNIT_ASSERT(table[0].size() == outTable[0].size());

	// first point x
	cout << "table[0][0].a[0] = " << table[0][0].a[0] << endl;
	cout << "table[0][0].a[1] = " << table[0][0].a[1] << endl;
	cout << "outTable[0][0].a[0] = " << outTable[0][0].a[0] << endl;
	cout << "outTable[0][0].a[1] = " << outTable[0][0].a[1] << endl;

	double tol = 0.001;
	CPPUNIT_ASSERT_DOUBLES_EQUAL( table[0][0].a[0], outTable[0][0].a[0], tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( table[0][0].a[1], outTable[0][0].a[1], tol );
	// first point y
	// CPPUNIT_ASSERT(table[0][0].a[1] == outTable[0][0].a[1]);

}



void ClipperTestCase::testSimpleClipper()
{

	cout << endl;
	ClipperLib::Polygons in_polys;

	vector<ClipperLib::IntPoint> points;

	points.push_back(ClipperLib::IntPoint(-25867,  20729) );
	points.push_back(ClipperLib::IntPoint(-22404,  20729) );
	points.push_back(ClipperLib::IntPoint( 25867,  20729) );
	points.push_back(ClipperLib::IntPoint( 25867, -19889) );
	points.push_back(ClipperLib::IntPoint( 25867, -22802) );
	points.push_back(ClipperLib::IntPoint( 22404, -22802) );
	points.push_back(ClipperLib::IntPoint(-25867, -22802) );
	points.push_back(ClipperLib::IntPoint(-25867,  17816) );
	points.push_back(ClipperLib::IntPoint(-25867,  20729) );


	if(!Orientation(points))
	{
	      cout << "Wrong orientation" << endl;
	      ReversePoints(points);
	}

	in_polys.push_back(points);

	ClipperLib::Polygons out_polys;
	ClipperLib::JoinType jointype = ClipperLib::jtMiter;
	double miterLimit = 3.0;
	OffsetPolygons(in_polys, out_polys, 1000, jointype, miterLimit);
	cout << "testSimpleClipper count = " << out_polys.size() << endl;

	dumpClipperPolys("out_polys",out_polys);


}


void ClipperTestCase::testSimpleInset()
{
	cout << endl;
	SegmentTable table;

	table.push_back(vector<LineSegment2>());
	vector<LineSegment2> &segs = *table.rbegin();

	segs.push_back(LineSegment2(Vector2(-25.867, 20.72951), Vector2(-24.93132, 20.72951)));
	segs.push_back(LineSegment2(Vector2(-24.93132, 20.72951), Vector2(25.867, 20.72951)));
	segs.push_back(LineSegment2(Vector2(25.867, 20.72951), Vector2(25.867, -22.01515)));
	segs.push_back(LineSegment2(Vector2(25.867, -22.01515), Vector2(25.867, -22.80249)));
	segs.push_back(LineSegment2(Vector2(25.867, -22.80249), Vector2(24.93132, -22.80249)));
	segs.push_back(LineSegment2(Vector2(24.93132, -22.80249), Vector2(-25.867, -22.80249)));
	segs.push_back(LineSegment2(Vector2(-25.867, -22.80249), Vector2(-25.867, 19.94217)));
	segs.push_back(LineSegment2(Vector2(-25.867, 19.94217), Vector2(-25.867, 20.72951)));

	// std::reverse(segs.begin(), segs.end());

	ScadDebugFile::segment3(cout, "", "in_segments", segs, 0, 0);

	SegmentTable outTable;

	ClipperInsetter insetter;
	insetter.inset(table, -1, outTable);
	cout << "outTable_size = " << outTable.size() << ";" << endl;
    dumpSegmentTable("outTable", outTable);

/*
loop_segments3(in_segments);
color([1,0,0,0])loop_segments3(out_segments_0);
color([0,1,1,0])loop_segments3(out_segments_1);
color([0,1,0,0])loop_segments3(out_segments_2);
color([0,0,1,0])loop_segments3(out_segments_3);

 */


//	vector<LineSegment2> &outSegs = *outTable.rbegin();
//	cout << "outSegs size = " << outSegs.size();
//	ScadTubeFile::segment3(cout, "", "out_segments", outSegs, 0, 0);

}
