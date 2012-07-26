#include <list>
#include <limits>

#include <cppunit/config/SourcePrefix.h>

#include "UnitTestUtils.h"
#include "SlicerSplitTestCase.h"

#include "UnitTestUtils.h"

#include "mgl/mgl.h"
#include "mgl/configuration.h"
#include "mgl/slicy.h"

#include "mgl/shrinky.h"
#include "mgl/meshy.h"

CPPUNIT_TEST_SUITE_REGISTRATION( SlicerSplitTestCase );

using namespace std;
using namespace mgl;
using namespace libthing;

string inputDir("test_cases/slicerCupTestCase/stls/");
string outputDir("outputs/test_cases/SlicerSplitTestCase/");

void SlicerSplitTestCase::setUp()
{
	MyComputer computer;
	char pathsep = computer.fileSystem.getPathSeparatorCharacter();
	inputDir = string("test_cases") + pathsep + string("slicerCupTestCase") + 
			pathsep + string("stls") + pathsep;
	outputDir = string("outputs") + pathsep + string("test_cases") + pathsep + 
			string("SlicerSplitTestCase") + pathsep;
	computer.fileSystem.guarenteeDirectoryExistsRecursive(outputDir.c_str());
}

void SlicerSplitTestCase::test_m()
{
	std::vector<LineSegment2> segs;
	Scalar x = 0;
	Scalar y = 0;

	segs.push_back(LineSegment2(Vector2(0.2+x, 0.2+y), Vector2(0.2+x, 4.63+y)));
	segs.push_back(LineSegment2(Vector2(0.2+x, 4.63+y), Vector2(0.2+x, 4.8+y)));
	segs.push_back(LineSegment2(Vector2(0.2+x, 4.8+y), Vector2(3.52+x, 4.8+y)));
	segs.push_back(LineSegment2(Vector2(3.52+x, 4.8+y), Vector2(4.8+x, 4.8+y)));
	segs.push_back(LineSegment2(Vector2(4.8+x, 4.8+y), Vector2(4.8+x, 4.63+y)));
	segs.push_back(LineSegment2(Vector2(4.8+x, 4.63+y), Vector2(4.8+x, 0.2+y)));
	segs.push_back(LineSegment2(Vector2(4.8+x, 0.2+y), Vector2(4.63+x, 0.2+y)));
	segs.push_back(LineSegment2(Vector2(4.63+x, 0.2+y), Vector2(0.2+x, 0.2+y)));


    Shrinky shrinky;
	Scalar insetDistance = 0.9 * 0.4 * 2;

	std::vector<LineSegment2> finalInsets;

	shrinky.inset(segs, insetDistance , finalInsets);

	cout << "TEST done... verifying" << endl;
	for (unsigned int i=0; i < finalInsets.size(); i++)
	{

		const LineSegment2 &seg = finalInsets[i];
		Scalar l = seg.length();
		cout << "seg[" << i << "] = " << seg << " l = " << l << endl;
		CPPUNIT_ASSERT(l > 0);
	}
}

