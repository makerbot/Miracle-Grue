#include <cppunit/config/SourcePrefix.h>
#include "UnitTestUtils.h"

#include "RoofingTestCase.h"

#include "UnitTestUtils.h"
#include "mgl/miracle.h"
#include "mgl/grid.h"

//
#include "mgl/configuration.h"
#include "mgl/insets.h"


CPPUNIT_TEST_SUITE_REGISTRATION( RoofingTestCase );

using namespace std;
using namespace mgl;
using namespace libthing;

MyComputer computer;

string outputDir ("outputs/test_cases/roofingTestCase/");

string inputDir("./test_cases/roofingTestCase/stls/");

void rangeTableDifference(	const ScalarRangeTable &src,
							const ScalarRangeTable &del,
							ScalarRangeTable &diff);

void rangeDifference(const vector< ScalarRange > &srcLine,
		 	 	 	  const vector< ScalarRange > &delLine,
		 	 	 	  vector< ScalarRange > &diffLine );

bool scalarRangeDifference(const ScalarRange& diffRange,
							ScalarRange& srcRange,
							ScalarRange &resultRange);

vector< ScalarRange >::const_iterator  subRangeDifference(	const ScalarRange &initialRange,
		 	 	 	 	vector< ScalarRange >::const_iterator it,
		 	 	 	 	vector< ScalarRange >::const_iterator itEnd,
						vector< ScalarRange > &result );

void rangeUnion( const vector< ScalarRange > &firstLine,
		 	 	 	  const vector< ScalarRange > &secondLine,
		 	 	 	  vector< ScalarRange > &unionLine );

vector< ScalarRange >::const_iterator  subRangeUnion(const ScalarRange &initialRange,
		 	 	 	 	vector< ScalarRange >::const_iterator it,
		 	 	 	 	vector< ScalarRange >::const_iterator itEnd,
						vector< ScalarRange > &result );

bool scalarRangeUnion(const ScalarRange& range0, const ScalarRange& range1, ScalarRange &resultRange);

bool intersectRange(Scalar a, Scalar b, Scalar c, Scalar d, Scalar &begin, Scalar &end);

void rangeTersection(const vector< ScalarRange > &oneLine,
					 const vector< ScalarRange > &twoLine,
						vector< ScalarRange > &boolLine );


//bool scalarRangeIntersection(const ScalarRange &a,
//								const ScalarRange &b,
//								ScalarRange &inter)
//{
//	return intersectRange(a.min, a.max, b.min, b.max, inter.min, inter.max);
//}


void dumpRanges(const vector<ScalarRange> &ranges)
{
	for(size_t i=0; i < ranges.size(); i++)
	{
		const ScalarRange &range = ranges[i];
		cout << range << endl;
	}
}

void dumpRangeTable(const char *name, const ScalarRangeTable &rangeTable)
{
	cout << name << endl;
	for(size_t i=0; i<rangeTable.size(); i++)
	{
		cout << " " << i << endl;
		const vector<ScalarRange> &ranges = rangeTable[i];
		dumpRanges(ranges);
	}
}

void dumpGridRanges(const GridRanges &ranges)
{
	dumpRangeTable("xRays", ranges.xRays);
	dumpRangeTable("yRays", ranges.yRays);

}

void lineSegmentsFromScalarRanges(const std::vector<ScalarRange> &ranges,
									Scalar y,
									std::vector<LineSegment2> &gridSegments)
{
	assert(gridSegments.size()==0);
	gridSegments.reserve(ranges.size());
	for(size_t i=0; i < ranges.size(); i++)
	{
		const ScalarRange &range = ranges[i];
		gridSegments.push_back(LineSegment2(Vector2(range.min,y), Vector2(range.max,y)));
	}
}




void segmentTableFromRangeTable(const ScalarRangeTable &rangeTable,
								const vector<Scalar> &yValues,
								SegmentTable &rayTable)
{
	assert(rayTable.size() == 0);
	rayTable.resize(yValues.size());
	for(size_t i=0; i < yValues.size(); i++)
	{
		Scalar y = yValues[i];
		const std::vector<ScalarRange> &ranges = rangeTable[i];
		vector<LineSegment2> &segments = rayTable[i];
		lineSegmentsFromScalarRanges(ranges, y, segments);
	}
}


void addValues(Scalar min, Scalar max, Scalar delta, std::vector<Scalar>& values)
{
	Scalar value = min;
	while(value <= max)
	{
		values.push_back(value);
		value += delta;
	}
}


void rayCastAlongX(	const SegmentTable &outlineLoops,
									Scalar y,
									Scalar xMin,
									Scalar xMax,
									std::vector<ScalarRange> &ranges);

void castRaysOnSliceAlongX(const SegmentTable &outlineLoops,
					const std::vector<Scalar> &yValues,
					Scalar xMin,
					Scalar xMax,
					ScalarRangeTable &rangeTable);

double tol = 1e-6;

void RoofingTestCase::setUp()
{
	MyComputer computer;
	mkDebugPath(outputDir.c_str());
}

void RoofingTestCase::testSimple()
{
	cout<<endl;

	SegmentTable outlineLoops;
	outlineLoops.push_back(vector<LineSegment2>());

	LineSegment2 s= LineSegment2(Vector2(0,0), Vector2(0,1));
	outlineLoops[0].push_back(s);
	outlineLoops[0].push_back(LineSegment2(Vector2(1,0), Vector2(1,1)) );

	vector<ScalarRange> ranges;
	Scalar y= 0.5;
	Scalar xMin = -1;
	Scalar xMax = 2;
	rayCastAlongX(outlineLoops, y, xMin, xMax, ranges);

	double tol = 1e-6;

	CPPUNIT_ASSERT_EQUAL((size_t)1, ranges.size());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, ranges[0].min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, ranges[0].max, tol);

//	cout << "HO!" << endl;
}


