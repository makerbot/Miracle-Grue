#include <set>

#include "infill.h"


using namespace std;
using namespace mgl;

//bool shrinkLine(Vector2 &left, Vector2 &right, Scalar shrinkDistance)
//{
//	Vector2 l = right -left;
//	if(l.magnitude() > 2 * shrinkDistance)
//	{
//		left[0]  +=  shrinkDistance;
//		right[0] -=  shrinkDistance;
//		return true;
//	}
//	return false;
//}


void mgl::infillosophy( const  SegmentTable &outlineLoops,
                        const Limits& limits,
                        Scalar, //z,
                        Scalar layerW,
                        size_t skipCount,
                        bool xDirection, // y when false
                        Scalar, // infillShrinking,
                        Polygons& infills)
{
    Grid grid(limits, layerW);

    GridRanges gridRanges;
    grid.createGridRanges(outlineLoops, gridRanges);

    GridRanges infillRanges;
    grid.subSample(gridRanges, skipCount, infillRanges);

    grid.polygonsFromRanges(infillRanges, xDirection, infills);
}