void SlicerSplitTestCase::test_calibration_slice_70()
{
	Scalar firstLayerZ = 0.1;
	Scalar layerH = 0.27;
	//Scalar layerW = 0.4;
	cout << endl;
	cout << fixed;

	Meshy mesh;
	Segmenter seg(firstLayerZ, layerH);

	string inputFile = inputDir + "ultimate_calibration_test.stl";
	mesh.readStlFile(inputFile.c_str());
	seg.tablaturize(mesh);
	unsigned int sliceCount = seg.readSliceTable().size();
	cout << "Slices " << sliceCount << endl;

	const TriangleIndices & trianglesForSlice = seg.readSliceTable()[70];
	const vector<Triangle3> &allTriangles = mesh.readAllTriangles();
	std::vector<LineSegment2> segments;
	Scalar z = seg.readLayerMeasure().sliceIndexToHeight(70);
	cout  << "z="<< z << endl;
	segmentationOfTriangles(trianglesForSlice, allTriangles, z, segments);

	//int count =0;
	cout << "polyhedron ( points = [ "<< endl;


	index_t triangle_id = trianglesForSlice[33];
	const Triangle3 &triangle = allTriangles[triangle_id];

	cout << "\t" << triangle[0] << ",\t" << triangle[1] << ",\t" << triangle[2] << ","<< endl;

	 // [-25.000000, -0.000002, 19.000000],	[-25.000000, 30.000000, 0.000000],	[-25.000000, -5.000002, 19.000000],
//	Triangle3 t(Vector3());
/*
	for(unsigned int i=0; i < trianglesForSlice.size(); i++)
	{
		index_t triangle_id = trianglesForSlice[i];
		const Triangle3 &triangle = allTriangles[triangle_id];

		Vector3 a, b;
		// bool cut = sliceTriangle(triangle[0], triangle[1], triangle[2], z, a, b);
		bool cut = triangle.cut(z, a, b);

		if(cut)
		{
			cout << "\t" << triangle[0] << ",\t" << triangle[1] << ",\t" << triangle[2] << ","<< endl;
			count++;
		}
		else
		{
			cout << "\t" << triangle[0] << ",\t" << triangle[1] << ",\t" << triangle[2] << ","<< endl;
			count ++;
		}
	}
	cout << "], " << endl;
	cout << "triangles = [" << endl;
	int v =0;
	for(unsigned int i=0; i < count; i++)
	{
		cout << "[" << v << ", ";
		v ++;
		cout << v << ", ";
		v ++;
		cout << v << "],"<< endl;
		v++;
	}
	cout << "]);"<< endl;


	for(unsigned int i=0; i < segments.size(); i++)
	{
		LineSegment2 &seg = segments[i];
		cout << i << "\t" << seg << endl;
	}
	*/
	/*

	Slicy slicy(mesh.readAllTriangles(),
				mesh.readLimits(),
				layerW,
				layerH,
				sliceCount,
				NULL);

	const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[70];

	Scalar z = 0;
	unsigned int sliceId = 0;
	Scalar tubeSpacing  = 1.0;

	// unsigned int
	slicy.slice(trianglesForSlice,
			sliceId,
			unsigned int extruderId,
			Scalar tubeSpacing,
			Scalar sliceAngle,
			unsigned int nbOfShells,
			Scalar cutoffLength,
			Scalar infillShrinking,
			Scalar insetDistanceFactor,
			bool writeDebugScadFiles,
			SliceData &slice)
		*/

}

