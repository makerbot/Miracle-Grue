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

void addValues(Scalar min, Scalar max, Scalar delta, std::vector<Scalar>& values)
{
	Scalar value = min;
	while(value <= max)
	{
		values.push_back(value);
		value += delta;
	}

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

void intersection(const SegmentTable& one, const SegmentTable& two, SegmentTable &intersection)
{

}

void RoofingTestCase::testBoolean()
{
	SegmentTable slice0;
	slice0.push_back(std::vector<LineSegment2>());

	SegmentTable slice1;
	slice0.push_back(std::vector<LineSegment2>());

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
	intersection(top, bottom, botTop);

	string filename = outputDir + "bot_top.scad";
	ScadDebugFile fscad;
	fscad.open(filename.c_str());
	addLinea(fscad);
    fscad.writeHeader();


	Scalar z = 0;
	Scalar dz = 0;

	writeScanLines(fscad, "intersection_", "linea", -1, -0.1, botTop );
    writeScanLines(fscad, "top", "linea", z, dz, top );
    writeScanLines(fscad, "bottom", "linea", 0.5, dz, bottom );

    std::ostream & out = fscad.getOut();

    out << "intersection_all();" << endl;
    out << "top_all();" << endl;
    out << "bottom_all();" << endl;
    fscad.close();
}



