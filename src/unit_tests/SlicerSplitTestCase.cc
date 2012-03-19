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



