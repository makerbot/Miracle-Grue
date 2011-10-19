#include "BGLBounds.h"

namespace BGL {

const Scalar Bounds::NONE = -9e9;

void Bounds::expand(const Point& pt)
{
    if (minX == NONE) {
        minX = maxX = pt.x;
        minY = maxY = pt.y;
	return;
    }
    if (pt.x < minX) {
        minX = pt.x;
    }
    if (pt.x > maxX) {
        maxX = pt.x;
    }
    if (pt.y < minY) {
        minY = pt.y;
    }
    if (pt.y > maxY) {
        maxY = pt.y;
    }
}


}