void addInnerHexLoop(std::vector<LineSegment2>& segs, Scalar dx=0, Scalar dy=0)
{
	segs.push_back(LineSegment2(Vector2( dx + 2.16506, -1.25 + dy ), Vector2( dx + 2.1669, -1.24605 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.1669, -1.24605 + dy ), Vector2( dx + 2.34923, -0.85505 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.34923, -0.85505 + dy ), Vector2( dx + 2.35036, -0.85084 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.35036, -0.85084 + dy ), Vector2( dx + 2.46202, -0.43412 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.46202, -0.43412 + dy ), Vector2( dx + 2.4624, -0.42978 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.4624, -0.42978 + dy ), Vector2( dx + 2.5, 0.0 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.5, 0.0 + dy ), Vector2( dx + 2.49962, 0.00434 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.49962, 0.00434 + dy ), Vector2( dx + 2.46202, 0.43412 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.46202, 0.43412 + dy ), Vector2( dx + 2.46089, 0.43833 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.46089, 0.43833 + dy ), Vector2( dx + 2.34923, 0.85505 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.34923, 0.85505 + dy ), Vector2( dx + 2.34739, 0.859 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.34739, 0.859 + dy ), Vector2( dx + 2.16506, 1.25 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.16506, 1.25 + dy ), Vector2( dx + 2.16256, 1.25357 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 2.16256, 1.25357 + dy ), Vector2( dx + 1.91511, 1.60697 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.91511, 1.60697 + dy ), Vector2( dx + 1.91203, 1.61005 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.91203, 1.61005 + dy ), Vector2( dx + 1.60697, 1.91511 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.60697, 1.91511 + dy ), Vector2( dx + 1.6034, 1.91761 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.6034, 1.91761 + dy ), Vector2( dx + 1.25, 2.16506 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.25, 2.16506 + dy ), Vector2( dx + 1.24605, 2.16691 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.24605, 2.16691 + dy ), Vector2( dx + 0.85505, 2.34923 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.85505, 2.34923 + dy ), Vector2( dx + 0.85084, 2.35036 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.85084, 2.35036 + dy ), Vector2( dx + 0.43412, 2.46202 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.43412, 2.46202 + dy ), Vector2( dx + 0.42978, 2.4624 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.42978, 2.4624 + dy ), Vector2( dx + 0.0, 2.5 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.0, 2.5 + dy ), Vector2( dx + -0.00434, 2.49962 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -0.00434, 2.49962 + dy ), Vector2( dx + -0.43412, 2.46202 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -0.43412, 2.46202 + dy ), Vector2( dx + -0.43833, 2.46089 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -0.43833, 2.46089 + dy ), Vector2( dx + -0.85505, 2.34923 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -0.85505, 2.34923 + dy ), Vector2( dx + -0.859, 2.34739 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -0.859, 2.34739 + dy ), Vector2( dx + -1.25, 2.16506 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.25, 2.16506 + dy ), Vector2( dx + -1.25357, 2.16256 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.25357, 2.16256 + dy ), Vector2( dx + -1.60697, 1.91511 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.60697, 1.91511 + dy ), Vector2( dx + -1.61005, 1.91203 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.61005, 1.91203 + dy ), Vector2( dx + -1.91511, 1.60697 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.91511, 1.60697 + dy ), Vector2( dx + -1.91761, 1.6034 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.91761, 1.6034 + dy ), Vector2( dx + -2.16506, 1.25 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.16506, 1.25 + dy ), Vector2( dx + -2.1669, 1.24605 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.1669, 1.24605 + dy ), Vector2( dx + -2.34923, 0.85505 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.34923, 0.85505 + dy ), Vector2( dx + -2.35036, 0.85084 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.35036, 0.85084 + dy ), Vector2( dx + -2.46202, 0.43412 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.46202, 0.43412 + dy ), Vector2( dx + -2.4624, 0.42978 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.4624, 0.42978 + dy ), Vector2( dx + -2.5, 0.0 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.5, 0.0 + dy ), Vector2( dx + -2.49962, -0.00434 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.49962, -0.00434 + dy ), Vector2( dx + -2.46202, -0.43412 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.46202, -0.43412 + dy ), Vector2( dx + -2.46089, -0.43833 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.46089, -0.43833 + dy ), Vector2( dx + -2.34923, -0.85505 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.34923, -0.85505 + dy ), Vector2( dx + -2.34739, -0.859 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.34739, -0.859 + dy ), Vector2( dx + -2.16506, -1.25 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.16506, -1.25 + dy ), Vector2( dx + -2.16256, -1.25357 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -2.16256, -1.25357 + dy ), Vector2( dx + -1.91511, -1.60697 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.91511, -1.60697 + dy ), Vector2( dx + -1.91203, -1.61005 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.91203, -1.61005 + dy ), Vector2( dx + -1.60697, -1.91511 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.60697, -1.91511 + dy ), Vector2( dx + -1.6034, -1.91761 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.6034, -1.91761 + dy ), Vector2( dx + -1.25, -2.16506 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.25, -2.16506 + dy ), Vector2( dx + -1.24605, -2.1669 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -1.24605, -2.1669 + dy ), Vector2( dx + -0.85505, -2.34923 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -0.85505, -2.34923 + dy ), Vector2( dx + -0.85084, -2.35036 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -0.85084, -2.35036 + dy ), Vector2( dx + -0.43412, -2.46202 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -0.43412, -2.46202 + dy ), Vector2( dx + -0.42978, -2.4624 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + -0.42978, -2.4624 + dy ), Vector2( dx + 0.0, -2.5 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.0, -2.5 + dy ), Vector2( dx + 0.00434, -2.49962 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.00434, -2.49962 + dy ), Vector2( dx + 0.43412, -2.46202 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.43412, -2.46202 + dy ), Vector2( dx + 0.43833, -2.46089 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.43833, -2.46089 + dy ), Vector2( dx + 0.85505, -2.34923 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.85505, -2.34923 + dy ), Vector2( dx + 0.859, -2.34739 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 0.859, -2.34739 + dy ), Vector2( dx + 1.25, -2.16506 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.25, -2.16506 + dy ), Vector2( dx + 1.25357, -2.16256 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.25357, -2.16256 + dy ), Vector2( dx + 1.60697, -1.91511 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.60697, -1.91511 + dy ), Vector2( dx + 1.61005, -1.91203 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.61005, -1.91203 + dy ), Vector2( dx + 1.91511, -1.60697 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.91511, -1.60697 + dy ), Vector2( dx + 1.91761, -1.6034 + dy)));
	segs.push_back(LineSegment2(Vector2( dx + 1.91761, -1.6034 + dy ), Vector2( dx + 2.16506, -1.25 + dy)));

}

void addOuterHexLoop(std::vector<LineSegment2>& segs, Scalar dx=0, Scalar dy=0)
{
	segs.push_back(LineSegment2(Vector2( dx+ 10.0,   -5.773501+dy ), Vector2( dx +   9.89, -5.83701+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ 9.89,   -5.83701+dy ),  Vector2( dx +   0.0, -11.54701+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ 0.0,   -11.54701+dy ),  Vector2( dx +  -0.11, -11.483502+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ -0.11, -11.483502+dy ), Vector2( dx + -10.0, -5.773501+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ -10.0,  -5.773501+dy ), Vector2( dx + -10.0, -5.646484+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ -10.0,  -5.646484+dy ), Vector2( dx + -10.0, 5.773504+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ -10.0,   5.773504+dy ), Vector2( dx +  -9.89, 5.837012+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ -9.89,	  5.837012+dy ), Vector2( dx +   0.0, 11.54701+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ 0.0, 	 11.54701+dy ),  Vector2( dx +   0.11, 11.483502+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ 0.11, 	 11.483502+dy ), Vector2( dx +  10.0, 5.773504+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ 10.0, 	  5.773504+dy ), Vector2( dx +  10.0, 5.646487+ dy )));
	segs.push_back(LineSegment2(Vector2( dx+ 10.0, 	  5.646487+dy ), Vector2( dx +  10.0, -5.773501+ dy )));
}


