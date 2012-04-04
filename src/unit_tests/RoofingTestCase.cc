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
    	RayException messup();

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
	loops[0].push_back(LineSegment2(Vector2(1,1), Vector2(1,0)) );

	std::vector<LineSegment2> intersects;
	rayCast(loops, 0.5, -1, 2, intersects);

	double tol = 1e-6;

	CPPUNIT_ASSERT_EQUAL((size_t)1, intersects.size());
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, intersects[0].a[0], tol);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, intersects[0].b[0], tol);

	cout << "HO!" << endl;
}

void RoofingTestCase::testHoly()
{
	cout << endl;
	SegmentTable loops;
	loops.push_back(std::vector<LineSegment2>());
	loops.push_back(std::vector<LineSegment2>());

	std::vector<LineSegment2>& segs = loops[0];
	loops[0].push_back(LineSegment2(Vector2(10.0,  -5.773501), Vector2(9.89, -5.83701)));
	loops[0].push_back(LineSegment2(Vector2(9.89,  -5.83701), Vector2(0.0, -11.54701)));
	loops[0].push_back(LineSegment2(Vector2(0.0,   -11.54701), Vector2(-0.11, -11.483502)));
	loops[0].push_back(LineSegment2(Vector2(-0.11, -11.483502), Vector2(-10.0, -5.773501)));
	loops[0].push_back(LineSegment2(Vector2(-10.0,  -5.773501), Vector2(-10.0, -5.646484)));
	loops[0].push_back(LineSegment2(Vector2(-10.0,  -5.646484), Vector2(-10.0, 5.773504)));
	loops[0].push_back(LineSegment2(Vector2(-10.0, 	 5.773504), Vector2(-9.89, 5.837012)));
	loops[0].push_back(LineSegment2(Vector2(-9.89,	 5.837012), Vector2(0.0, 11.54701)));
	loops[0].push_back(LineSegment2(Vector2(0.0, 	11.54701), Vector2(0.11, 11.483502)));
	loops[0].push_back(LineSegment2(Vector2(0.11, 	11.483502), Vector2(10.0, 5.773504)));
	loops[0].push_back(LineSegment2(Vector2(10.0, 	 5.773504), Vector2(10.0, 5.646487)));
	loops[0].push_back(LineSegment2(Vector2(10.0, 	 5.646487), Vector2(10.0, -5.773501)));

	loops[1].push_back(LineSegment2(Vector2(2.16506, -1.25), Vector2(2.1669, -1.24605)));
	loops[1].push_back(LineSegment2(Vector2(2.1669, -1.24605), Vector2(2.34923, -0.85505)));
	loops[1].push_back(LineSegment2(Vector2(2.34923, -0.85505), Vector2(2.35036, -0.85084)));
	loops[1].push_back(LineSegment2(Vector2(2.35036, -0.85084), Vector2(2.46202, -0.43412)));
	loops[1].push_back(LineSegment2(Vector2(2.46202, -0.43412), Vector2(2.4624, -0.42978)));
	loops[1].push_back(LineSegment2(Vector2(2.4624, -0.42978), Vector2(2.5, 0.0)));
	loops[1].push_back(LineSegment2(Vector2(2.5, 0.0), Vector2(2.49962, 0.00434)));
	loops[1].push_back(LineSegment2(Vector2(2.49962, 0.00434), Vector2(2.46202, 0.43412)));
	loops[1].push_back(LineSegment2(Vector2(2.46202, 0.43412), Vector2(2.46089, 0.43833)));
	loops[1].push_back(LineSegment2(Vector2(2.46089, 0.43833), Vector2(2.34923, 0.85505)));
	loops[1].push_back(LineSegment2(Vector2(2.34923, 0.85505), Vector2(2.34739, 0.859)));
	loops[1].push_back(LineSegment2(Vector2(2.34739, 0.859), Vector2(2.16506, 1.25)));
	loops[1].push_back(LineSegment2(Vector2(2.16506, 1.25), Vector2(2.16256, 1.25357)));
	loops[1].push_back(LineSegment2(Vector2(2.16256, 1.25357), Vector2(1.91511, 1.60697)));
	loops[1].push_back(LineSegment2(Vector2(1.91511, 1.60697), Vector2(1.91203, 1.61005)));
	loops[1].push_back(LineSegment2(Vector2(1.91203, 1.61005), Vector2(1.60697, 1.91511)));
	loops[1].push_back(LineSegment2(Vector2(1.60697, 1.91511), Vector2(1.6034, 1.91761)));
	loops[1].push_back(LineSegment2(Vector2(1.6034, 1.91761), Vector2(1.25, 2.16506)));
	loops[1].push_back(LineSegment2(Vector2(1.25, 2.16506), Vector2(1.24605, 2.16691)));
	loops[1].push_back(LineSegment2(Vector2(1.24605, 2.16691), Vector2(0.85505, 2.34923)));
	loops[1].push_back(LineSegment2(Vector2(0.85505, 2.34923), Vector2(0.85084, 2.35036)));
	loops[1].push_back(LineSegment2(Vector2(0.85084, 2.35036), Vector2(0.43412, 2.46202)));
	loops[1].push_back(LineSegment2(Vector2(0.43412, 2.46202), Vector2(0.42978, 2.4624)));
	loops[1].push_back(LineSegment2(Vector2(0.42978, 2.4624), Vector2(0.0, 2.5)));
	loops[1].push_back(LineSegment2(Vector2(0.0, 2.5), Vector2(-0.00434, 2.49962)));
	loops[1].push_back(LineSegment2(Vector2(-0.00434, 2.49962), Vector2(-0.43412, 2.46202)));
	loops[1].push_back(LineSegment2(Vector2(-0.43412, 2.46202), Vector2(-0.43833, 2.46089)));
	loops[1].push_back(LineSegment2(Vector2(-0.43833, 2.46089), Vector2(-0.85505, 2.34923)));
	loops[1].push_back(LineSegment2(Vector2(-0.85505, 2.34923), Vector2(-0.859, 2.34739)));
	loops[1].push_back(LineSegment2(Vector2(-0.859, 2.34739), Vector2(-1.25, 2.16506)));
	loops[1].push_back(LineSegment2(Vector2(-1.25, 2.16506), Vector2(-1.25357, 2.16256)));
	loops[1].push_back(LineSegment2(Vector2(-1.25357, 2.16256), Vector2(-1.60697, 1.91511)));
	loops[1].push_back(LineSegment2(Vector2(-1.60697, 1.91511), Vector2(-1.61005, 1.91203)));
	loops[1].push_back(LineSegment2(Vector2(-1.61005, 1.91203), Vector2(-1.91511, 1.60697)));
	loops[1].push_back(LineSegment2(Vector2(-1.91511, 1.60697), Vector2(-1.91761, 1.6034)));
	loops[1].push_back(LineSegment2(Vector2(-1.91761, 1.6034), Vector2(-2.16506, 1.25)));
	loops[1].push_back(LineSegment2(Vector2(-2.16506, 1.25), Vector2(-2.1669, 1.24605)));
	loops[1].push_back(LineSegment2(Vector2(-2.1669, 1.24605), Vector2(-2.34923, 0.85505)));
	loops[1].push_back(LineSegment2(Vector2(-2.34923, 0.85505), Vector2(-2.35036, 0.85084)));
	loops[1].push_back(LineSegment2(Vector2(-2.35036, 0.85084), Vector2(-2.46202, 0.43412)));
	loops[1].push_back(LineSegment2(Vector2(-2.46202, 0.43412), Vector2(-2.4624, 0.42978)));
	loops[1].push_back(LineSegment2(Vector2(-2.4624, 0.42978), Vector2(-2.5, 0.0)));
	loops[1].push_back(LineSegment2(Vector2(-2.5, 0.0), Vector2(-2.49962, -0.00434)));
	loops[1].push_back(LineSegment2(Vector2(-2.49962, -0.00434), Vector2(-2.46202, -0.43412)));
	loops[1].push_back(LineSegment2(Vector2(-2.46202, -0.43412), Vector2(-2.46089, -0.43833)));
	loops[1].push_back(LineSegment2(Vector2(-2.46089, -0.43833), Vector2(-2.34923, -0.85505)));
	loops[1].push_back(LineSegment2(Vector2(-2.34923, -0.85505), Vector2(-2.34739, -0.859)));
	loops[1].push_back(LineSegment2(Vector2(-2.34739, -0.859), Vector2(-2.16506, -1.25)));
	loops[1].push_back(LineSegment2(Vector2(-2.16506, -1.25), Vector2(-2.16256, -1.25357)));
	loops[1].push_back(LineSegment2(Vector2(-2.16256, -1.25357), Vector2(-1.91511, -1.60697)));
	loops[1].push_back(LineSegment2(Vector2(-1.91511, -1.60697), Vector2(-1.91203, -1.61005)));
	loops[1].push_back(LineSegment2(Vector2(-1.91203, -1.61005), Vector2(-1.60697, -1.91511)));
	loops[1].push_back(LineSegment2(Vector2(-1.60697, -1.91511), Vector2(-1.6034, -1.91761)));
	loops[1].push_back(LineSegment2(Vector2(-1.6034, -1.91761), Vector2(-1.25, -2.16506)));
	loops[1].push_back(LineSegment2(Vector2(-1.25, -2.16506), Vector2(-1.24605, -2.1669)));
	loops[1].push_back(LineSegment2(Vector2(-1.24605, -2.1669), Vector2(-0.85505, -2.34923)));
	loops[1].push_back(LineSegment2(Vector2(-0.85505, -2.34923), Vector2(-0.85084, -2.35036)));
	loops[1].push_back(LineSegment2(Vector2(-0.85084, -2.35036), Vector2(-0.43412, -2.46202)));
	loops[1].push_back(LineSegment2(Vector2(-0.43412, -2.46202), Vector2(-0.42978, -2.4624)));
	loops[1].push_back(LineSegment2(Vector2(-0.42978, -2.4624), Vector2(0.0, -2.5)));
	loops[1].push_back(LineSegment2(Vector2(0.0, -2.5), Vector2(0.00434, -2.49962)));
	loops[1].push_back(LineSegment2(Vector2(0.00434, -2.49962), Vector2(0.43412, -2.46202)));
	loops[1].push_back(LineSegment2(Vector2(0.43412, -2.46202), Vector2(0.43833, -2.46089)));
	loops[1].push_back(LineSegment2(Vector2(0.43833, -2.46089), Vector2(0.85505, -2.34923)));
	loops[1].push_back(LineSegment2(Vector2(0.85505, -2.34923), Vector2(0.859, -2.34739)));
	loops[1].push_back(LineSegment2(Vector2(0.859, -2.34739), Vector2(1.25, -2.16506)));
	loops[1].push_back(LineSegment2(Vector2(1.25, -2.16506), Vector2(1.25357, -2.16256)));
	loops[1].push_back(LineSegment2(Vector2(1.25357, -2.16256), Vector2(1.60697, -1.91511)));
	loops[1].push_back(LineSegment2(Vector2(1.60697, -1.91511), Vector2(1.61005, -1.91203)));
	loops[1].push_back(LineSegment2(Vector2(1.61005, -1.91203), Vector2(1.91511, -1.60697)));
	loops[1].push_back(LineSegment2(Vector2(1.91511, -1.60697), Vector2(1.91761, -1.6034)));
	loops[1].push_back(LineSegment2(Vector2(1.91761, -1.6034), Vector2(2.16506, -1.25)));


	Scalar y = -12;
	Scalar dy = 0.1;
	Scalar xMin = -11;
	Scalar xMax =  12;

	vector<Scalar> yValues;

	while(y <= 12)
	{
		yValues.push_back(y);
		y += dy;
	}

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

    std::ostream & out = fscad.getOut();
    out << "draw();" << endl;
    out << "" << endl;
    out << "module linea(segments, ball=true)" << endl;
    out << "{" << endl;
    out << "	if(ball) corner (x=segments[0][0][0],  y=segments[0][0][1], z=segments[0][0][2], diameter=0.25, faces=12, thickness_over_width=1);" << endl;
    out << "    for(seg = segments)" << endl;
    out << "    {" << endl;
    out << "        tube(x1=seg[0][0], y1=seg[0][1], z1=seg[0][2], x2=seg[1][0], y2=seg[1][1], z2=seg[1][2] , diameter1=0.1, diameter2=0.05, faces=4, thickness_over_width=1);" << endl;
    out << "    }" << endl;
    out << "}" << endl;

    fscad.writeHeader();

	Scalar z = 0;
	Scalar dz = 0.1;
	for(size_t i=0; i < rayTable.size(); i++)
	{
		std::vector<LineSegment2>& line = rayTable[i];
		fscad.writeSegments3("scan_line_", "linea", line, z, dz, i);

	}

	fscad.writeMinMax("draw", "scan_line_", rayTable.size());

	fscad.close();
}
