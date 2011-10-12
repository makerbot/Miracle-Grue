#include "BGLIntersection.h"
#include "BGLLine.h"

namespace BGL {
    Line Intersection::line() const
    {
        return Line(p1, p2);
    }

}


