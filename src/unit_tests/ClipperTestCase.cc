#include <list>
#include <limits>

#include <cppunit/config/SourcePrefix.h>
#include "ClipperTestCase.h"

#include "insetTests.h"


#include "clipper/clipper.h"



CPPUNIT_TEST_SUITE_REGISTRATION( ClipperTestCase );

using namespace std;
using namespace mgl;


string outputDir("outputs/test_cases/ClipperTestCase/");

#define DBLTOINT 1000
bool useClipper = true;




void clipperToMgl(const ClipperLib::Polygons &polys, mgl::SegmentTable & outlinesSegments)
{
	size_t loopCount = polys.size();
	for(size_t i=0; i < loopCount; i++)
	{
		const ClipperLib::Polygon &loop = polys[i];
		outlinesSegments.push_back(std::vector<mgl::LineSegment2 > ());
		std::vector<mgl::LineSegment2 > &segments = *outlinesSegments.rbegin();
		for(size_t j=0; j < loop.size(); j++)
		{
			size_t next = j==loop.size()-1?0:j+1;
			const ClipperLib::IntPoint &point = loop[j];
			const ClipperLib::IntPoint &nextPoint = loop[next];

			mgl::LineSegment2 s;
			s.a[0] = point.X / (Scalar)DBLTOINT;
			s.a[1] = point.Y / (Scalar)DBLTOINT;
			s.b[0] = nextPoint.X / (Scalar)DBLTOINT;
			s.b[1] = nextPoint.Y / (Scalar)DBLTOINT;
			segments.push_back(s);
		}
	}
}

void mglToClipper(const mgl::SegmentTable &segmentTable, ClipperLib::Polygons &out_polys )
{
	for(size_t i=0; i < segmentTable.size(); i++)
	{
		out_polys.push_back(vector<ClipperLib::IntPoint>());
		vector<ClipperLib::IntPoint>& poly = *out_polys.rbegin();

		const vector<mgl::LineSegment2> &loop = segmentTable[i];
		for(size_t j=0; j < loop.size(); j++)
		{
			const mgl::LineSegment2 &seg = loop[j];
			ClipperLib::IntPoint p;
			p.X = seg.a[0] * DBLTOINT;
			p.Y = seg.a[1] * DBLTOINT;
			poly.push_back(p);
		}
	}
}

void dumpSegmentTable(const char* name, const SegmentTable & outTable)
{
    for(int i = 0;i < outTable.size();i++){
        const vector<LineSegment2> & segs = outTable[i];
        stringstream ss;
        ss << name << "_" << i;
        ScadTubeFile::segment3(cout, "", ss.str().c_str(), segs, 0, 0);
    }
}

void dumpClipperPolys(const char*name, const ClipperLib::Polygons  &polys)
{
	for(size_t i=0; i < polys.size(); i++)
	{
		const ClipperLib::Polygon &poly = polys[i];
		cout <<  name <<"_" << i << "= [";
		for(size_t j=0; j < poly.size(); j++)
		{
			const ClipperLib::IntPoint &p = poly[j];
			cout << "[" << p.X << ", "<< p.Y << "]," << endl;
		}
		cout<< "];" << endl;
	}
}

class ClipperInsetter
{

public:
	void inset( const mgl::SegmentTable & inputPolys,
				Scalar insetDist,
				mgl::SegmentTable & outputPolys)
	{

		ClipperLib::Polygons in_polys, out_polys;
		ClipperLib::JoinType jointype = ClipperLib::jtMiter;
		double miterLimit = 3.0;

		double delta = -insetDist * DBLTOINT;

		mglToClipper  (inputPolys, in_polys);
		//dumpClipperPolys(in_polys);
		OffsetPolygons(in_polys, out_polys, delta, jointype, miterLimit);
		//dumpClipperPolys(out_polys);
		clipperToMgl(out_polys, outputPolys);
	}


};


void ClipperTestCase::setUp()
{
	MyComputer computer;
	computer.fileSystem.mkpath(outputDir.c_str());
}

void ClipperTestCase::test_conversion()
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
//	CPPUNIT_ASSERT(table[0][0].a[1] == outTable[0][0].a[1]);

}



void ClipperTestCase::testSimpleClipper()
{

	cout << endl;
	ClipperLib::Polygons in_polys;

	vector<ClipperLib::IntPoint> points;
	/*
	points.push_back(ClipperLib::IntPoint(-25867,  20729) );
	points.push_back(ClipperLib::IntPoint(-22404,  20729) );
	points.push_back(ClipperLib::IntPoint( 25867,  20729) );
	points.push_back(ClipperLib::IntPoint( 25867, -19889) );
	points.push_back(ClipperLib::IntPoint( 25867, -22802) );
	points.push_back(ClipperLib::IntPoint( 22404, -22802) );
	points.push_back(ClipperLib::IntPoint(-25867, -22802) );
	points.push_back(ClipperLib::IntPoint(-25867,  17816) );
	points.push_back(ClipperLib::IntPoint(-25867,  20729) );
	*/
	points.push_back(ClipperLib::IntPoint(-25867,  20729) );
	points.push_back(ClipperLib::IntPoint(-25867,  17816) );
	points.push_back(ClipperLib::IntPoint(-25867, -22802) );
	points.push_back(ClipperLib::IntPoint( 22404, -22802) );
	points.push_back(ClipperLib::IntPoint( 25867, -22802) );
	points.push_back(ClipperLib::IntPoint( 25867, -19889) );
	points.push_back(ClipperLib::IntPoint( 25867,  20729) );
	points.push_back(ClipperLib::IntPoint(-22404,  20729) );
	points.push_back(ClipperLib::IntPoint(-25867,  20729) );

	in_polys.push_back(points);

	ClipperLib::Polygons out_polys;
	ClipperLib::JoinType jointype = ClipperLib::jtMiter;
	double miterLimit = 3.0;
	OffsetPolygons(in_polys, out_polys, 1000, jointype, miterLimit);
	cout << "testSimpleClipper count = " << out_polys.size() << endl;

	dumpClipperPolys("out_polys",out_polys);

/*

 Program output:

 out_polys_0= [[24867, 20729],
[24867, 19729],
[25867, 19729],
[25867, 20729],
];
out_polys_1= [[-25867, 20729],
[-25867, 19729],
[-24867, 19729],
[-24867, 20729],
];
out_polys_2= [[-25867, -21802],
[-25867, -22802],
[-24867, -22802],
[-24867, -21802],
];
out_polys_3= [[24867, -21802],
[24867, -22802],
[25867, -22802],
[25867, -21802],
];

 */
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


	ScadTubeFile::segment3(cout, "", "in_segments", segs, 0, 0);

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
