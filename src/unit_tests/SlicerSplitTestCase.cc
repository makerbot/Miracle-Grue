#include <list>
#include <limits>

#include <cppunit/config/SourcePrefix.h>
#include "SlicerSplitTestCase.h"

#include "mgl/core.h"
#include "mgl/configuration.h"
#include "mgl/slicy.h"

#include "mgl/shrinky.h"
#include "mgl/meshy.h"

CPPUNIT_TEST_SUITE_REGISTRATION( SlicerSplitTestCase );

using namespace std;
using namespace mgl;




void SlicerSplitTestCase::test_m()
{
	std::vector<TriangleSegment2> segs;
	Scalar x = 0;
	Scalar y = 0;

	segs.push_back(TriangleSegment2(Vector2(0.2+x, 0.2+y), Vector2(0.2+x, 4.63+y)));
	segs.push_back(TriangleSegment2(Vector2(0.2+x, 4.63+y), Vector2(0.2+x, 4.8+y)));
	segs.push_back(TriangleSegment2(Vector2(0.2+x, 4.8+y), Vector2(3.52+x, 4.8+y)));
	segs.push_back(TriangleSegment2(Vector2(3.52+x, 4.8+y), Vector2(4.8+x, 4.8+y)));
	segs.push_back(TriangleSegment2(Vector2(4.8+x, 4.8+y), Vector2(4.8+x, 4.63+y)));
	segs.push_back(TriangleSegment2(Vector2(4.8+x, 4.63+y), Vector2(4.8+x, 0.2+y)));
	segs.push_back(TriangleSegment2(Vector2(4.8+x, 0.2+y), Vector2(4.63+x, 0.2+y)));
	segs.push_back(TriangleSegment2(Vector2(4.63+x, 0.2+y), Vector2(0.2+x, 0.2+y)));


    Shrinky shrinky;
	Scalar insetDistance = 0.9 * 0.4 * 2;

	std::vector<TriangleSegment2> finalInsets;

	shrinky.inset(segs, insetDistance , finalInsets);

	cout << "TEST done... verifying" << endl;
	for (unsigned int i=0; i < finalInsets.size(); i++)
	{

		const TriangleSegment2 &seg = finalInsets[i];
		Scalar l = seg.length();
		cout << "seg[" << i << "] = " << seg << " l = " << l << endl;
		CPPUNIT_ASSERT(l > 0);
	}
}

void SlicerSplitTestCase::test_calibration_slice_70()
{
	Scalar firstLayerZ = 0.1;
	Scalar layerH = 0.27;
	Scalar layerW = 0.4;
	cout << endl;
	cout << fixed;

	Meshy mesh(firstLayerZ, layerH);

	loadMeshyFromStl(mesh, "test_cases/slicerCupTestCase/stls/ultimate_calibration_test.stl");
	unsigned int sliceCount = mesh.readSliceTable().size();
	cout << "Slices " << sliceCount << endl;

	const TriangleIndices & trianglesForSlice = mesh.readSliceTable()[70];
	const vector<mgl::Triangle3> &allTriangles = mesh.readAllTriangles();
	std::vector<TriangleSegment2> segments;
	Scalar z = mesh.readLayerMeasure().sliceIndexToHeight(70);
	cout  << "z="<< z << endl;
	segmentationOfTriangles(trianglesForSlice, allTriangles, z, segments);

	int count =0;
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
		TriangleSegment2 &seg = segments[i];
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
			z,
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