void addLinea(ScadDebugFile &fscad)
{
    std::ostream & out = fscad.getOut();

    double layerH = 0.3;
    double layerW = 0.4;

    out << "" << endl;
    out << "module linea(segments, ball=true)" << endl;
    out << "{" << endl;
    out << "	if(ball) corner (x=segments[0][0][0],  y=segments[0][0][1], z=segments[0][0][2], diameter=0.25, faces=12, thickness_over_width=1);" << endl;
    out << "    for(seg = segments)" << endl;
    out << "    {" << endl;
    out << "        tube(x1=seg[0][0], y1=seg[0][1], z1=seg[0][2], x2=seg[1][0], y2=seg[1][1], z2=seg[1][2] , diameter1=0.1, diameter2=0.05, faces=4, thickness_over_width=1);" << endl;
    out << "    }" << endl;
    out << "}" << endl;

	out  << std::endl;
	out << "module extrusion(x1, y1, z1, x2, y2, z2)" << std::endl;
	out << "{" << std::endl;
	out << "    d = " << layerH << ";" << std::endl;
	out << "    f = 6;" << std::endl;
	out << "    t =  "  << layerH / layerW << ";"<< std::endl;
	out << "    corner(x1,y1,z1, diameter=d, faces=f, thickness_over_width =t );" << std::endl;
	out << "    tube(x1, y1, z1, x2, y2, z2, diameter1=d, diameter2=d, faces=f, thickness_over_width=t);" << std::endl;
	out << "}" << std::endl;

	out << std::endl;
	out << "module polys(points, paths)" << std::endl;
	out << "{" << std::endl;
	out << "     for (p= paths)" << std::endl;
	out << "    {" << std::endl;
	out << "        extrusion(points[p[0]][0],points[p[0]][1],points[p[0]][2], points[p[1]][0],points[p[1]][1],points[p[1]][2] );" << std::endl;
	out << "    }" << std::endl;
	out << "}" << std::endl;
	out << std::endl;

}

Scalar writeScanLines(ScadDebugFile& fscad,
					const char*name,
					const char* implementation,
					Scalar z,
					Scalar dz,
					const SegmentTable &rayTable)
{
	Scalar h = z;
	for(size_t i=0; i < rayTable.size(); i++)
	{
		const std::vector<LineSegment2>& line = rayTable[i];
		h = fscad.writeSegments3(name, implementation, line, h, dz, i);
	}

	string minMaxName = name;
	minMaxName += "all";
	fscad.writeMinMax(minMaxName.c_str(), name, rayTable.size());
	return h;
}

void RoofingTestCase::testHoly() // test with a hole in the slice
{
	cout << endl;

	SegmentTable loops;
	loops.push_back(std::vector<LineSegment2>());
	loops.push_back(std::vector<LineSegment2>());
	addOuterHexLoop(loops[0]);
	addInnerHexLoop(loops[1]);

	Scalar xMin = -12;
	Scalar xMax =  12;
	vector<Scalar> yValues;
	Scalar dy = 0.1;
	addValues(xMin, xMax, dy, yValues);

	SegmentTable rayTable;
	ScalarRangeTable rangeTable;
	castRaysOnSliceAlongX(loops, yValues, xMin, xMax, rangeTable);
	segmentTableFromRangeTable(rangeTable, yValues, rayTable);

	bool printMe = false;
	if(printMe)
	{
		for(size_t i=0; i < rayTable.size(); i++)
		{
			std::vector<LineSegment2>& line = rayTable[i];
			// cout << i << ": " << yValues[i] << ") " << line.size() << endl << "\t";
			for(size_t j=0; j < line.size(); j++)
			{
				cout <<  line[j] << "\t";
			}
			cout << endl;
		}
	}

	string filename = outputDir + "hexagon_lines.scad";
	ScadDebugFile fscad;
	fscad.open(filename.c_str());
	std::ostream & out = fscad.getOut();
	out << "draw_x_all();" << endl << endl;
	addLinea(fscad);
    fscad.writeHeader();

	Scalar z = 0;
	Scalar dz = 0;
    writeScanLines(fscad, "draw_x_", "linea", z, dz, rayTable );
	fscad.close();
}



void RoofingTestCase::testGrid()
{
    cout << endl;
	SegmentTable loops;
	loops.push_back(std::vector<LineSegment2>());

	SegmentTable innerLoops;
	loops.push_back(std::vector<LineSegment2>());

	Scalar xMin = -12;
	Scalar xMax =  12;
	vector<Scalar> yValues;
	Scalar dy = 0.1;

	Scalar yMin = -12;
	Scalar yMax =  12;
	vector<Scalar> xValues;
	Scalar dx = 0.1;

	addOuterHexLoop(loops[0]);
	addInnerHexLoop(loops[1]);
	addValues(xMin, xMax, dy, yValues);
	addValues(yMin, yMax, dx, xValues);

	Vector2 toRotationCenter(0,0);
	Vector2 backToOrigin( toRotationCenter * -1);

	Scalar angle = M_PI * 0.5;

	SegmentTable xRays, yRays;
	SegmentTable rotatedLoops;

    assert(xRays.size() == 0);
    assert(yRays.size() == 0);
    assert(rotatedLoops.size() == 0);

    rotatedLoops = loops;
    translateLoops(rotatedLoops, toRotationCenter);
    rotateLoops(rotatedLoops, angle);

    ScalarRangeTable xRangeTable;
    castRaysOnSliceAlongX(loops, yValues, xMin, xMax, xRangeTable);

    ScalarRangeTable yRangeTable;
    castRaysOnSliceAlongX(rotatedLoops, xValues, yMin, yMax, yRangeTable);

    segmentTableFromRangeTable(xRangeTable, yValues, xRays);
    segmentTableFromRangeTable(yRangeTable, xValues, yRays);
    rotateLoops(yRays, -angle);
    translateLoops(yRays, backToOrigin);

	string filename = outputDir + "hexagon_grid.scad";
	ScadDebugFile fscad;
    cout << "writing " << filename << endl;
	fscad.open(filename.c_str());
	addLinea(fscad);
    fscad.writeHeader();

	Scalar  z = 0;
	Scalar dz = 0;

	writeScanLines(fscad, "outlines_", "linea", -1, -0.1, loops);
    writeScanLines(fscad, "draw_x_", "linea", z, dz, xRays );
    writeScanLines(fscad, "draw_y_", "linea", 0.5, dz, yRays );

    std::ostream & out = fscad.getOut();

    out << "draw_x_all();" << endl;
    out << "draw_y_all();" << endl;
    out << "outlines_all();" << endl;
    fscad.close();
}

