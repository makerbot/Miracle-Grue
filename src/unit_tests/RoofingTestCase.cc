#include <cppunit/config/SourcePrefix.h>
#include "UnitTestUtils.h"

#include "RoofingTestCase.h"

#include "UnitTestUtils.h"
#include "mgl/miracle.h"


CPPUNIT_TEST_SUITE_REGISTRATION( RoofingTestCase );

using namespace std;
using namespace mgl;

MyComputer computer;

string outputDir ("outputs/test_cases/roofingTestCase/");

string inputDir("./test_cases/roofingTestCase/stls/");


class RayException : public mgl::Exception {	public: RayException(const char *msg) :Exception(msg){} };



bool intersectRange(Scalar a, Scalar b, Scalar c, Scalar d, Scalar &begin, Scalar &end)
{
	assert(b>=a);
	assert(d>=c);

//	cout << "[" << a << ", " << b << "] inter ";
//  cout << "[" << c << ", " << d << "]";

	if(a >= d)
	{
		cout << " = 0" << endl;
		return false;
	}

	if (c >=b )
	{
		cout << " = 0" << endl;
		return false;
	}

	if(a >= c)
	{
		begin = a;
	}
	else
	{
		begin = c;
	}

	if( b >= d)
	{
		end = d;
	}
	else
	{
		end = b;
	}
//	cout << " = [" << begin << ", " << end << "]" << endl;
	return true;
}

struct ScalarRange
{
	Scalar min;
	Scalar max;
	ScalarRange()
	:min(0), max(0)
	{

	}

	ScalarRange(Scalar a, Scalar b)
	:min(a), max(b)
	{
		// assert(b>a);
	}

	ScalarRange(const ScalarRange& original)
	{
		this->min = original.min;
		this->max = original.max;
	}

	// default assignment operator (min and max are copied)
	void operator = (const ScalarRange& next)
	{
		if( &next != this)
		{
			this->min = next.min;
			this->max = next.max;
		}
	}
};


typedef std::vector< std::vector<ScalarRange> > ScalarRangeTable;



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

void scalarRangesFromIntersections(const std::set<Scalar> &lineCuts, std::vector<ScalarRange> &ranges)
{
	ranges.reserve(lineCuts.size());
    bool inside = false;
    Scalar xBegin = 0; // initial value is not used
    Scalar xEnd = 0;   // initial value is not used
    for(std::set<Scalar>::iterator it = lineCuts.begin(); it != lineCuts.end(); it++)
	{
    	Scalar intersection = *it;
    	if(inside)
    	{
    		xEnd = intersection;
    		// gridSegments.push_back(LineSegment2(Vector2(xBegin,y), Vector2(xEnd,y)));
    		ranges.push_back(ScalarRange(xBegin, xEnd));
    	}
    	else
    	{
    		xBegin = intersection;
    	}
    	inside = !inside;
	}

    if(inside)
    {
    	// this is not good. xMax should be outside the object
    	RayException messup("Ray has been cast outside the model mesh.");

    }
}

void rayCastAlongX(	const SegmentTable &outlineLoops,
									Scalar y,
									Scalar xMin,
									Scalar xMax,
									std::vector<ScalarRange> &ranges)
{
    std::set<Scalar> lineCuts;

    // go through all the segments in every loop
    for(unsigned int j=0; j< outlineLoops.size(); j++)
    {
        const std::vector<LineSegment2> &outlineLineSegment2s = outlineLoops[j];
        for(std::vector<LineSegment2>::const_iterator it= outlineLineSegment2s.begin(); it!= outlineLineSegment2s.end(); it++)
        {
            const LineSegment2 &segment = *it;
            Scalar intersectionX, intersectionY;
            if (segmentSegmentIntersection( xMin,
                                            y,
                                            xMax,
                                            y,
                                            segment.a.x,
                                            segment.a.y,
                                            segment.b.x,
                                            segment.b.y,
                                            intersectionX,
                                            intersectionY))
            {
                lineCuts.insert(intersectionX);
            }
        }
    }

    scalarRangesFromIntersections(lineCuts, ranges);

}

