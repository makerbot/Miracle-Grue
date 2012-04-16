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

	cout << "[" << a << ", " << b << "] inter ";
    cout << "[" << c << ", " << d << "]";

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
	cout << " = [" << begin << ", " << end << "]" << endl;
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
		assert(b>a);
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


void scalarRangesFromIntersections(const std::set<Scalar> &lineCuts, std::vector<ScalarRange> &ranges)
{
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

void rayCast(const SegmentTable &outlineLoops,
				Scalar y,
				Scalar xMin,
				Scalar xMax,
				std::vector<LineSegment2> &gridSegments)
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
    bool inside = false;
    Scalar xBegin = xMin;
    Scalar xEnd = xMin;
    for(std::set<Scalar>::iterator it = lineCuts.begin(); it != lineCuts.end(); it++)
	{
    	Scalar intersection = *it;
    	if(inside)
    	{
    		xEnd = intersection;
    		gridSegments.push_back(LineSegment2(Vector2(xBegin,y), Vector2(xEnd,y)));
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

void multiRayCast(	const SegmentTable &loops,
					const std::vector<Scalar> &yValues,
					Scalar xMin,
					Scalar xMax,
					SegmentTable &rayTable)
{
	assert(rayTable.size() == 0);
	rayTable.resize(yValues.size());
	for(size_t i=0; i < yValues.size(); i++)
	{
		Scalar y = yValues[i];
		rayCast(loops, y, xMin, xMax, rayTable[i]);
	}
}

void gridCast(	const SegmentTable &loops,
				const std::vector<Scalar> &xValues,
				Scalar yMin,
				Scalar yMax,
				const std::vector<Scalar> &yValues,
				Scalar xMin,
				Scalar xMax,
				Scalar angle,
				SegmentTable &xRays,
				SegmentTable &yRays,
				SegmentTable &rotatedLoops)
{
	assert(xRays.size() == 0);
	assert(yRays.size() == 0);
	assert(rotatedLoops.size() == 0);

	rotatedLoops = loops;
	rotateLoops(rotatedLoops, angle);

	multiRayCast(loops, yValues, xMin, xMax, xRays);

	multiRayCast(rotatedLoops, xValues, yMin, yMax, yRays);
	rotateLoops(yRays, -angle);
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

	SegmentTable loops;
	loops.push_back(vector<LineSegment2>());

	LineSegment2 s= LineSegment2(Vector2(0,0), Vector2(0,1));
	loops[0].push_back(s);
	loops[0].push_back(LineSegment2(Vector2(1,0), Vector2(1,1)) );
	std::vector<LineSegment2> intersects;
	rayCast(loops, 0.5, -1, 2, intersects);

	double tol = 1e-6;

	CPPUNIT_ASSERT_EQUAL((size_t)1, intersects.size());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, intersects[0].a[0], tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, intersects[0].b[0], tol);

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

void addOuterHexLoop(std::vector<LineSegment2>& segs)
{
	segs.push_back(LineSegment2(Vector2(10.0,  -5.773501), Vector2(9.89, -5.83701)));
	segs.push_back(LineSegment2(Vector2(9.89,  -5.83701), Vector2(0.0, -11.54701)));
	segs.push_back(LineSegment2(Vector2(0.0,   -11.54701), Vector2(-0.11, -11.483502)));
	segs.push_back(LineSegment2(Vector2(-0.11, -11.483502), Vector2(-10.0, -5.773501)));
	segs.push_back(LineSegment2(Vector2(-10.0,  -5.773501), Vector2(-10.0, -5.646484)));
	segs.push_back(LineSegment2(Vector2(-10.0,  -5.646484), Vector2(-10.0, 5.773504)));
	segs.push_back(LineSegment2(Vector2(-10.0, 	 5.773504), Vector2(-9.89, 5.837012)));
	segs.push_back(LineSegment2(Vector2(-9.89,	 5.837012), Vector2(0.0, 11.54701)));
	segs.push_back(LineSegment2(Vector2(0.0, 	11.54701), Vector2(0.11, 11.483502)));
	segs.push_back(LineSegment2(Vector2(0.11, 	11.483502), Vector2(10.0, 5.773504)));
	segs.push_back(LineSegment2(Vector2(10.0, 	 5.773504), Vector2(10.0, 5.646487)));
	segs.push_back(LineSegment2(Vector2(10.0, 	 5.646487), Vector2(10.0, -5.773501)));
}


void addLinea(ScadDebugFile &fscad)
{
    std::ostream & out = fscad.getOut();

    out << "" << endl;
    out << "module linea(segments, ball=true)" << endl;
    out << "{" << endl;
    out << "	if(ball) corner (x=segments[0][0][0],  y=segments[0][0][1], z=segments[0][0][2], diameter=0.25, faces=12, thickness_over_width=1);" << endl;
    out << "    for(seg = segments)" << endl;
    out << "    {" << endl;
    out << "        tube(x1=seg[0][0], y1=seg[0][1], z1=seg[0][2], x2=seg[1][0], y2=seg[1][1], z2=seg[1][2] , diameter1=0.1, diameter2=0.05, faces=4, thickness_over_width=1);" << endl;
    out << "    }" << endl;
    out << "}" << endl;

}

void writeScanLines(ScadDebugFile& fscad,
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
}

void RoofingTestCase::testHoly()
{
	cout << endl;

	SegmentTable loops;
	loops.push_back(std::vector<LineSegment2>());
	loops.push_back(std::vector<LineSegment2>());
	addOuterHexLoop(loops[0]);
	addInnerHexLoop(loops[1]);

	Scalar xMin = -11;
	Scalar xMax =  12;
	vector<Scalar> yValues;
	Scalar dy = 0.1;
	addValues(xMin, xMax, dy, yValues);

	SegmentTable rayTable;
	multiRayCast(loops, yValues, xMin, xMax, rayTable);

	for(size_t i=0; i < rayTable.size(); i++)
	{
		std::vector<LineSegment2>& line = rayTable[i];
		cout << i << ": " << yValues[i] << ") " << line.size() << endl << "\t";
		for(size_t j=0; j < line.size(); j++)
		{
			cout <<  line[j] << "\t";
		}
		cout << endl;
	}

	string filename = outputDir + "hexagon_lines.scad";
	ScadDebugFile fscad;
	fscad.open(filename.c_str());
	addLinea(fscad);
    fscad.writeHeader();

	Scalar z = 0;
	Scalar dz = 0;
    writeScanLines(fscad, "draw_x_", "scan_line_x_", z, dz, rayTable );
	fscad.close();
}



void RoofingTestCase::testGrid()
{
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

	Vector2 rotationCenter(0,0);
	Scalar angle = M_PI * 0.5;

	SegmentTable xRays, yRays;
	SegmentTable rotatedLoops;

	gridCast(loops,
			xValues,
			yMin,
			yMax,
			yValues,
			xMin,
			xMax,
			angle,
			xRays,
			yRays,
			rotatedLoops);

	string filename = outputDir + "hexagon_grid.scad";
	ScadDebugFile fscad;
	fscad.open(filename.c_str());
	addLinea(fscad);
    fscad.writeHeader();

	Scalar z = 0;
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






/*
void rangeTersection(const vector< ScalarRange > &oneLine,
					 const vector< ScalarRange > &twoLine,
						vector< ScalarRange > &boolLine )
{
	vector< ScalarRange >::const_iterator itOne = oneLine.begin();
	vector< ScalarRange >::const_iterator itTwo = twoLine.begin();
	unsigned int test = 0;
	while ( (itOne != oneLine.end())  && (itTwo != twoLine.end()) )
	{
		while(itOne->max >= itTwo->min && itTwo != twoLine.end())
		{
			ScalarRange range;
			cout << test << " test1) "  << *itOne  << " and " << *itTwo << endl;
			if( intersectRange(itOne->min, itOne->max, itTwo->min, itTwo->max, range.min, range.max) )
			{
				cout << " Intersect: [" << range.min << ", " << range.max << "]"<< endl;
				boolLine.push_back(range);
			}
			else
			{
				cout  << " no intersection" << endl;
			}
			test ++;
			cout << " Advancing two" << endl;
			itTwo++;
		}
		while(itTwo->max >= itOne->min && itOne != oneLine.end())
		{
			ScalarRange range;
			cout << test << " test2) "  << *itOne  << " and " << *itTwo << endl;
			if( intersectRange(itOne->min, itOne->max, itTwo->min, itTwo->max, range.min, range.max) )
			{
				cout << " Intersect: [" << range.min << ", " << range.max << "]"<< endl;
				boolLine.push_back(range);
			}
			else
			{
				cout  << " no intersection" << endl;
			}
			test ++;
			cout << " Advancing one" << endl;
			itOne++;
		}
	}
	cout << "Done" << endl;
}
*/




void lineTersect(const vector<LineSegment2> &oneLine,
					const vector<LineSegment2> &twoLine,
						vector<LineSegment2> &boolLine )
{

	cout << "lineTersect is UNDER CONSTRUCTION!!" << endl;
	size_t idTwo = 0;
	size_t idOne = 0;

	bool done = false;
	while (!done)
	{
		const LineSegment2 *pOne = &oneLine[idOne];
		const LineSegment2 *pTwo = &oneLine[idTwo];

		done = true;
	}
}

void intersectionWithSegmentTables(	const SegmentTable& one,
					const SegmentTable& two,
					SegmentTable &intersection)
{
	size_t count = one.size();
	assert(count == two.size());

	intersection.reserve(count);
	for (size_t i=0; i < count ; i++)
	{
		intersection.push_back(std::vector<LineSegment2>());
		const vector<LineSegment2> &oneLine = one[i];
		const vector<LineSegment2> &twoLine = two[i];
		vector<LineSegment2> &result = intersection[i];
		lineTersect(oneLine, twoLine, result);
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

	SegmentTable top;
	multiRayCast(slice0, yValues, xMin, xMax, top);

	SegmentTable bottom;
	multiRayCast(slice1, yValues, xMin, xMax, bottom);

	SegmentTable botTop;
	intersectionWithSegmentTables(top, bottom, botTop);

	string filename = outputDir + "intersect.scad";

	cout << filename << endl;

	ScadDebugFile fscad;
	fscad.open(filename.c_str());
	addLinea(fscad);
    fscad.writeHeader();


	Scalar z = 0;
	Scalar dz = 0;

	writeScanLines(fscad, "intersection_", "linea", -1, -0.1, botTop );
    writeScanLines(fscad, "top_", "linea", z, dz, top );
    writeScanLines(fscad, "bottom_", "linea", 0.5, dz, bottom );

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
				const ScalarRange &lastRange = *twoLine.rbegin();
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

// removes diffRange from srcRange. The result is put in resultRange, and srcRange is updated
// returns false if there is no resultRange
bool rangeDiff(const ScalarRange& diffRange, ScalarRange& srcRange, ScalarRange &resultRange)
{

	cout << srcRange << " - " << diffRange << " = ";
	// the diffRange is left of srcRange ... no result
	if(diffRange.max <= srcRange.min)
	{
		cout << "0 (before)" << endl;
		return false;
	}

	// the diff covers the src
	// the src is (partially) occluded
	if(diffRange.min <= srcRange.min)
	{
		if(diffRange.max >= srcRange.max )
		{
			srcRange.min = srcRange.max;
			cout << "0 (occlusion)" << endl;
			return false;
		}
		// else... adjust the srcRange and make it smaller
		srcRange.min = diffRange.max;
		cout << "0 partial occlusion, leftover = " << srcRange << endl;
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
		cout << resultRange << " (intersection!) leftover " <<  srcRange << endl;
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
		cout << resultRange << " (all in!) leftover " <<  srcRange << endl;
		return true;
	}

	cout << "PROBLEM!" << endl;
	assert(0);
	return false;
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

vector< ScalarRange >::const_iterator  subRangeDifference(	const ScalarRange &initialRange,
		 	 	 	 	vector< ScalarRange >::const_iterator it,
		 	 	 	 	vector< ScalarRange >::const_iterator itEnd,
						vector< ScalarRange > &result )
{
	ScalarRange range(initialRange);
	while(it != itEnd)
	{
		const ScalarRange &itRange = *it;
		if( (it->min >= range.max)  )
		{
			cout << " subrange done" << endl; // << currentRange << endl;
			return it;
		}

		ScalarRange difference;
		if (rangeDiff(itRange, range, difference))
		{
			result.push_back(difference);
		}
		if(range.min >= range.max) // the leftover range has no length
		{
			cout << "no left over" << endl;
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

void rangeDifference(const vector< ScalarRange > &oneLine,
		 	 	 	  const vector< ScalarRange > &twoLine,
		 	 	 	  vector< ScalarRange > &boolLine )
{
	vector< ScalarRange >::const_iterator itOne = oneLine.begin();
	vector< ScalarRange >::const_iterator itTwo = twoLine.begin();
	while(itOne != oneLine.end())
	{
		const ScalarRange &range = *itOne;
		cout << "range=" << range << endl;
		itTwo = subRangeDifference(range, itTwo, twoLine.end(), boolLine);
		if(itTwo == twoLine.end())
		{
			return;
		}
		itOne++;
	}
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