void checkSegment(const LineSegment2& s, Scalar x0, Scalar x1, Scalar y0, Scalar y1)
{
	Scalar tol= 1e-6;
	cout << "Check a" << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(x0, s.a[0], tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(y0, s.a[1], tol);
	cout << "Check b" << endl;
	CPPUNIT_ASSERT_DOUBLES_EQUAL(x1, s.b[0], tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(y1, s.b[1], tol);
}

void RoofingTestCase::testIntersectRange()
{
	cout << endl;

	Scalar tol = 1e-6;

	Scalar a,b;
	bool r;

	r = intersectRange(0,1,2,3, a,b); // disconnected (ab before cd)
	CPPUNIT_ASSERT (r == false);

	r = intersectRange(2,3,0,1, a,b); // disconnected (cd before ab)
	CPPUNIT_ASSERT (r == false);

	r = intersectRange(0, 1, 0.5, 2, a, b);

	CPPUNIT_ASSERT(r);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, a, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  1, b, tol);

	r = intersectRange(0, 1, 0, 1, a, b);
	CPPUNIT_ASSERT(r);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  0, a, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  1, b, tol);

	r = intersectRange(0, 1, -1, 0.5, a, b);
	CPPUNIT_ASSERT(r);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  0, a, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(  0.5, b, tol);

}








void RoofingTestCase::testBooleanIntersect()
{
	cout << endl;

	SegmentTable slice0;
	slice0.push_back(std::vector<LineSegment2>());

	SegmentTable slice1;
	slice1.push_back(std::vector<LineSegment2>());

	Scalar xMin = -3;
	Scalar xMax = 8;
	Scalar dy = 0.1;
	vector<Scalar> yValues;

	addValues(-3, 3, 0.1, yValues );
	addInnerHexLoop(slice0[0], 0, 0);
	addInnerHexLoop(slice1[0], 2, 0);

	addValues(xMin, xMax, dy, yValues);

	Vector2 rotationCenter(0,0);
	Scalar angle = M_PI * 0.5;

	ScalarRangeTable rangeTableBottom;
	castRaysOnSliceAlongX(slice0, yValues, xMin, xMax, rangeTableBottom);

	ScalarRangeTable rangeTableTop;
	castRaysOnSliceAlongX(slice1, yValues, xMin, xMax, rangeTableTop);

	ScalarRangeTable rangeTableDiff;
	rangeTableDifference(rangeTableBottom, rangeTableTop, rangeTableDiff);

	SegmentTable top;
	segmentTableFromRangeTable(rangeTableTop, yValues, top);
	SegmentTable bottom;
	segmentTableFromRangeTable(rangeTableBottom, yValues, bottom);

	SegmentTable difference;
	segmentTableFromRangeTable(rangeTableDiff, yValues, difference);

	string filename = outputDir + "intersect.scad";

	cout << filename << endl;

	ScadDebugFile fscad;
	fscad.open(filename.c_str());
	addLinea(fscad);
    fscad.writeHeader();

	Scalar z = 0;
	Scalar dz = 0;

	writeScanLines(fscad, "intersection_", "linea", 2, dz, difference );
    writeScanLines(fscad, "bottom_", "linea", 0, dz, bottom );
    writeScanLines(fscad, "top_", "linea", 1, dz, top );

    std::ostream & out = fscad.getOut();

    out << "intersection_all();" << endl;
    out << "top_all();" << endl;
    out << "bottom_all();" << endl;
    fscad.close();
}



void RoofingTestCase::testSimpleLineTersect()
{
	cout << endl;
	vector<ScalarRange> first;
	vector<ScalarRange> second;

	first.push_back(ScalarRange(0, 1));
	first.push_back(ScalarRange(2, 3));
	first.push_back(ScalarRange(4, 5));

	second.push_back(ScalarRange(2.5, 3.5));

	vector<ScalarRange> intersection;
	cout << "rangeTersection" << endl;
	rangeTersection(first, second, intersection);

//	cout << "INTERSECTIONS" << endl;
//	for(unsigned int i=0; i < intersection.size(); i++)
//	{
//		cout << " "<< i << ") " << intersection[i] << endl;
//	}

	CPPUNIT_ASSERT_EQUAL((size_t)1, intersection.size());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.5, intersection[0].min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3, intersection[0].max, tol);

	vector<ScalarRange> intersection2;
	rangeTersection(second, first, intersection2);

//	cout << "INTERSECTIONS 2" << endl;
//	for(unsigned int i=0; i < intersection.size(); i++)
//	{
//		cout << " "<< i << ") " << intersection[i] << endl;
//	}

	CPPUNIT_ASSERT_EQUAL((size_t)1, intersection2.size());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.5, intersection2[0].min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3, intersection2[0].max, tol);
}

void RoofingTestCase::testLineTersect2()
{
	//	1      ******      ********    ******   **   **
	//  2  **     *****   ****  *    ***   *******
	//  3         ---      ---  -      -   --   --
	//     012345678901234567890123456789012345678901234
	//               1         2         3         4
	//  7-10 16-19 21-22 28-29 32-34
	cout << endl;
	vector<ScalarRange> first;
	vector<ScalarRange> second;

	first.push_back( ScalarRange( 4, 10));
	first.push_back( ScalarRange(16, 24));
	first.push_back( ScalarRange(28, 34));
	first.push_back( ScalarRange(37, 39));
	first.push_back( ScalarRange(42, 44));

	second.push_back(ScalarRange( 0,  2));
	second.push_back(ScalarRange( 7, 12));
	second.push_back(ScalarRange(15, 19));
	second.push_back(ScalarRange(21, 22));
	second.push_back(ScalarRange(25, 29));
	second.push_back(ScalarRange(32, 39));

	vector<ScalarRange> intersection;
	cout << "rangeTersection" << endl;
	rangeTersection(first, second, intersection);

	cout << "INTERSECTIONS" << endl;
	for(unsigned int i=0; i < intersection.size(); i++)
	{
		cout << " "<< i << ") " << intersection[i] << endl;
	}
	CPPUNIT_ASSERT(intersection.size() == 6);
}



void RoofingTestCase::testSimpleDifference()
{
	ScalarRange src(1,5);
	ScalarRange diff(2,4);
	ScalarRange res;
	bool b;

	b = scalarRangeDifference(diff, src, res);

	CPPUNIT_ASSERT(b);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(1, res.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2, res.max, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4, src.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5, src.max, tol);

	src = ScalarRange(2,4);
	diff = ScalarRange(1,5);
	b = scalarRangeDifference(diff, src, res);
	CPPUNIT_ASSERT(!b);

	src = ScalarRange(1,5);
	diff = ScalarRange(3,6);
	b = scalarRangeDifference(diff, src, res);
	CPPUNIT_ASSERT(b);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1, res.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3, res.max, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5, src.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5, src.max, tol);

	src = ScalarRange(3,5);
	diff = ScalarRange(1,4);
	b = scalarRangeDifference(diff, src, res);
	CPPUNIT_ASSERT(!b);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4, src.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5, src.max, tol);

	src = ScalarRange(1,3);
	diff = ScalarRange(2,4);
	b = scalarRangeDifference(diff, src, res);
	CPPUNIT_ASSERT(b);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1, res.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2, res.max, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3, src.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3, src.max, tol);
}