void SlicerSplitTestCase::test_cath()
{
	cout << endl;

	std::vector<LineSegment2> segs;
	Scalar x = 0;
	Scalar y = 0;

/*
	segs.push_back(LineSegment2(Vector2(-31.60671+x, 9.23628+y), Vector2(-31.60666+x, 9.27745+y)));
	segs.push_back(LineSegment2(Vector2(-31.60666+x, 9.27745+y), Vector2(-31.60271+x, 12.67971+y)));
	segs.push_back(LineSegment2(Vector2(-31.60271+x, 12.67971+y), Vector2(-20.23976+x, 12.66786+y)));
	segs.push_back(LineSegment2(Vector2(-20.23976+x, 12.66786+y), Vector2(-20.11261+x, 12.66774+y)));
	segs.push_back(LineSegment2(Vector2(-20.11261+x, 12.66774+y), Vector2(-20.098+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(-20.098+x, 12.66771+y), Vector2(-19.99037+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(-19.99037+x, 12.66771+y), Vector2(-19.98871+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 12.66771+y), Vector2(-19.98871+x, 12.66991+y)));
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 12.66991+y), Vector2(-19.98871+x, 12.67015+y)));
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 12.67015+y), Vector2(-19.98871+x, 12.67372+y)));
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 12.67372+y), Vector2(-19.98871+x, 14.07857+y)));  // 10
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 14.07857+y), Vector2(-19.98871+x, 14.07971+y)));
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 14.07971+y), Vector2(-17.51451+x, 14.07971+y)));
	segs.push_back(LineSegment2(Vector2(-17.51451+x, 14.07971+y), Vector2(-17.50729+x, 14.07971+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 14.07971+y), Vector2(-17.50729+x, 14.07857+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 14.07857+y), Vector2(-17.50729+x, 12.67372+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 12.67372+y), Vector2(-17.50729+x, 12.67015+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 12.67015+y), Vector2(-17.50729+x, 12.66991+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 12.66991+y), Vector2(-17.50729+x, 12.66771+y)));

	segs.push_back(LineSegment2(Vector2(-17.50729+x, 12.66771+y), Vector2(-17.39837+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(-17.39837+x, 12.66771+y), Vector2(-17.398+x, 12.66771+y)));     // 20
	segs.push_back(LineSegment2(Vector2(-17.398+x, 12.66771+y), Vector2(-16.21208+x, 12.6697+y)));
	segs.push_back(LineSegment2(Vector2(-16.21208+x, 12.6697+y), Vector2(-16.208+x, 12.66971+y)));
	segs.push_back(LineSegment2(Vector2(-16.208+x, 12.66971+y), Vector2(-15.95301+x, 12.66971+y)));
	segs.push_back(LineSegment2(Vector2(-15.95301+x, 12.66971+y), Vector2(4.488+x, 12.66971+y)));
	segs.push_back(LineSegment2(Vector2(4.488+x, 12.66971+y), Vector2(4.49214+x, 12.6697+y)));
	segs.push_back(LineSegment2(Vector2(4.49214+x, 12.6697+y), Vector2(5.674+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(5.674+x, 12.66771+y), Vector2(5.78162+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(5.78162+x, 12.66771+y), Vector2(5.78329+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.66771+y), Vector2(5.78329+x, 12.6701+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.6701+y), Vector2(5.78329+x, 12.68999+y)));   // 30
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.68999+y), Vector2(5.78329+x, 12.69005+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.69005+y), Vector2(5.78329+x, 12.69008+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.69008+y), Vector2(5.78329+x, 12.69352+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.69352+y), Vector2(5.78329+x, 14.01863+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 14.01863+y), Vector2(5.78329+x, 14.02035+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 14.02035+y), Vector2(5.78329+x, 14.01971+y))); // 36
	segs.push_back(LineSegment2(Vector2(5.78329+x, 14.01971+y), Vector2(8.25732+x, 14.01971+y)));
	segs.push_back(LineSegment2(Vector2(8.25732+x, 14.01971+y), Vector2(8.26471+x, 14.01971+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 14.01971+y), Vector2(8.26471+x, 14.0186+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 14.0186+y), Vector2(8.26471+x, 12.67359+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 12.67359+y), Vector2(8.26471+x, 12.6701+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 12.6701+y), Vector2(8.26471+x, 12.66993+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 12.66993+y), Vector2(8.26471+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 12.66771+y), Vector2(8.37362+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(8.37362+x, 12.66771+y), Vector2(8.374+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(8.374+x, 12.66771+y), Vector2(8.38896+x, 12.66774+y)));
	segs.push_back(LineSegment2(Vector2(8.38896+x, 12.66774+y), Vector2(9.564+x, 12.66971+y)));
	segs.push_back(LineSegment2(Vector2(9.564+x, 12.66971+y), Vector2(9.60304+x, 12.66971+y)));

	segs.push_back(LineSegment2(Vector2(9.60304+x, 12.66971+y), Vector2(12.66871+x, 12.66971+y)));
	segs.push_back(LineSegment2(Vector2(12.66871+x, 12.66971+y), Vector2(12.66871+x, 12.63196+y)));
	segs.push_back(LineSegment2(Vector2(12.66871+x, 12.63196+y), Vector2(12.66871+x, 9.565+y)));
	segs.push_back(LineSegment2(Vector2(12.66871+x, 9.565+y), Vector2(12.66871+x, 9.56084+y)));
	segs.push_back(LineSegment2(Vector2(12.66871+x, 9.56084+y), Vector2(12.66671+x, 8.375+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, 8.375+y), Vector2(12.66671+x, 8.37462+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, 8.37462+y), Vector2(12.66671+x, 8.26571+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, 8.26571+y), Vector2(12.66894+x, 8.26571+y)));
	segs.push_back(LineSegment2(Vector2(12.66894+x, 8.26571+y), Vector2(12.66911+x, 8.26571+y)));
	segs.push_back(LineSegment2(Vector2(12.66911+x, 8.26571+y), Vector2(12.6726+x, 8.26571+y)));
	segs.push_back(LineSegment2(Vector2(12.6726+x, 8.26571+y), Vector2(14.0196+x, 8.26571+y)));
	segs.push_back(LineSegment2(Vector2(14.0196+x, 8.26571+y), Vector2(14.02135+x, 8.26571+y)));
	segs.push_back(LineSegment2(Vector2(14.02135+x, 8.26571+y), Vector2(14.02071+x, 8.26571+y)));
	segs.push_back(LineSegment2(Vector2(14.02071+x, 8.26571+y), Vector2(14.02071+x, 5.79168+y)));
	segs.push_back(LineSegment2(Vector2(14.02071+x, 5.79168+y), Vector2(14.02071+x, 5.78429+y)));
	segs.push_back(LineSegment2(Vector2(14.02071+x, 5.78429+y), Vector2(12.69466+x, 5.78429+y)));
	segs.push_back(LineSegment2(Vector2(12.69466+x, 5.78429+y), Vector2(12.69107+x, 5.78429+y)));
	segs.push_back(LineSegment2(Vector2(12.69107+x, 5.78429+y), Vector2(12.69104+x, 5.78429+y)));
	segs.push_back(LineSegment2(Vector2(12.69104+x, 5.78429+y), Vector2(12.69099+x, 5.78429+y)));
	segs.push_back(LineSegment2(Vector2(12.69099+x, 5.78429+y), Vector2(12.66911+x, 5.78429+y)));
	segs.push_back(LineSegment2(Vector2(12.66911+x, 5.78429+y), Vector2(12.66671+x, 5.78429+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, 5.78429+y), Vector2(12.66671+x, 5.78262+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, 5.78262+y), Vector2(12.66671+x, 5.675+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, 5.675+y), Vector2(12.66672+x, 5.67084+y)));
	segs.push_back(LineSegment2(Vector2(12.66672+x, 5.67084+y), Vector2(12.66674+x, 5.66006+y)));
	segs.push_back(LineSegment2(Vector2(12.66674+x, 5.66006+y), Vector2(12.66871+x, 4.487+y)));
	segs.push_back(LineSegment2(Vector2(12.66871+x, 4.487+y), Vector2(12.66871+x, 4.22689+y)));
	segs.push_back(LineSegment2(Vector2(12.66871+x, 4.22689+y), Vector2(12.66871+x, -16.207+y)));
	segs.push_back(LineSegment2(Vector2(12.66871+x, -16.207+y), Vector2(12.66674+x, -17.38431+y)));
	segs.push_back(LineSegment2(Vector2(12.66674+x, -17.38431+y), Vector2(12.66672+x, -17.39491+y)));
	segs.push_back(LineSegment2(Vector2(12.66672+x, -17.39491+y), Vector2(12.66671+x, -17.399+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, -17.399+y), Vector2(12.66671+x, -17.50663+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, -17.50663+y), Vector2(12.66671+x, -17.50829+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, -17.50829+y), Vector2(14.07716+x, -17.50829+y)));
	segs.push_back(LineSegment2(Vector2(14.07716+x, -17.50829+y), Vector2(14.08071+x, -17.50829+y)));
	segs.push_back(LineSegment2(Vector2(14.08071+x, -17.50829+y), Vector2(14.08071+x, -17.5155+y)));
	segs.push_back(LineSegment2(Vector2(14.08071+x, -17.5155+y), Vector2(14.08071+x, -19.98771+y)));
	segs.push_back(LineSegment2(Vector2(14.08071+x, -19.98771+y), Vector2(14.07957+x, -19.98771+y)));
	segs.push_back(LineSegment2(Vector2(14.07957+x, -19.98771+y), Vector2(12.67274+x, -19.98771+y)));
	segs.push_back(LineSegment2(Vector2(12.67274+x, -19.98771+y), Vector2(12.66915+x, -19.98771+y)));
	segs.push_back(LineSegment2(Vector2(12.66915+x, -19.98771+y), Vector2(12.66891+x, -19.98771+y)));
	segs.push_back(LineSegment2(Vector2(12.66891+x, -19.98771+y), Vector2(12.66671+x, -19.98771+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, -19.98771+y), Vector2(12.66671+x, -19.98937+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, -19.98937+y), Vector2(12.66671+x, -20.097+y)));
	segs.push_back(LineSegment2(Vector2(12.66671+x, -20.097+y), Vector2(12.66672+x, -20.10107+y)));
	segs.push_back(LineSegment2(Vector2(12.66672+x, -20.10107+y), Vector2(12.66674+x, -20.11161+y)));
	segs.push_back(LineSegment2(Vector2(12.66674+x, -20.11161+y), Vector2(12.68056+x, -31.47783+y)));
	segs.push_back(LineSegment2(Vector2(12.68056+x, -31.47783+y), Vector2(12.68071+x, -31.60371+y)));
	segs.push_back(LineSegment2(Vector2(12.68071+x, -31.60371+y), Vector2(9.27846+x, -31.60569+y)));
	segs.push_back(LineSegment2(Vector2(9.27846+x, -31.60569+y), Vector2(9.23729+x, -31.60571+y)));
	segs.push_back(LineSegment2(Vector2(9.23729+x, -31.60571+y), Vector2(9.23714+x, -31.47985+y)));
	segs.push_back(LineSegment2(Vector2(9.23714+x, -31.47985+y), Vector2(9.22134+x, -20.11561+y)));
	segs.push_back(LineSegment2(Vector2(9.22134+x, -20.11561+y), Vector2(9.22129+x, -20.101+y)));
	segs.push_back(LineSegment2(Vector2(9.2209+x, -20.10008+y), Vector2(9.10929+x, -19.99046+y)));
	segs.push_back(LineSegment2(Vector2(9.10929+x, -19.99046+y), Vector2(9.1089+x, -19.99008+y)));
	segs.push_back(LineSegment2(Vector2(9.108+x, -19.98971+y), Vector2(7.54139+x, -19.98772+y)));
	segs.push_back(LineSegment2(Vector2(7.54139+x, -19.98772+y), Vector2(7.53729+x, -19.98771+y)));
	segs.push_back(LineSegment2(Vector2(7.53729+x, -19.98771+y), Vector2(7.53729+x, -19.9805+y)));
	segs.push_back(LineSegment2(Vector2(7.53729+x, -19.9805+y), Vector2(7.53729+x, -17.50829+y)));
	segs.push_back(LineSegment2(Vector2(7.53729+x, -17.50829+y), Vector2(9.10261+x, -17.50829+y)));
	segs.push_back(LineSegment2(Vector2(9.10261+x, -17.50829+y), Vector2(9.108+x, -17.50829+y)));
	segs.push_back(LineSegment2(Vector2(9.10889+x, -17.50793+y), Vector2(9.2245+x, -17.39631+y)));
	segs.push_back(LineSegment2(Vector2(9.2245+x, -17.39631+y), Vector2(9.22529+x, -17.39555+y)));
	segs.push_back(LineSegment2(Vector2(9.22529+x, -17.39555+y), Vector2(9.22529+x, -17.39092+y)));
	segs.push_back(LineSegment2(Vector2(9.22529+x, -17.39092+y), Vector2(9.22529+x, -17.38034+y)));
	segs.push_back(LineSegment2(Vector2(9.22529+x, -17.38034+y), Vector2(9.22529+x, -16.205+y)));
	segs.push_back(LineSegment2(Vector2(9.22529+x, -16.205+y), Vector2(9.22529+x, -15.95008+y)));
	segs.push_back(LineSegment2(Vector2(9.22529+x, -15.95008+y), Vector2(9.22529+x, 4.485+y)));
	segs.push_back(LineSegment2(Vector2(9.22529+x, 4.485+y), Vector2(9.22529+x, 4.48915+y)));
	segs.push_back(LineSegment2(Vector2(9.22529+x, 4.48915+y), Vector2(9.22529+x, 5.67155+y)));
	segs.push_back(LineSegment2(Vector2(9.22529+x, 5.67155+y), Vector2(9.1093+x, 5.78353+y)));
	segs.push_back(LineSegment2(Vector2(9.1093+x, 5.78353+y), Vector2(9.10889+x, 5.78393+y)));
	segs.push_back(LineSegment2(Vector2(9.108+x, 5.78429+y), Vector2(7.54149+x, 5.78429+y)));
	segs.push_back(LineSegment2(Vector2(7.54149+x, 5.78429+y), Vector2(7.53729+x, 5.78429+y)));
	segs.push_back(LineSegment2(Vector2(7.53729+x, 5.78429+y), Vector2(7.53729+x, 5.78913+y)));
	segs.push_back(LineSegment2(Vector2(7.53729+x, 5.78913+y), Vector2(7.53729+x, 7.53829+y)));
	segs.push_back(LineSegment2(Vector2(7.53729+x, 7.53829+y), Vector2(7.52987+x, 7.53829+y)));
	segs.push_back(LineSegment2(Vector2(7.52987+x, 7.53829+y), Vector2(5.78329+x, 7.53829+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 7.53829+y), Vector2(5.78329+x, 7.54249+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 7.54249+y), Vector2(5.78329+x, 9.107+y)));
	segs.push_back(LineSegment2(Vector2(5.78293+x, 9.10789+y), Vector2(5.78255+x, 9.10829+y)));
	segs.push_back(LineSegment2(Vector2(5.78255+x, 9.10829+y), Vector2(5.67255+x, 9.22429+y)));
	segs.push_back(LineSegment2(Vector2(5.67255+x, 9.22429+y), Vector2(4.48815+x, 9.22628+y)));
	segs.push_back(LineSegment2(Vector2(4.48815+x, 9.22628+y), Vector2(4.484+x, 9.22629+y)));
	segs.push_back(LineSegment2(Vector2(4.484+x, 9.22629+y), Vector2(-15.94911+x, 9.22629+y)));
	segs.push_back(LineSegment2(Vector2(-15.94911+x, 9.22629+y), Vector2(-16.204+x, 9.22629+y)));
	segs.push_back(LineSegment2(Vector2(-16.204+x, 9.22629+y), Vector2(-16.20809+x, 9.22628+y)));
	segs.push_back(LineSegment2(Vector2(-16.20809+x, 9.22628+y), Vector2(-17.39655+x, 9.22429+y)));
	segs.push_back(LineSegment2(Vector2(-17.39655+x, 9.22429+y), Vector2(-17.50656+x, 9.10828+y)));
	segs.push_back(LineSegment2(Vector2(-17.50656+x, 9.10828+y), Vector2(-17.50693+x, 9.10789+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 9.107+y), Vector2(-17.50729+x, 7.54238+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 7.54238+y), Vector2(-17.50729+x, 7.53829+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 7.53829+y), Vector2(-17.51451+x, 7.53829+y)));
	segs.push_back(LineSegment2(Vector2(-17.51451+x, 7.53829+y), Vector2(-19.98871+x, 7.53829+y)));
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 7.53829+y), Vector2(-19.98871+x, 7.54238+y)));
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 7.54238+y), Vector2(-19.98871+x, 9.107+y)));
	segs.push_back(LineSegment2(Vector2(-19.98909+x, 9.10791+y), Vector2(-19.98947+x, 9.10829+y)));
	segs.push_back(LineSegment2(Vector2(-19.98947+x, 9.10829+y), Vector2(-20.10109+x, 9.21991+y)));
	segs.push_back(LineSegment2(Vector2(-20.10199+x, 9.22029+y), Vector2(-20.11661+x, 9.22036+y)));
	segs.push_back(LineSegment2(Vector2(-20.11661+x, 9.22036+y), Vector2(-20.24376+x, 9.22048+y)));
	segs.push_back(LineSegment2(Vector2(-20.24376+x, 9.22048+y), Vector2(-31.60671+x, 9.23628+y)));
*/

/*

	segs.push_back(LineSegment2(Vector2(-19.98871+x, 12.67372+y), Vector2(-19.98871+x, 14.07857+y)));  // 10
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 14.07857+y), Vector2(-19.98871+x, 14.07971+y)));
	segs.push_back(LineSegment2(Vector2(-19.98871+x, 14.07971+y), Vector2(-17.51451+x, 14.07971+y)));
	segs.push_back(LineSegment2(Vector2(-17.51451+x, 14.07971+y), Vector2(-17.50729+x, 14.07971+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 14.07971+y), Vector2(-17.50729+x, 14.07857+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 14.07857+y), Vector2(-17.50729+x, 12.67372+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 12.67372+y), Vector2(-17.50729+x, 12.67015+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 12.67015+y), Vector2(-17.50729+x, 12.66991+y)));
	segs.push_back(LineSegment2(Vector2(-17.50729+x, 12.66991+y), Vector2(-17.50729+x, 12.66771+y)));

	segs.push_back(LineSegment2(Vector2(-17.50729+x, 12.66771+y), Vector2(-17.39837+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(-17.39837+x, 12.66771+y), Vector2(-17.398+x, 12.66771+y)));     // 20
	segs.push_back(LineSegment2(Vector2(-17.398+x, 12.66771+y), Vector2(-16.21208+x, 12.6697+y)));
	segs.push_back(LineSegment2(Vector2(-16.21208+x, 12.6697+y), Vector2(-16.208+x, 12.66971+y)));
	segs.push_back(LineSegment2(Vector2(-16.208+x, 12.66971+y), Vector2(-15.95301+x, 12.66971+y)));
	segs.push_back(LineSegment2(Vector2(-15.95301+x, 12.66971+y), Vector2(4.488+x, 12.66971+y)));
	segs.push_back(LineSegment2(Vector2(4.488+x, 12.66971+y), Vector2(4.49214+x, 12.6697+y)));
	segs.push_back(LineSegment2(Vector2(4.49214+x, 12.6697+y), Vector2(5.674+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(5.674+x, 12.66771+y), Vector2(5.78162+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(5.78162+x, 12.66771+y), Vector2(5.78329+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.66771+y), Vector2(5.78329+x, 12.6701+y)));


*/

	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.6701+y), Vector2(5.78329+x, 12.68999+y)));   // 30
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.68999+y), Vector2(5.78329+x, 12.69005+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.69005+y), Vector2(5.78329+x, 12.69008+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.69008+y), Vector2(5.78329+x, 12.69352+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 12.69352+y), Vector2(5.78329+x, 14.01863+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 14.01863+y), Vector2(5.78329+x, 14.02035+y)));
	segs.push_back(LineSegment2(Vector2(5.78329+x, 14.02035+y), Vector2(5.78329+x, 14.01971+y))); // 36
	segs.push_back(LineSegment2(Vector2(5.78329+x, 14.01971+y), Vector2(8.25732+x, 14.01971+y)));
	segs.push_back(LineSegment2(Vector2(8.25732+x, 14.01971+y), Vector2(8.26471+x, 14.01971+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 14.01971+y), Vector2(8.26471+x, 14.0186+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 14.0186+y), Vector2(8.26471+x, 12.67359+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 12.67359+y), Vector2(8.26471+x, 12.6701+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 12.6701+y), Vector2(8.26471+x, 12.66993+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 12.66993+y), Vector2(8.26471+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(8.26471+x, 12.66771+y), Vector2(8.37362+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(8.37362+x, 12.66771+y), Vector2(8.374+x, 12.66771+y)));
	segs.push_back(LineSegment2(Vector2(8.374+x, 12.66771+y), Vector2(8.38896+x, 12.66774+y)));
	segs.push_back(LineSegment2(Vector2(8.38896+x, 12.66774+y), Vector2(9.564+x, 12.66971+y)));
	segs.push_back(LineSegment2(Vector2(9.564+x, 12.66971+y), Vector2(9.60304+x, 12.66971+y)));


	segs.push_back(LineSegment2(Vector2(9.60304+x, 12.66971+y) , Vector2(5.78329+x, 12.6701+y)));



	Shrinky shrinky("test_cath.scad");
	Scalar insetDistance = 0.9 * 0.4 * 2;

	std::vector<LineSegment2> finalInsets;

	try
	{
		shrinky.inset(segs, insetDistance , finalInsets);
	}
	catch(mgl::Exception &e)
	{
		cout << "ERROR" << endl;
		cout << e.error << endl;
	}
	cout << "TEST done... verifying" << endl;
	for (unsigned int i=0; i < finalInsets.size(); i++)
	{

		const LineSegment2 &seg = finalInsets[i];
		Scalar l = seg.length();
		cout << "seg[" << i << "] = " << seg << " l = " << l << endl;
		CPPUNIT_ASSERT(l > 0);
	}
}

void SlicerSplitTestCase::test_ultimate_59()
{
	cout << endl;
	std::vector<LineSegment2> segs;
	//Scalar x = 0;
	//Scalar y = 0;

	segs.push_back(LineSegment2(Vector2(22.0, 18.86667), Vector2(22.0, 21.65)));
	segs.push_back(LineSegment2(Vector2(22.0, 21.65), Vector2(22.0, 30.0)));
	segs.push_back(LineSegment2(Vector2(22.0, 30.0), Vector2(23.503, 30.0)));
	segs.push_back(LineSegment2(Vector2(23.503, 30.0), Vector2(25.0, 30.0)));
	segs.push_back(LineSegment2(Vector2(25.0, 30.0), Vector2(25.0, 22.24355)));
	segs.push_back(LineSegment2(Vector2(25.0, 22.24355), Vector2(25.0, 21.4125)));
	segs.push_back(LineSegment2(Vector2(25.0, 21.4125), Vector2(25.0, 18.55)));
	segs.push_back(LineSegment2(Vector2(25.0, 18.55), Vector2(25.0, 17.176)));
	segs.push_back(LineSegment2(Vector2(25.0, 17.176), Vector2(25.0, 11.78409)));
	segs.push_back(LineSegment2(Vector2(25.0, 11.78409), Vector2(25.0, 8.14091)));
	segs.push_back(LineSegment2(Vector2(25.0, 8.14091), Vector2(25.0, 4.68947)));
	segs.push_back(LineSegment2(Vector2(25.0, 4.68947), Vector2(25.0, 0.47105)));
	segs.push_back(LineSegment2(Vector2(25.0, 0.47105), Vector2(25.0, -5.0)));
	segs.push_back(LineSegment2(Vector2(25.0, -5.0), Vector2(24.97, -5.0)));
	segs.push_back(LineSegment2(Vector2(24.97, -5.0), Vector2(22.0, -5.0)));
	segs.push_back(LineSegment2(Vector2(22.0, -5.0), Vector2(22.0, 0.775)));
	segs.push_back(LineSegment2(Vector2(22.0, 0.775), Vector2(22.0, 4.95)));
	segs.push_back(LineSegment2(Vector2(22.0, 4.95), Vector2(22.0, 12.10714)));
	segs.push_back(LineSegment2(Vector2(22.0, 12.10714), Vector2(22.0, 14.34375)));
	segs.push_back(LineSegment2(Vector2(22.0, 14.34375), Vector2(22.0, 17.475)));
	segs.push_back(LineSegment2(Vector2(22.0, 17.475), Vector2(22.0, 18.86667)));

	std::vector<LineSegment2> segs2;
	clip(segs, 1, 10, segs2);

	string output = outputDir + "test_ultimate_59.scad";

	Shrinky shrinky(output.c_str());
	Scalar insetDistance = 0.9 * 0.4 * 2;

	std::vector<LineSegment2> finalInsets;

	try
	{
		shrinky.inset(segs2, insetDistance , finalInsets);
	}
	catch(mgl::Exception &e)
	{
		cout << "ERROR" << endl;
		cout << e.error << endl;
	}
	cout << "TEST done... verifying" << endl;
	for (unsigned int i=0; i < finalInsets.size(); i++)
	{
		const LineSegment2 &seg = finalInsets[i];
		Scalar l = seg.length();
		cout << "seg[" << i << "] = " << seg << " l = " << l << endl;
		CPPUNIT_ASSERT(l > 0);
	}

}
