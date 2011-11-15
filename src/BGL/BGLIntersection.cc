#include "BGLIntersection.h"
#include "BGLLine.h"

namespace BGL {

Line Intersection::line() const
{
    return Line(p1, p2);
}



void Intersection::quantize(float quanta) {
    if (type != NONE) {
	p1.quantize(quanta);
	if (type != POINT) {
	    p2.quantize(quanta);
	}
    }
}



void Intersection::quantize() {
    if (type != NONE) {
	p1.quantize();
	if (type != POINT) {
	    p2.quantize();
	}
    }
}

}