void RoofingTestCase::testSubRangeDifference()
{
	ScalarRange initialRange(1,8);
	vector< ScalarRange > diffRanges;
	vector< ScalarRange > results;

	diffRanges.push_back( ScalarRange(2,3) );
	diffRanges.push_back( ScalarRange(4,5) );
	diffRanges.push_back( ScalarRange(6,7) );
	subRangeDifference(initialRange, diffRanges.begin(), diffRanges.end(), results );
	CPPUNIT_ASSERT_EQUAL((size_t)4, results.size());
}


void RoofingTestCase::testRangeDifference()
{
	//	1      ******      ********    ******   **   **
	//  2  **     *****   ****  *    ***   *******
	//  3      +++            ++        +++          ++
	//     012345678901234567890123456789012345678901234
	//               1         2         3         4
	//  4-7 19-21 29-32 42-44
	cout << endl;
	vector<ScalarRange> first;
	vector<ScalarRange> second;

	first.push_back( ScalarRange( 4, 10));
	first.push_back( ScalarRange(16, 24));
	first.push_back( ScalarRange(28, 34));
	first.push_back( ScalarRange(37, 39));
	first.push_back( ScalarRange(42, 44));

	second.push_back(ScalarRange( 0,  2));
	second.push_back(ScalarRange( 7, 12));
	second.push_back(ScalarRange(15, 19));
	second.push_back(ScalarRange(21, 22));
	second.push_back(ScalarRange(25, 29));
	second.push_back(ScalarRange(32, 39));

	vector<ScalarRange> difference;
	cout << "scalarRangeDifference" << endl;
	rangeDifference(first, second, difference);

	cout << "DIFFERENCES" << endl;
	for(unsigned int i=0; i < difference.size(); i++)
	{
		cout << " "<< i << ") " << difference[i] << endl;
	}
	CPPUNIT_ASSERT_EQUAL((size_t)4, difference.size());

	CPPUNIT_ASSERT_DOUBLES_EQUAL( 4, difference[0].min, tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL( 7, difference[0].max, tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(19, difference[1].min, tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(21, difference[1].max, tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(29, difference[2].min, tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(32, difference[2].max, tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(42, difference[3].min, tol );
	CPPUNIT_ASSERT_DOUBLES_EQUAL(44, difference[3].max, tol );

}

void RoofingTestCase::testDifferenceRangeEmpty()
{

// src = [-24.907, -9.22883]
// [-8.32378, -5.93331]
// [-5.41545, 24.907]
// del = (0 points )
// diff = [-24.907, -9.22883]

	cout << endl;
	vector<ScalarRange> first;
	vector<ScalarRange> second;

	first.push_back( ScalarRange( -24.907, -9.22883 ));
	first.push_back( ScalarRange( -8.32378, -5.93331));
	first.push_back( ScalarRange( -5.41545, 24.907));

//	second.push_back(ScalarRange(  ));


	vector<ScalarRange> difference;
	cout << "scalarRangeDifference" << endl;
	rangeDifference(first, second, difference);

	cout << "DIFFERENCES" << endl;
	dumpRanges(difference);

//	CPPUNIT_ASSERT_EQUAL((size_t)3, difference.size());


}

void RoofingTestCase::testSimpleUnion()
{

	//  *****
	//  		  *****
	//  0123456789012345678901234567890
	//            1         2         3

	ScalarRange range;
	bool r;

	r = scalarRangeUnion(ScalarRange(0,5),  ScalarRange(10,15), range );
	CPPUNIT_ASSERT(!r);

	r = scalarRangeUnion(ScalarRange(10,15),  ScalarRange(0,5), range );
	CPPUNIT_ASSERT(!r);

	//    *******
	//  	********
	//  0123456789012345678901234567890
	//            1         2         3
	r = scalarRangeUnion(ScalarRange(2,9),  ScalarRange(4,12), range );
	CPPUNIT_ASSERT(r);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(2, range.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(12, range.max, tol);

}

void RoofingTestCase::testSubRangeUnion()
{
	cout << endl;

	//              		   **********
	//	1      ******      ********    ******   **   **
	//     012345678901234567890123456789012345678901234
	//               1         2         3         4
	//  7-10 16-19 21-22 28-29 32-34
	vector<ScalarRange> first;

	first.push_back( ScalarRange( 4, 10));
	first.push_back( ScalarRange(16, 24));
	first.push_back( ScalarRange(28, 34));
	first.push_back( ScalarRange(37, 39));
	first.push_back( ScalarRange(42, 44));

	ScalarRange range(20, 30);

	vector<ScalarRange> result;
	vector<ScalarRange>::const_iterator it;
	it = subRangeUnion(range, first.begin(), first.end(), result);

	cout << "Result size " << result.size() << endl;

	for(unsigned int i=0; i < result.size(); i++)
	{
		const ScalarRange &range = result[i];
		cout << i << ") " << range << endl;
	}

	CPPUNIT_ASSERT_EQUAL((size_t)2,  result.size());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4,  result[0].min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(10, result[0].max, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(16, result[1].min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(34, result[1].max, tol);

}


void RoofingTestCase::testRangeUnion()
{
	cout<< endl;
	//	1      ******      ********    ******   **   **
	//  2  **     *****   ****  *    ***   *******
	//  3  ++  ++++++++   +++++++++  +++++++++++++   ++
	//     012345678901234567890123456789012345678901234
	//               1         2         3         4

	vector<ScalarRange> first;
	vector<ScalarRange> second;

	first.push_back( ScalarRange( 4, 10));
	first.push_back( ScalarRange(16, 24));
	first.push_back( ScalarRange(28, 34));
	first.push_back( ScalarRange(37, 39));
	first.push_back( ScalarRange(42, 44));

	second.push_back(ScalarRange( 0,  2));
	second.push_back(ScalarRange( 7, 12));
	second.push_back(ScalarRange(15, 19));
	second.push_back(ScalarRange(21, 22));
	second.push_back(ScalarRange(25, 29));
	second.push_back(ScalarRange(32, 39));

	vector<ScalarRange> result;

	rangeUnion(first, second, result);

	for(unsigned int i=0; i < result.size(); i++)
	{
		const ScalarRange &range = result[i];
		cout << i << ") " << range << endl;
	}
	CPPUNIT_ASSERT_EQUAL((size_t)5,  result.size());
}


void RoofingTestCase::testGridStruct()
{
    cout << endl;

    Limits limits;
    limits.grow(Vector3(5,5,0));
    limits.grow(Vector3(10,10,10));

    Grid grid(limits, 0.391);
    cout << "center = " << grid.readGridCenter() << endl;
//    cout << "X values" << endl;
//    for(unsigned int i=0; i < grid.readXvalues().size(); i++)
//    {
//        cout << " " << grid.readXvalues()[i] << endl;
//    }
//
//    cout << "Y values" << endl;
//    for(unsigned int i=0; i < grid.readYvalues().size(); i++)
//    {
//        cout << " " << grid.readYvalues()[i] << endl;
//    }

    CPPUNIT_ASSERT_EQUAL((size_t)13, grid.readXvalues().size());
    CPPUNIT_ASSERT_EQUAL((size_t)13, grid.readYvalues().size());
}


void RoofingTestCase::testFlatsurface()
{
	cout << endl;

	Scalar centerX = 5;
	Scalar centerY = 3;
	Scalar gridSpacing = 0.39;
	unsigned int skipCount = 0; // infill gaps between grid lines

	SegmentTable loops;
	loops.push_back(std::vector<LineSegment2>());

	SegmentTable innerLoops;
	loops.push_back(std::vector<LineSegment2>());

	addOuterHexLoop(loops[0], centerX, centerY);
	addInnerHexLoop(loops[1], centerX, centerY);

	Vector3 min(-12.1 + centerX, -12.1 + centerY, 0);
	Vector3 max( 12.1 + centerX,  12.1 + centerY, 1);
	Limits limits;
	limits.grow(min);
	limits.grow(max);

	Grid grid(limits, gridSpacing);

	GridRanges surface;
	grid.createGridRanges(loops, surface);

	GridRanges infills;
	grid.subSample(surface, skipCount, infills);

	Polygons xPolys;
	grid.polygonsFromRanges(infills, true, xPolys);

	Polygons yPolys;
	grid.polygonsFromRanges(infills, false, yPolys);

	string filename = outputDir + "hexagon_surface.scad";
	ScadDebugFile fscad;
    cout << "writing " << filename << endl;
	fscad.open(filename.c_str());
	addLinea(fscad);
    fscad.writeHeader();

	Scalar z = 0;
	Scalar dz = 0;

	writeScanLines(fscad, "outlines_", "linea", -1, -0.1, loops);
	fscad.writePolygons(  "draw_polys_x_","polys", xPolys, z, 0);
	fscad.writePolygons(  "draw_polys_y_","polys", yPolys, z, 0);
//  writeScanLines(fscad, "draw_x_", 	  "linea", z, dz, xSegs );
//  writeScanLines(fscad, "draw_y_", 	  "linea", 0.5, dz, ySegs );

    std::ostream & out = fscad.getOut();

    out << "draw_polys_x_0();" << endl;
    out << "draw_polys_y_0();" << endl;

//    out << "draw_x_all();" << endl;
//    out << "draw_y_all();" << endl;
    out << "outlines_all();" << endl;
    fscad.close();
}


/*
void SurfaceUnion(const FlatSurface& surface, const FlatSurface &)
{

}
*/

struct ModelSkeleton
{
	Grid grid;
	LayerMeasure layerMeasure;

	std::vector<SegmentTable>   outlines;

    std::vector<Insets> 		insets;
    std::vector<GridRanges>     flatSurfaces; // # number of slices + roofCount * 2

    std::vector<GridRanges>     roofings;
    std::vector<GridRanges>     floorings;

    std::vector<GridRanges>     infills;

    ModelSkeleton()
    :layerMeasure(0,0)
    {}
};

void dumpSegments(const char* prefix, const std::vector<LineSegment2> &segments)
{
	cout << prefix << "segments = [ // " << segments.size() << " segments" << endl;
    for(size_t id = 0; id < segments.size(); id++)
    {
    	LineSegment2 seg = segments[id];
    	cout  << " [[" << seg.a << ", " << seg.b << "]], // " << id << endl;
    }
    cout << prefix << "]" << endl;
    cout << "// color([1,0,0.4,1])loop_segments(segments,0.050000);" << endl;
}

class Gcodor
{
public:
	GCoder gcoder;

	void init(const GCoder &gcoderCfg)
	{
		this->gcoder = gcoderCfg;
	}

	void outlines(const SegmentTable& outlinesSegments, Polygons &boundary)
	{
		createPolysFromloopSegments(outlinesSegments, boundary);
	}

	void insets(const Insets& insetsForSlice, std::vector<Polygons> &insetPolys)
	{
		size_t nbOfShells = insetsForSlice.size();
		polygonsFromLoopSegmentTables(nbOfShells, insetsForSlice, insetPolys);
	}

	void infills(const GridRanges &infillRanges,
					const Grid &grid,
					bool direction,
					Polygons &infills)
	{
		grid.polygonsFromRanges(infillRanges, direction, infills);
	}

	void writeGcode(const char *gcodeFile, const char* modelSource, const std::vector<SliceData> &slices)
	{
		std::ofstream gout(gcodeFile);
		gcoder.writeStartOfFile(gout, modelSource);

		size_t sliceCount = slices.size();

		// progress.reset(sliceCount, "Gcoding");
		for(size_t sliceId=0; sliceId < sliceCount; sliceId++)
		{
			// progress.tick();
			const SliceData &slice = slices[sliceId];
			gcoder.writeSlice(gout, slice);
		}
		gout.close();
	}

};

class Slicor
{
public:
	SlicerConfig slicerCfg;

	size_t roofCount;
	size_t floorCount;
	Scalar gridSpacingMultiplier;
	size_t skipCount;
public:

	void init(	const SlicerConfig &slicerCfg,
				Scalar gridSpacingMultiplier,
				size_t roofCount,
				size_t floorCount,
				size_t  skipCount)
	{

		this->slicerCfg = slicerCfg;
		this->roofCount = roofCount;
		this->floorCount = floorCount;
		this->gridSpacingMultiplier = gridSpacingMultiplier;
		this->skipCount = skipCount;
	}


	void outlines( const char* modelFile, LayerMeasure &layerMeasure, Grid &grid, std::vector<SegmentTable> &outlines)
	{
		Meshy mesh(slicerCfg.firstLayerZ, slicerCfg.layerH);
		mesh.readStlFile(modelFile);

		// grid.init(mesh.limits, slicerCfg.layerW);
		unsigned int sliceCount = mesh.readSliceTable().size();
		outlines.resize(sliceCount);

		for(size_t sliceId =0; sliceId < sliceCount; sliceId++)
		{
			//cout << sliceId << "/" << sliceCount << " outlines" << endl;
			SegmentTable &segments = outlines[sliceId];
			outlinesForSlice(mesh, sliceId, segments);
		}

		Scalar gridSpacing = slicerCfg.layerW * gridSpacingMultiplier;
		Limits limits = mesh.readLimits();

		grid.init(limits, gridSpacing);

		layerMeasure = mesh.readLayerMeasure();
	}



    void insetsForSlice(const SegmentTable & sliceOutlines,
    					Insets & sliceInsets,
    					const char*scadFile=NULL)
    {


        bool writeDebugScadFiles = false;
        inshelligence(sliceOutlines,
        				slicerCfg.nbOfShells,
        				slicerCfg.layerW,
        				slicerCfg.insetDistanceMultiplier,
        				scadFile,
        				writeDebugScadFiles,
        				sliceInsets);
    }

    void insets(const std::vector<SegmentTable> & outlinesSegments, std::vector<Insets> & insets)
    {

        unsigned int sliceCount = outlinesSegments.size();

        insets.resize(sliceCount);
        // slice id must be adjusted for
        for(size_t i = 0;i < sliceCount;i++)
        {
        	const SegmentTable & sliceOutlines = outlinesSegments[i];
        	Insets & sliceInsets = insets[i];

            insetsForSlice(sliceOutlines, sliceInsets);
            				//scadFile,
            				//writeDebugScadFiles,
            				//insetsForSlice);
//            const char *scadFile = NULL;
//            bool writeDebugScadFiles = false;
//            inshelligence(sliceOutlines,
//            				slicerCfg.nbOfShells,
//            				slicerCfg.layerW,
//            				slicerCfg.insetDistanceMultiplier,
//            				scadFile,
//            				writeDebugScadFiles,
//            				insetsForSlice);
		}
	}


    void flatSurfaces(	const std::vector<Insets> & insets,
    					const Grid & grid,
    					std::vector<GridRanges> & gridRanges)
    {
        assert(gridRanges.size() == 0);
        unsigned int sliceCount = insets.size();
        gridRanges.resize(sliceCount);
        for(size_t i = 0;i < sliceCount;i++)
        {
        	const Insets & allInsetsForSlice = insets[i];
        	GridRanges & surface = gridRanges[i];
            gridRangesForSlice(allInsetsForSlice, grid, surface);
		}
	}

    void roofForSlice( const GridRanges & currentSurface, const GridRanges & surfaceAbove, const Grid & grid, GridRanges & roofing)
    {
        grid.gridRangeDifference(currentSurface, surfaceAbove, roofing);
    }

    void roofing(const std::vector<GridRanges> & flatSurfaces, const Grid & grid, std::vector<GridRanges> & roofings)
    {
        assert(flatSurfaces.size() > 0);
        assert(roofings.size() == 0);
        unsigned int sliceCount = flatSurfaces.size();
        roofings.resize(sliceCount);
        for(size_t i = 0;i < sliceCount - 1;i++){
            const GridRanges & currentSurface = flatSurfaces[i];
            const GridRanges & surfaceAbove = flatSurfaces[i + 1];
            GridRanges & roofing = roofings[i];
            roofForSlice(currentSurface, surfaceAbove, grid, roofing);
    	}
    	roofings[sliceCount -1] = flatSurfaces[sliceCount -1];
    }

    void infills(const std::vector<GridRanges> &flatSurfaces,
    			 const Grid &grid,
    			 const std::vector<GridRanges> &roofings,
    			 const std::vector<GridRanges> &floorings,
    			 std::vector<GridRanges> &infills)
    {
    	assert(infills.size() == 0);
    	assert(flatSurfaces.size() > 0);
    	assert(roofings.size() > 0);

        unsigned int sliceCount = flatSurfaces.size();
        infills.resize(sliceCount);
        for(size_t i=0; i< sliceCount; i++)
        {
        	cout  << "INFILL " << i << "/" << sliceCount << endl;
        	const GridRanges &surface = flatSurfaces[i];
        	const GridRanges &roofing = roofings[i];

        	/*
        	GridRanges sparseInfill;
        	cout << i << "/" << sliceCount << " subsample " << skipCount << endl;
        	grid.subSample(surface, skipCount, sparseInfill);


        	cout << " union " << i << endl;

        	grid.gridRangeUnion(sparseInfill, roofing, infill);
        	*/
        	GridRanges &infill = infills[i];
        	infill = roofing;
        }
    }

    void gridRangesForSlice(const Insets &allInsetsForSlice,
    						const Grid &grid,
    						GridRanges &surface)
    {
    	const SegmentTable & innerMostLoops = allInsetsForSlice.back();
    	grid.createGridRanges(innerMostLoops, surface);
    }



private:
//	void gridRangesForSlice(const std::vector<Insets> &insets,
//							const Grid &grid,
//							size_t sliceIndex,
//							std::vector<GridRanges> &gridRanges)
//    {
//        const Insets & allInsetsForSlice = insets[sliceIndex];
//        const SegmentTable & innerMostLoops = allInsetsForSlice.back();
//        GridRanges & surface = gridRanges[sliceIndex];
//        grid.createGridRanges(innerMostLoops, surface);
//    }

	void loopsFromLineSegments(const std::vector<LineSegment2>& unorderedSegments, Scalar tol, SegmentTable & segments)
    {
        // dumpSegments("unordered_", unorderedSegments);
        // cout << segments << endl;
        if(unorderedSegments.size() > 0){
            //cout << " loopsAndHoleOgy " << endl;
          	std::vector<LineSegment2> segs =  unorderedSegments;
            loopsAndHoleOgy(segs, tol, segments);
        }
    }

    void outlinesForSlice(const Meshy & mesh, size_t sliceId, SegmentTable & segments)
    {
        Scalar tol = 1e-6;
        const LayerMeasure & layerMeasure = mesh.readLayerMeasure();
        Scalar z = layerMeasure.sliceIndexToHeight(sliceId);
        const std::vector<Triangle3> & allTriangles = mesh.readAllTriangles();
        const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[sliceId];
        std::vector<LineSegment2> unorderedSegments;
        segmentationOfTriangles(trianglesForSlice, allTriangles, z, unorderedSegments);
        assert(segments.size() ==0);
        // dumpSegments("unordered_", unorderedSegments);
        // cout << segments << endl;
        loopsFromLineSegments(unorderedSegments, tol, segments);
		// cout << " done " << endl;
	}



};




void RoofingTestCase::testSkeleton()
{
	cout << endl;
	cout << "Skeleton" << endl;

	string configFileName = "miracle.config";
	unsigned int roofLayerCount = 3;
	unsigned int floorLayerCount = 3;

	cout << "read config" << endl;
    Configuration config;
   	config.readFromFile(configFileName.c_str());

	GCoder gcoderCfg;
	SlicerConfig slicerCfg;

	loadGCoderData(config, gcoderCfg);
	loadSlicerData(config, slicerCfg);

	string modelFile = "inputs/hexagon.stl"; // "inputs/3D_Knot.stl";
	string gcodeFile = "outputs/hexagon.gcode";  // "outputs/3D_Knot.gcode";

	cout << "read model " << modelFile << endl;

	Slicor slicor;
	slicor.init(slicerCfg, 0.95, 3, 3, 3);

	ModelSkeleton skeleton;

	cout << "outlines" << endl;
	slicor.outlines(modelFile.c_str(),
					skeleton.layerMeasure,
					skeleton.grid,
					skeleton.outlines);

	cout << "insets" << endl;
	slicor.insets(skeleton.outlines,
				  skeleton.insets);

	cout << "flat surfaces" << endl;
	slicor.flatSurfaces(skeleton.insets,
						skeleton.grid,
						skeleton.flatSurfaces);

	cout << "roofings" << endl;
	slicor.roofing(skeleton.flatSurfaces, skeleton.grid, skeleton.roofings);


	cout << "infills" << endl;
	slicor.infills( skeleton.flatSurfaces,
					skeleton.grid,
					skeleton.roofings,
					skeleton.floorings,
					skeleton.infills);


	cout << "slice data" << endl;
	std::vector<SliceData> slices;

	Gcodor gcodor;
	gcodor.init(gcoderCfg);

	size_t sliceCount = skeleton.outlines.size();

	slices.resize(sliceCount);
	bool direction = false;
	for(size_t i=0; i < sliceCount; i++)
	{
		direction = !direction;
		SliceData& slice = slices[i];

		Scalar z = skeleton.layerMeasure.sliceIndexToHeight(i);
		slice.updatePosition(z, i);
		slice.extruderSlices.resize(1);

		ExtruderSlice &extruderSlice = slice.extruderSlices[0];

		const Insets &insets = skeleton.insets[i];
		const SegmentTable &outlineSegments = skeleton.outlines[i];

		gcodor.outlines(outlineSegments, extruderSlice.boundary);
		gcodor.insets(insets, extruderSlice.insetLoopsList);

		const GridRanges &infillRanges = skeleton.infills[i];

		Polygons &infills = extruderSlice.infills;
		gcodor.infills(infillRanges, skeleton.grid, direction, infills);
	}

	cout << "gcode" << endl;
	gcodor.writeGcode(gcodeFile.c_str(), modelFile.c_str(), slices);
	cout << "done" << endl;
}


void RoofingTestCase::test3dKnotPlatform()
{
	cout << endl;
	string modelFile = "inputs/3D_Knot.stl";
	cout << "model "  << modelFile << endl;

	SlicerConfig slicerCfg;
	slicerCfg.firstLayerZ = 0.1;
	slicerCfg.layerH = 0.3;
	slicerCfg.layerW = 0.4;
	slicerCfg.nbOfShells = 3;
	slicerCfg.insetDistanceMultiplier = 0.95;

	Slicor slicor;
	slicor.init(slicerCfg, 0.95, 3, 3, 2);

	vector<SegmentTable> outlines;
	Grid grid;
	LayerMeasure layerMeasure(0,0);
	slicor.outlines(modelFile.c_str(), layerMeasure,
			grid,
			outlines);

	size_t sliceId = 18; // 20;

	const SegmentTable &sliceOutlines = outlines[sliceId];
	const SegmentTable &sliceAboveOutlines = outlines[sliceId+1];

	Insets insets;
	slicor.insetsForSlice(sliceOutlines, insets);
	Insets insetsAbove;
	slicor.insetsForSlice(sliceAboveOutlines, insetsAbove);
	GridRanges surface;
	slicor.gridRangesForSlice(insets, grid, surface);
	GridRanges surfaceAbove;
	slicor.gridRangesForSlice(insetsAbove, grid, surfaceAbove);

	GridRanges roofing;
	slicor.roofForSlice(surface, surfaceAbove, grid, roofing);

	// problems start at 23
	size_t lineNb = 27;

	vector<ScalarRange> srcLine= surface.xRays[lineNb];
	vector<ScalarRange> diffLine= surfaceAbove.xRays[lineNb];
	vector<ScalarRange> roofLine = roofing.xRays[lineNb];

	cout << "src = ";
	dumpRanges(srcLine);

	cout << "del = ";
	dumpRanges(diffLine);
	cout << "(" << diffLine.size() << " points )" << endl;

	cout << "diff = ";
	dumpRanges(roofLine);

	// todo: remove sliceId from call
	// slicor.gridRangesForSlice(sliceOutlines, skeleton.grid );

	string filename = outputDir + "knot_grid.scad";
	ScadDebugFile fscad;
	cout << "writing " << filename << endl;
	fscad.open(filename.c_str());
	addLinea(fscad);
	fscad.writeHeader();

	Scalar  z = 0;
	Scalar dz = 0;

	const SegmentTable &innerOutlines = *(insets.begin());

	writeScanLines(fscad, "outlines_", "linea", -1, -0.1 , innerOutlines);

	SegmentTable xRays;
    segmentTableFromRangeTable(surface.xRays , grid.readYvalues(), xRays);

	SegmentTable xRaysAbove;
    segmentTableFromRangeTable(surfaceAbove.xRays , grid.readYvalues(), xRaysAbove);

    SegmentTable xRaysRoof;
    segmentTableFromRangeTable( roofing.xRays , grid.readYvalues(), xRaysRoof);


	writeScanLines(fscad, "slice_x_", "linea",   z, dz, xRays);
	writeScanLines(fscad, "slice_above_x_", "linea", 0.5, dz, xRaysAbove);
	writeScanLines(fscad, "roof_x_", "linea", 0.5, dz, xRaysRoof);

	std::ostream & out = fscad.getOut();

	out << "slice_x_all();" << endl;
	out << "slice_above_x_all();" << endl;
	out << "outlines_all();" << endl;
	out << "roof_x_all();" << endl;
	fscad.close();

//	slicor.insets();
//	slicor.flatSurfaces()

/*
	Scalar firstLayerZ = 0.1;
	Scalar layerH = 0.3;
	string modelFile = "inputs/3D_Knot";

	slicerCfg.nbOfShells,
							  slicerCfg.layerW,
							  i,
							  slicerCfg.insetDistanceMultiplier,
							  scadFile,
							  writeDebugScadFiles,
							  insetsForSlice);

				polygonsFromLoopSegmentTables(slicerCfg.nbOfShells
	*/

}

/*

doit(modelFile, size_t floorThick, size_t ceilingThick)
{

	mesh(ModelFile);

	addFloorThick empty layers to flatSurfaces

	FUTURE
	From top down:
		support grid.


	for each layer in mesh:
		outlines segments
		order loops
		insets
		flatSurface

	add ceilingThick empty layers FlatSurfaces



	for layer:
		roofing  flat surface - loyer above's flat
		flooring

	for each layer:
		infill = sum(next roofings, previous flooring, flatSurfaces)


////////////////

	addFloorThick empty layers to flatSurfaces

	Priming:
	for # of ceilThick (+1):
		outlines
		insets
		flatSurface
	for # of ceilThick (+1):
		roofing
		flooring


	for slice 0:
		infill (flat + sum floors + roofs)

	for each slice:
		outlines (*) done by support
		insets
		flatsurface
		roofing
		flooring



/////////////////////

	 ?? start with ceilThick empty slices

	for slice len-1 down to 0:
		unordered outlines
		support region:
			flat slice - flat slice below

	box of 2 slices:
		enter box, outline, surface
		if box is full, support =
}

 */