void rayCastAlongY(	const SegmentTable &outlineLoops,
						Scalar x,
						Scalar yMin,
						Scalar yMax,
						std::vector<ScalarRange> &ranges)
{
    std::set<Scalar> lineCuts;

    // go through all the segments in every loop
    for(unsigned int j=0; j< outlineLoops.size(); j++)
    {
        const std::vector<LineSegment2> &outlineLineSegment2s = outlineLoops[j];
        for(std::vector<LineSegment2>::const_iterator it= outlineLineSegment2s.begin(); it!= outlineLineSegment2s.end(); it++)
        {
            const LineSegment2 &segment = *it;
            Scalar intersectionX, intersectionY;
            if (segmentSegmentIntersection( x,
                                            yMin,
                                            x,
                                            yMax,
                                            segment.a.x,
                                            segment.a.y,
                                            segment.b.x,
                                            segment.b.y,
                                            intersectionX,
                                            intersectionY))
            {
                lineCuts.insert(intersectionY);
            }
        }
    }
    scalarRangesFromIntersections(lineCuts, ranges);

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

void castRaysOnSliceAlongX(const SegmentTable &outlineLoops,
					const std::vector<Scalar> &yValues,
					Scalar xMin,
					Scalar xMax,
					ScalarRangeTable &rangeTable)
{
	assert(rangeTable.size() == 0);
	rangeTable.resize(yValues.size());
	for(size_t i=0; i < rangeTable.size(); i++)
	{
		Scalar y = yValues[i];
		std::vector<ScalarRange> &ranges = rangeTable[i];
	    rayCastAlongX(outlineLoops, y, xMin, xMax, ranges);
	}
}

void castRaysOnSliceAlongY(const SegmentTable &outlineLoops,
					const std::vector<Scalar> &values, // x
					Scalar min,
					Scalar max,
					ScalarRangeTable &rangeTable)
{
	assert(rangeTable.size() == 0);
	rangeTable.resize(values.size());
	for(size_t i=0; i < rangeTable.size(); i++)
	{
		Scalar value = values[i];
		std::vector<ScalarRange> &ranges = rangeTable[i];
	    rayCastAlongY(outlineLoops, value, min, max, ranges);
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

	cout << "HO!" << endl;
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
					SegmentTable &rayTable)
{
	Scalar h = z;
	for(size_t i=0; i < rayTable.size(); i++)
	{
		std::vector<LineSegment2>& line = rayTable[i];
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


std::ostream& operator << (std::ostream &os,const ScalarRange &pt);

std::ostream& operator << (std::ostream &os,const ScalarRange &p)
{
	cout << "[" << p.min << ", " << p.max << "]";
	return os;
}

// return false if the ranges don't intersect
//
bool rangeUnion(const ScalarRange& range0, const ScalarRange& range1, ScalarRange &resultRange)
{
	// cout << " union( " << range0 << ", " << range1 << ")=";
	if( (range1.min > range0.max) || (range0.min > range1.max) )
	{
		// cout << "0" << endl;
		return false;
	}

	resultRange.min = range0.min < range1.min?range0.min:range1.min;
	resultRange.max = range0.max > range1.max?range0.max:range1.max;

	// cout << resultRange<<endl;
	return true;
}

// removes diffRange from srcRange. The result is put in resultRange, and srcRange is updated
// returns false if there is no resultRange
bool rangeDiff(const ScalarRange& diffRange, ScalarRange& srcRange, ScalarRange &resultRange)
{
	// cout << srcRange << " - " << diffRange << " = ";
	// the diffRange is left of srcRange ... no result
	if(diffRange.max <= srcRange.min)
	{
		// cout << "0 (before)" << endl;
		return false;
	}

	// the diff covers the src
	// the src is (partially) occluded
	if(diffRange.min <= srcRange.min)
	{
		if(diffRange.max >= srcRange.max )
		{
			srcRange.min = srcRange.max;
			// cout << "0 (occlusion)" << endl;
			return false;
		}
		// else... adjust the srcRange and make it smaller
		srcRange.min = diffRange.max;
		// cout << "0 partial occlusion, leftover = " << srcRange << endl;
		return false;

	}

	// intersection of the ranges
	if( (diffRange.min >= srcRange.min)  )
	{
		resultRange.min = srcRange.min;
		resultRange.max = diffRange.min;

		// left over on the right side
		if(diffRange.max <= srcRange.max)
		{
			srcRange.min = diffRange.max;
		}
		else
		{
			srcRange.min = srcRange.max;
		}
		// cout << resultRange << " (intersection!) leftover " <<  srcRange << endl;
		return true;
	}

	// srcRange is not occluded by diffRange which
	// is right of scrRange
	// there is nothing to remove: the result is the range
	if(diffRange.min >= srcRange.max)
	{
		resultRange = srcRange;

		// remove srcRange so it is
		// not used twice
		srcRange.min = srcRange.max;
		// cout << resultRange << " (all in!) leftover " <<  srcRange << endl;
		return true;
	}

	cout << "PROBLEM!" << endl;
	assert(0);
	return false;
}

vector< ScalarRange >::const_iterator  subRangeUnion(const ScalarRange &initialRange,
		 	 	 	 	vector< ScalarRange >::const_iterator it,
		 	 	 	 	vector< ScalarRange >::const_iterator itEnd,
						vector< ScalarRange > &result )
{

	ScalarRange range(initialRange);
	// cout << endl <<"-- subRangeUnion --" << endl;
	while(it != itEnd)
	{
		const ScalarRange &itRange = *it;
		if( (itRange.min > range.max)  )
		{
			// cout << " -PUSH" << range << endl;
			result.push_back(range);
			return it;
		}
		ScalarRange newRange;
		bool u = rangeUnion(range, itRange, newRange);
		if(u)
		{
			// cout << " -RANGE=" << range << endl;
			range = newRange;
		}
		else
		{
			// cout << " -PUSH" << range << endl;
			result.push_back(itRange);
		}
		it++;
	}
	// cout << " -done!" <<endl;
	result.push_back(range);
}


void rangeUnion( const vector< ScalarRange > &firstLine,
		 	 	 	  const vector< ScalarRange > &secondLine,
		 	 	 	  vector< ScalarRange > &unionLine )
{
	vector< ScalarRange >::const_iterator itOne = firstLine.begin();
	vector< ScalarRange >::const_iterator itTwo = secondLine.begin();

	while(itOne != firstLine.end())
	{
		const ScalarRange &range = *itOne;
		// cout << "range=" << range << endl;

		// check that the last range has not advanced beyond the firstLine
		if(unionLine.size() >0)
		{
			ScalarRange &lastUnion = unionLine.back();
			// cout << "LAST RANGE UPDATE COMPARE: last=" << lastUnion << " range=" << range;
			if(range.min <= lastUnion.max && lastUnion.max >= range.max)
			{
				// cout << " !UPDATE ONLY" << endl;
				lastUnion.max = range.max;
				itOne++;
				continue;
			}
		}
		// cout << " !no update" << endl;
		if(itTwo == secondLine.end())
		{
			unionLine.push_back(range);
		}
		else
		{
			itTwo = subRangeUnion(range, itTwo, secondLine.end(), unionLine);
		}
		itOne++;
	}
}

vector< ScalarRange >::const_iterator  subRangeDifference(	const ScalarRange &initialRange,
		 	 	 	 	vector< ScalarRange >::const_iterator it,
		 	 	 	 	vector< ScalarRange >::const_iterator itEnd,
						vector< ScalarRange > &result )
{
	ScalarRange range(initialRange);
	while(it != itEnd)
	{
		const ScalarRange &itRange = *it;
		if( (itRange.min >= range.max)  )
		{
			return it;
		}

		ScalarRange difference;
		if (rangeDiff(itRange, range, difference))
		{
			result.push_back(difference);
		}
		if(range.min >= range.max) // the leftover range has no length
		{
			// cout << "no left over" << endl;
			return it;
		}

		it ++;
	}
	// add the left over (if any)
	if(range.max > range.min)
	{
		result.push_back(range);
	}
	return it;
}

void rangeDifference(const vector< ScalarRange > &srcLine,
		 	 	 	  const vector< ScalarRange > &delLine,
		 	 	 	  vector< ScalarRange > &diffLine )
{
	vector< ScalarRange >::const_iterator itOne = srcLine.begin();
	vector< ScalarRange >::const_iterator itTwo = delLine.begin();
	while(itOne != srcLine.end())
	{
		const ScalarRange &range = *itOne;
		// cout << "range=" << range << endl;
		itTwo = subRangeDifference(range, itTwo, delLine.end(), diffLine);
		if(itTwo == delLine.end())
		{
			return;
		}
		itOne++;
	}
}

// computes the difference between the ranges of two layers
void rangeTableDifference(const ScalarRangeTable &src, const ScalarRangeTable &del, ScalarRangeTable &diff)
{
	size_t lineCount = src.size();
	assert(lineCount == del.size());
	diff.resize(lineCount);

	for(size_t i=0; i < src.size(); i++ )
	{
		const vector<ScalarRange> &lineRangeSrc = src[i];
		const vector<ScalarRange> &lineRangeDel = del[i];
		vector<ScalarRange> &lineRangeDiff = diff[i];

		rangeDifference(lineRangeSrc, lineRangeDel, lineRangeDiff);
	}
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

vector< ScalarRange >::const_iterator  subRangeTersect(	const ScalarRange &range,
		 	 	 	 	vector< ScalarRange >::const_iterator it,
		 	 	 	 	vector< ScalarRange >::const_iterator itEnd,
						vector< ScalarRange > &result )
{

	while(it != itEnd)
	{
		const ScalarRange &currentRange = *it;
		if( (it->min >= range.max)  )
		{
			// cout << " subrange done" << endl; // << currentRange << endl;
			return it;
		}

		ScalarRange intersection;
		// cout << " second="<< currentRange << endl;
		if( intersectRange(range.min, range.max, currentRange.min, currentRange.max, intersection.min, intersection.max) )
		{
			// cout << " Intersect: [" << range.min << ", " << range.max << "]"<< endl;
			result.push_back(intersection);
		}
		it ++;
	}
	return it;
}



void rangeTersection(const vector< ScalarRange > &oneLine,
					 const vector< ScalarRange > &twoLine,
						vector< ScalarRange > &boolLine )
{
	vector< ScalarRange >::const_iterator itOne = oneLine.begin();
	vector< ScalarRange >::const_iterator itTwo = twoLine.begin();
	while(itOne != oneLine.end())
	{
		const ScalarRange &range = *itOne;
		cout << "range=" << range << endl;
		itTwo = subRangeTersect(range, itTwo, twoLine.end(), boolLine);
		if(itTwo == twoLine.end())
		{
			itOne++;
			if(itOne != oneLine.end())
			{
				const ScalarRange &lastRange = twoLine.back();
				cout << "lastRange=" << lastRange << endl;
				subRangeTersect(lastRange, itOne, oneLine.end(), boolLine);
			}
			return;
		}
		itOne++;
	}
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

	b = rangeDiff(diff, src, res);

	CPPUNIT_ASSERT(b);

	CPPUNIT_ASSERT_DOUBLES_EQUAL(1, res.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2, res.max, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4, src.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5, src.max, tol);

	src = ScalarRange(2,4);
	diff = ScalarRange(1,5);
	b = rangeDiff(diff, src, res);
	CPPUNIT_ASSERT(!b);

	src = ScalarRange(1,5);
	diff = ScalarRange(3,6);
	b = rangeDiff(diff, src, res);
	CPPUNIT_ASSERT(b);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1, res.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3, res.max, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5, src.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5, src.max, tol);

	src = ScalarRange(3,5);
	diff = ScalarRange(1,4);
	b = rangeDiff(diff, src, res);
	CPPUNIT_ASSERT(!b);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4, src.min, tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(5, src.max, tol);

	src = ScalarRange(1,3);
	diff = ScalarRange(2,4);
	b = rangeDiff(diff, src, res);
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
	cout << "rangeDiff" << endl;
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

void RoofingTestCase::testSimpleUnion()
{

	//  *****
	//  		  *****
	//  0123456789012345678901234567890
	//            1         2         3

	ScalarRange range;
	bool r;

	r = rangeUnion(ScalarRange(0,5),  ScalarRange(10,15), range );
	CPPUNIT_ASSERT(!r);

	r = rangeUnion(ScalarRange(10,15),  ScalarRange(0,5), range );
	CPPUNIT_ASSERT(!r);

	//    *******
	//  	********
	//  0123456789012345678901234567890
	//            1         2         3
	r = rangeUnion(ScalarRange(2,9),  ScalarRange(4,12), range );
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
	cout << "rangeDiff" << endl;
	rangeUnion(first, second, result);

	for(unsigned int i=0; i < result.size(); i++)
	{
		const ScalarRange &range = result[i];
		cout << i << ") " << range << endl;
	}
	CPPUNIT_ASSERT_EQUAL((size_t)5,  result.size());
}



struct Grid
{
    vector<Scalar> yValues;
    vector<Scalar> xValues;
    Vector2 gridCenter;


    Grid(const Limits &limits, Scalar gridSpacing)
    {
        Scalar deltaY = limits.yMax - limits.yMin;
        Scalar deltaX = limits.xMax - limits.xMin;

        gridCenter[0] = limits.xMin + 0.5 * deltaX;
        gridCenter[1] = limits.yMin + 0.5 * deltaY;

        // round to nearest odd number
        unsigned int yGridSize = (unsigned int)( deltaY / gridSpacing) + 1;
        unsigned int xGridSize = (unsigned int)( deltaX / gridSpacing) + 1;

        yValues.resize(yGridSize);
        for (unsigned int i=0; i < yGridSize; i++)
        {
            Scalar v = gridCenter[1] -0.5 * deltaY + i * gridSpacing;
            yValues[i] = v;
        }

        xValues.resize(xGridSize);
        for (unsigned int i=0; i < xGridSize; i++)
        {
            Scalar v = gridCenter[0] -0.5 * deltaX + i * gridSpacing;
            xValues[i] = v;
        }
    }
};

void RoofingTestCase::testGridStruct()
{
    cout << endl;

    Limits limits;
    limits.grow(Vector3(5,5,0));
    limits.grow(Vector3(10,10,10));

    Grid grid(limits, 0.391);
    cout << "center = " << grid.gridCenter << endl;
    cout << "X values" << endl;
    for(unsigned int i=0; i < grid.xValues.size(); i++)
    {
        cout << " " << grid.xValues[i] << endl;
    }

    cout << "Y values" << endl;
    for(unsigned int i=0; i < grid.yValues.size(); i++)
    {
        cout << " " << grid.yValues[i] << endl;
    }

    CPPUNIT_ASSERT_EQUAL((size_t)12, grid.xValues.size());
    CPPUNIT_ASSERT_EQUAL((size_t)12, grid.yValues.size());
}


struct GridRanges
{
    ScalarRangeTable xRays;
    ScalarRangeTable yRays;
};

void gridCast (const SegmentTable &loops, const Grid &grid, GridRanges &surface)
{
	Vector2 toRotationCenter(grid.gridCenter * -1);

	Scalar angle = M_PI_2;

	SegmentTable xRays, yRays;

    assert(xRays.size() == 0);
    assert(yRays.size() == 0);

    ScalarRangeTable xRangeTable;
    Scalar xMin = grid.xValues[0];
    Scalar xMax = grid.xValues.back();
    castRaysOnSliceAlongX(loops, grid.yValues, xMin, xMax, surface.xRays);

    ScalarRangeTable yRangeTable;
    Scalar yMin = grid.yValues[0];
    Scalar yMax = grid.yValues.back();
    castRaysOnSliceAlongY(loops, grid.xValues, yMin, yMax, surface.yRays);

}



void infillLines(const GridRanges &surface, unsigned int skipCount, GridRanges &result)
{

	result.xRays.resize(surface.xRays.size());
	result.yRays.resize(surface.yRays.size());

	for(size_t i=0; i < surface.xRays.size(); i++)
	{
		result.xRays[i] = surface.xRays[i]; // deep copy of the ranges for the selected lines
		i += skipCount; // skip lines depending on selected infill density
	}

	for(size_t i=0; i < surface.yRays.size(); i++)
	{
		result.yRays[i] = surface.yRays[i]; // deep copy of the ranges for the selected lines
		i += skipCount; // skip lines depending on selected infill density
	}
}

void polygonsFromScalarRangesAlongX( const ScalarRangeTable &rays,	   // the ranges along x, multiple per lines
								const std::vector<Scalar> &values, // the y values for each line
								Polygons &polygons)				   // the output
{
	// change direction of extrusion
	// for each line
	bool forward = false;
	assert(rays.size() == values.size());

	for(size_t rayId =0; rayId < rays.size(); rayId++)
	{
		const vector<ScalarRange> &ray = rays[rayId];
		if(ray.size() == 0)
			continue;

		forward = !forward;
		Scalar value = values[rayId];

		vector<ScalarRange> line;
		line.reserve(ray.size());
		if(forward)
		{
			line = ray;
		}
		else
		{
			// reverse the ranges in the ray
			for(size_t i=0; i < ray.size(); i++)
			{
				ScalarRange range(ray[i].max, ray[i].min);
				line.push_back(range);
			}
		}
		for(size_t i= 0; i< line.size(); i++ )
		{
			const ScalarRange &range = line[i];
			Vector2 begin(range.min, value);
			Vector2 end(range.max, value);

			// add a polygon
			polygons.push_back(Polygon());
			Polygon &poly = polygons.back();
			poly.push_back(begin);
			poly.push_back(end);
		}
	}
}

void polygonsFromScalarRangesAlongY( const ScalarRangeTable &rays,	   // the ranges along x, multiple per lines
								const std::vector<Scalar> &values, // the x values for each line
								Polygons &polygons)				   // the output
{
	// change direction of extrusion
	// for each line
	bool forward = false;
	assert(rays.size() == values.size());

	for(size_t rayId =0; rayId < rays.size(); rayId++)
	{
		const vector<ScalarRange> &ray = rays[rayId];
		if(ray.size() == 0)
			continue;

		forward = !forward;
		Scalar value = values[rayId];

		vector<ScalarRange> line;
		line.reserve(ray.size());
		if(forward)
		{
			line = ray;
		}
		else
		{
			// reverse the ranges in the ray
			for(size_t i=0; i < ray.size(); i++)
			{
				ScalarRange range(ray[i].max, ray[i].min);
				line.push_back(range);
			}
		}
		for(size_t i= 0; i< line.size(); i++ )
		{
			const ScalarRange &range = line[i];
			Vector2 begin(value, range.min);
			Vector2 end(value, range.max);

			// add a polygon
			polygons.push_back(Polygon());
			Polygon &poly = polygons.back();
			poly.push_back(begin);
			poly.push_back(end);
		}
	}
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
	gridCast(loops, grid, surface);

	GridRanges infills;
	infillLines(surface, skipCount, infills);

	Polygons xPolys;
	polygonsFromScalarRangesAlongX(infills.xRays, grid.yValues, xPolys);

	Polygons yPolys;
	polygonsFromScalarRangesAlongY(infills.yRays, grid.xValues, yPolys);


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
//    writeScanLines(fscad, "draw_x_", 	  "linea", z, dz, xSegs );
//    writeScanLines(fscad, "draw_y_", 	  "linea", 0.5, dz, ySegs );

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





